#include "wifi.h"
#include "error.h"
#include "string.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/event_groups.h"

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"
static const char *TAG = "WIFI";

static EventGroupHandle_t wifi_event_group;
const int SCAN_FINISHED = BIT0;
const int DISCONNECTED = BIT1;
const int CONNECTED = BIT2;

static wifi_ap_record_t ap_list[MAX_SCAN_RECORDS];

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    switch (event_id) {
        case WIFI_EVENT_WIFI_READY:
        {
            ESP_LOGI(TAG, "WIFI_EVENT_WIFI_READY");
            break;
        }
        case WIFI_EVENT_SCAN_DONE:{
            ESP_LOGI(TAG, "WIFI_EVENT_SCAN_DONE");
            // wifi_event_sta_scan_done_t* event = (wifi_event_sta_scan_done_t*)event_data;
            uint16_t ap_count = MAX_SCAN_RECORDS;
            esp_wifi_scan_get_ap_records(&ap_count, ap_list);
            xEventGroupSetBits(wifi_event_group, SCAN_FINISHED);
            break;
        }
        case WIFI_EVENT_STA_START:
        {
            ESP_LOGI(TAG, "WIFI_EVENT_STA_START");
            break;
        }
        case WIFI_EVENT_STA_STOP:
        {
            ESP_LOGI(TAG, "WIFI_EVENT_STA_STOP");
            break;
        }
        case WIFI_EVENT_STA_CONNECTED:
        {
            ESP_LOGI(TAG, "WIFI_EVENT_STA_CONNECTED");
            // (wifi_event_sta_connected_t*)event_data;
            xEventGroupSetBits(wifi_event_group, CONNECTED);
            break;
        }
        case WIFI_EVENT_STA_DISCONNECTED:
        {
            wifi_event_sta_disconnected_t* event = (wifi_event_sta_disconnected_t*) event_data;
            ESP_LOGW(TAG, "WIFI_EVENT_STA_DISCONNECTED %d", event->reason);
            xEventGroupSetBits(wifi_event_group, DISCONNECTED);
            // wifi_err_reason_t err;
            break;
        }
        case WIFI_EVENT_STA_AUTHMODE_CHANGE:
        {
            ESP_LOGI(TAG, "WIFI_EVENT_STA_AUTHMODE_CHANGE");
            // (wifi_event_sta_authmode_change_t*)event_data;
            break;
        }
        case WIFI_EVENT_AP_START:
        {
            ESP_LOGI(TAG, "WIFI_EVENT_AP_START");
            break;
        }
        case WIFI_EVENT_AP_STOP:
        {
            ESP_LOGI(TAG, "WIFI_EVENT_AP_STOP");
            break;
        }
        case WIFI_EVENT_AP_STACONNECTED:
        {
            ESP_LOGI(TAG, "WIFI_EVENT_AP_STACONNECTED");
            wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
            ESP_LOGI(TAG, "station "MACSTR" join, AID=%d", MAC2STR(event->mac), event->aid);
            break;
        }
        case WIFI_EVENT_AP_STADISCONNECTED:
        {
            ESP_LOGW(TAG, "WIFI_EVENT_AP_STADISCONNECTED");
            wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
            ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d", MAC2STR(event->mac), event->aid);
            break;
        }
        case WIFI_EVENT_AP_PROBEREQRECVED:
        {
            ESP_LOGI(TAG, "WIFI_EVENT_AP_PROBEREQRECVED");
            // (wifi_event_ap_probe_req_rx_t*)event_data;
            break;
        }
        default:
            break;
    }
}

