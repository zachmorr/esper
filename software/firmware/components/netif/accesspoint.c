#include "accesspoint.h"
#include "storage.h"

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "lwip/netdb.h"

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"
static const char *TAG = "AP";

static EventGroupHandle_t s_wifi_event_group;
const int SCAN_FINISHED_BIT = BIT0;
const int DISCONNECTED_BIT = BIT1;
const int CONNECTED_BIT = BIT2;

static uint16_t ap_count = 0;
static wifi_ap_record_t *ap_list;

static void store_scan_results();

static void apsta_wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    switch(event_id) {
        case SYSTEM_EVENT_AP_STACONNECTED:
        {
            wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
            ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                    MAC2STR(event->mac), event->aid);
            break;
        }
        case SYSTEM_EVENT_AP_STADISCONNECTED:
        {
            wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
            ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d",
                     MAC2STR(event->mac), event->aid);
            break;
        }
        case SYSTEM_EVENT_AP_STAIPASSIGNED: 
            ESP_LOGI(TAG, "SYSTEM_EVENT_AP_STAIPASSIGNED");
            break;
        case SYSTEM_EVENT_AP_PROBEREQRECVED:
            ESP_LOGI(TAG, "SYSTEM_EVENT_AP_PROBEREQRECVED");
            break;
        case SYSTEM_EVENT_SCAN_DONE:
            ESP_LOGI(TAG, "SYSTEM_EVENT_SCAN_DONE");
            store_scan_results();
            xEventGroupSetBits(s_wifi_event_group, SCAN_FINISHED_BIT);
            break;
        case SYSTEM_EVENT_STA_START:
            ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START");
            break;
        case SYSTEM_EVENT_STA_CONNECTED:
            ESP_LOGI(TAG, "SYSTEM_EVENT_STA_CONNECTED");
            break;
	    case SYSTEM_EVENT_STA_DISCONNECTED:
        {
            wifi_event_sta_disconnected_t* event = (wifi_event_sta_disconnected_t*) event_data;
            ESP_LOGI(TAG, "SYSTEM_EVENT_STA_DISCONNECTED (%d)", event->reason);
            xEventGroupSetBits(s_wifi_event_group, DISCONNECTED_BIT);
            break;
        }
        default:
            break;
    }
}

static void apsta_ip_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
    switch(event_id) {
        case IP_EVENT_STA_GOT_IP:
            ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");

            char ip[IP4ADDR_STRLEN_MAX];
            inet_ntop(AF_INET, &event->ip_info.ip, ip, IP4ADDR_STRLEN_MAX);
            nvs_set("ip", (void*)ip, (size_t)IP4ADDR_STRLEN_MAX);
            
            inet_ntop(AF_INET, &event->ip_info.netmask, ip, IP4ADDR_STRLEN_MAX);
            nvs_set("nm", (void*)ip, (size_t)IP4ADDR_STRLEN_MAX);
            
            inet_ntop(AF_INET, &event->ip_info.gw, ip, IP4ADDR_STRLEN_MAX);
            nvs_set("gw", (void*)ip, (size_t)IP4ADDR_STRLEN_MAX);

            xEventGroupSetBits(s_wifi_event_group, CONNECTED_BIT);
            break;
        default:
            break;
    }
}

bool test_authentication()
{
    ESP_LOGI(TAG, "Attempting To Connect");

    // Disconnect if already connected to AP
    if(CONNECTED_BIT & xEventGroupGetBits(s_wifi_event_group))
    {
        ESP_ERROR_CHECK(esp_wifi_disconnect());
        xEventGroupWaitBits(s_wifi_event_group, DISCONNECTED_BIT, 
                                pdFALSE, pdFALSE, portMAX_DELAY);
    }
    xEventGroupClearBits(s_wifi_event_group, CONNECTED_BIT | DISCONNECTED_BIT);


    // Try connection x3
    // Does not work properly, only retries twice. Likely a timing issue
    for(int i = 0; i < 3; i++)
    {
        // This is at the beginning so bits don't get cleared in the last iteration
        xEventGroupClearBits(s_wifi_event_group, DISCONNECTED_BIT); 

        ESP_ERROR_CHECK(esp_wifi_connect());
        uint32_t status = xEventGroupWaitBits(s_wifi_event_group, CONNECTED_BIT | DISCONNECTED_BIT, 
                                                pdFALSE, pdFALSE, portMAX_DELAY);
        
        if( (status & CONNECTED_BIT) )
        {
            ESP_ERROR_CHECK(esp_wifi_disconnect());
            return true;
        }
        
    }
    return false;
}

