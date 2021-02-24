#include "wifi.h"
#include "error.h"
#include "string.h"
#include "esp_system.h"
#include "esp_wifi.h"

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"
static const char *TAG = "WIFI";

static wifi_mode_t wifi_mode = WIFI_MODE_NULL;

static esp_netif_t *wifi_sta_netif = NULL;
static esp_netif_t *wifi_ap_netif = NULL;

static wifi_config_t sta_config = {0};
static wifi_config_t ap_config = {0};

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
            // (wifi_event_sta_scan_done_t*)event_data;
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
            break;
        }
        case WIFI_EVENT_STA_DISCONNECTED:
        {
            wifi_event_sta_disconnected_t* event = (wifi_event_sta_disconnected_t*) event_data;
            ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED %d", event->reason);
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
            ESP_LOGI(TAG, "WIFI_EVENT_AP_STADISCONNECTED");
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

static void ip_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    switch (event_id) {
        case IP_EVENT_ETH_GOT_IP:
            ESP_LOGI(TAG, "IP_EVENT_ETH_GOT_IP");
            break;
        case IP_EVENT_STA_GOT_IP:
            ESP_LOGI(TAG, "IP_EVENT_STA_GOT_IP");
            break;
        case IP_EVENT_STA_LOST_IP:
            ESP_LOGI(TAG, "IP_EVENT_STA_LOST_IP");
            break;
        case IP_EVENT_AP_STAIPASSIGNED:
            ESP_LOGI(TAG, "IP_EVENT_AP_STAIPASSIGNED");
            break;
        default:
            break;
    }
}

static esp_err_t init_wifi_sta_netif()
{
    esp_netif_config_t sta_cfg = {                                                 
        .base = &_g_esp_netif_inherent_sta_config,      
        .driver = NULL,                               
        .stack = ESP_NETIF_NETSTACK_DEFAULT_WIFI_STA, 
    };

    wifi_sta_netif = esp_netif_new(&sta_cfg);
    if( !wifi_sta_netif )
        return ESP_FAIL;
    
    ERROR_CHECK(esp_netif_attach_wifi_station(wifi_sta_netif))
    ERROR_CHECK(esp_wifi_set_default_wifi_sta_handlers())
    return ESP_OK;
}

static esp_err_t init_wifi_ap_netif()
{
    esp_netif_config_t ap_cfg = {                                                 
        .base = ESP_NETIF_BASE_DEFAULT_WIFI_AP,      
        .driver = NULL,                               
        .stack = ESP_NETIF_NETSTACK_DEFAULT_WIFI_AP, 
    };

    wifi_ap_netif = esp_netif_new(&ap_cfg);
    if( !wifi_ap_netif )
        return ESP_FAIL;
    
    ERROR_CHECK(esp_netif_attach_wifi_ap(wifi_ap_netif))
    ERROR_CHECK(esp_wifi_set_default_wifi_ap_handlers())

    wifi_config_t config = {
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
        config.ap.authmode = WIFI_AUTH_OPEN;
    }
    ap_config = config;

    return ESP_OK;
}

esp_err_t init_wifi_netif()
{
    ESP_LOGI(TAG, "Initializing wifi...");

    if( wifi_mode == WIFI_MODE_NULL )
        return WIFI_ERR_MODE_NULL;
    
    ERROR_CHECK(esp_netif_init())
    ERROR_CHECK(esp_event_loop_create_default())

    if( wifi_mode == WIFI_MODE_STA || wifi_mode == WIFI_MODE_APSTA ){
        ERROR_CHECK(init_wifi_sta_netif())
        ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &sta_config))
    }

    if( wifi_mode == WIFI_MODE_AP || wifi_mode == WIFI_MODE_APSTA ){
        ERROR_CHECK(init_wifi_ap_netif())
        ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &ap_config))
    }
 
    wifi_init_config_t init_cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&init_cfg);

    return ESP_OK;
}

esp_err_t wifi_start()
{
    ESP_LOGI(TAG, "Initializing Wifi");

    ERROR_CHECK(init_wifi_netif())
    ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL))
    ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &ip_event_handler, NULL))
    ERROR_CHECK(esp_wifi_set_mode(wifi_mode))
    ERROR_CHECK(esp_wifi_start())

    return ESP_OK;
}

esp_err_t set_sta_config(char* ssid, char* pass)
{
    if( ssid == NULL || pass == NULL)
    {
        ERROR_CHECK(esp_wifi_get_config(ESP_IF_WIFI_STA, &sta_config))
    }
    else
    {
        strcpy((char*)sta_config.sta.ssid, ssid);
        strcpy((char*)sta_config.sta.password, pass);
    }

    return ESP_OK;
}

esp_err_t set_wifi_mode(wifi_mode_t mode)
{
    wifi_mode = mode;
    return ESP_OK;
}