esp_err_t init_wifi_sta_netif(esp_netif_t** sta_netif)
{
    esp_netif_config_t sta_cfg = {                                                 
        .base = ESP_NETIF_BASE_DEFAULT_WIFI_STA,      
        .driver = NULL,                               
        .stack = ESP_NETIF_NETSTACK_DEFAULT_WIFI_STA, 
    };

    *sta_netif = esp_netif_new(&sta_cfg);
    if( !*sta_netif )
        return ESP_FAIL;
    
    ERROR_CHECK(esp_netif_attach_wifi_station(*sta_netif))
    ERROR_CHECK(esp_wifi_set_default_wifi_sta_handlers())
    // ERROR_CHECK(update_sta_config())
    return ESP_OK;
}

esp_err_t init_wifi_ap_netif(esp_netif_t** ap_netif)
{
    esp_netif_config_t ap_cfg = {                                                 
        .base = ESP_NETIF_BASE_DEFAULT_WIFI_AP,      
        .driver = NULL,                               
        .stack = ESP_NETIF_NETSTACK_DEFAULT_WIFI_AP, 
    };

    *ap_netif = esp_netif_new(&ap_cfg);
    if( !*ap_netif )
        return ESP_FAIL;
    
    ERROR_CHECK(esp_netif_attach_wifi_ap(*ap_netif))
    ERROR_CHECK(esp_wifi_set_default_wifi_ap_handlers())

    wifi_config_t ap_config = {
        .ap = {
            .ssid = CONFIG_AP_SSID,
            .ssid_len = strlen(CONFIG_AP_SSID),
            .channel = 1,
            .password = CONFIG_AP_PASSWORD,
            .max_connection = CONFIG_AP_CONNECTIONS,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    if (strlen(CONFIG_AP_PASSWORD) == 0) {
        ap_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_LOGI(TAG, "AP SSID (%s) PASS (%s)", ap_config.ap.ssid, ap_config.ap.password);
    ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &ap_config))

    return ESP_OK;
}

esp_err_t init_wifi()
{
    ESP_LOGI(TAG, "Initializing Wifi");
    ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL))
    wifi_event_group = xEventGroupCreate();

    wifi_init_config_t init_cfg = WIFI_INIT_CONFIG_DEFAULT();
    ERROR_CHECK(esp_wifi_init(&init_cfg))

    return ESP_OK;
}

esp_err_t wifi_scan()
{
    ESP_LOGI(TAG, "Starting scan...");
    static wifi_scan_config_t scanConf = {
        .ssid = NULL,
        .bssid = NULL,
        .channel = 0,
        .show_hidden = false,
        .scan_type = WIFI_SCAN_TYPE_ACTIVE,
		.scan_time.active.min = 120,
		.scan_time.active.max = 150,
    };

    ERROR_CHECK(esp_wifi_scan_start(&scanConf, false))
    xEventGroupClearBits(wifi_event_group, SCAN_FINISHED);

    return ESP_OK;
}

wifi_ap_record_t* scan_results(){
    xEventGroupWaitBits(wifi_event_group, SCAN_FINISHED, pdFALSE, pdFALSE, portMAX_DELAY);
    return ap_list;
}

esp_err_t attempt_to_connect(bool* result)
{
    ESP_LOGI(TAG, "Attempting to connect to AP");

    if( !result )
        return ESP_ERR_INVALID_ARG;

    // Disconnect if already connected to AP
    if(CONNECTED & xEventGroupGetBits(wifi_event_group))
    {
        ESP_ERROR_CHECK(esp_wifi_disconnect());
        xEventGroupWaitBits(wifi_event_group, DISCONNECTED, pdFALSE, pdFALSE, portMAX_DELAY);
    }

    // Clear status and attempt to connect
    xEventGroupClearBits(wifi_event_group, CONNECTED | DISCONNECTED);
    ERROR_CHECK(esp_wifi_connect())

    uint32_t status = xEventGroupWaitBits(wifi_event_group, CONNECTED | DISCONNECTED, pdFALSE, pdFALSE, portMAX_DELAY);
    if( status & CONNECTED )
        *result = true;
    else if ( status & DISCONNECTED )
        *result = false;
    else
        return ESP_FAIL;

    return ESP_OK;
}