esp_err_t start_wifi_scan()
{
    static wifi_scan_config_t scanConf = {
        .ssid = NULL,
        .bssid = NULL,
        .channel = 0,
        .show_hidden = 1,
        .scan_type = WIFI_SCAN_TYPE_ACTIVE,
		.scan_time.active.min = 120,
		.scan_time.active.max = 150,
    };

    ESP_LOGD(TAG, "Starting Wifi Scan...");
    xEventGroupClearBits(s_wifi_event_group, SCAN_FINISHED_BIT);
    esp_err_t err = esp_wifi_scan_start(&scanConf, 0);
    if(err != ESP_OK )
    {
        ESP_LOGD(TAG, "Problem starting scan, error code %d", err);
        return ESP_FAIL;
    }
    return ESP_OK;
}

static void store_scan_results()
{
    uint16_t count = 0;
    wifi_ap_record_t *list;

    // get scan results, store them in temporary variables
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&count));
    if(count > 0)
    {
        list = (wifi_ap_record_t *)malloc(count * sizeof(wifi_ap_record_t));
        ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&count, list));

        // count the number of non-empty ssid, then allocate space for them
        ap_count = 0;
        for(int i = 0; i < count; i++)
        {
            if(strlen((char*)list[i].ssid) > 1 && list[i].rssi > -80) ap_count++;

        }
        ESP_LOGD(TAG, "Found %d networks", ap_count);
        
        if(ap_list) free(ap_list);
        ap_list = (wifi_ap_record_t *)malloc(ap_count * sizeof(wifi_ap_record_t));

        // add non-empty scan results to ap_list
        int temp = 0;
        for(int i = 0; i < count; i++)
        {
            if(strlen((char*)list[i].ssid) > 1 && list[i].rssi > -80)
            {
                ap_list[temp] = list[i];
                temp++;
            }
        }

        // print ap_list
        for(int i = 0; i < ap_count; i++)
        {
            ESP_LOGD(TAG, "%s  %4d", ap_list[i].ssid, ap_list[i].rssi);
        }
        free(list);
    }
}

uint16_t get_scan_results(wifi_ap_record_t** list)
{
    xEventGroupWaitBits(s_wifi_event_group, SCAN_FINISHED_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
    *list = ap_list;
    return ap_count;
}

esp_err_t wifi_init_apsta()
{
    s_wifi_event_group = xEventGroupCreate();
    esp_err_t err = esp_netif_init();
    err |= esp_event_loop_create_default();
    err |= esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &apsta_wifi_event_handler, NULL);
    err |= esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &apsta_ip_event_handler, NULL);
    if( err != ESP_OK )
        return ESP_FAIL;

    esp_netif_create_default_wifi_sta();
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t wifi_cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    err = esp_wifi_init(&wifi_cfg);
    if( err != ESP_OK )
        return ESP_FAIL;

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = AP_SSID,
            .ssid_len = strlen(AP_SSID),
            .channel = 1,
            .password = AP_PASS,
            .max_connection = 10,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    if (strlen(AP_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    err = esp_wifi_set_mode(WIFI_MODE_APSTA);
    err |= esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config);
    err |= esp_wifi_start();
    if( err != ESP_OK )
        return ESP_FAIL;

    ESP_LOGI(TAG, "wifi_init_apsta finished.SSID:%s password:%s", AP_SSID, AP_PASS);

    err = start_wifi_scan();
    if( err != ESP_OK )
        return ESP_FAIL;
        
    return ESP_OK;
}

