#include "wifi.h"
#include "error.h"
#include "string.h"
#include "esp_system.h"
#include "esp_wifi.h"

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"
static const char *TAG = "WIFI";

static esp_netif_t *wifi_sta_netif = NULL;
static esp_netif_t *wifi_ap_netif = NULL;

static wifi_config_t sta_config;
static wifi_config_t ap_config;

esp_err_t init_wifi_netif(wifi_mode_t mode)
{
    ESP_LOGI(TAG, "Initializing wifi...");

    if( mode == WIFI_MODE_STA || mode == WIFI_MODE_APSTA ){
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
    }

    if( mode == WIFI_MODE_AP || mode == WIFI_MODE_APSTA ){
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
    }
 
    wifi_init_config_t init_cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&init_cfg);

    return ESP_OK;
}