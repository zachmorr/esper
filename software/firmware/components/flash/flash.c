#include "flash.h"
#include "error.h"
#include "url.h"
#include "logging.h"
#include "esp_system.h"
#include "string.h"
#include "nvs_flash.h"
#include "esp_spiffs.h"
#include "esp_netif.h"
#include "lwip/inet.h"
#include "esp_wifi.h"

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"
static const char *TAG = "FLASH";

static nvs_handle nvs;

esp_err_t nvs_set(char* key, void* value, size_t length)
{
    esp_err_t ret = nvs_set_blob(nvs, key, value, length);
    if (ret != ESP_OK) 
    {
        ESP_LOGE(TAG, "%s %s", key, esp_err_to_name(ret));
    }
    return ret;
}

esp_err_t nvs_get_length(char* key, size_t* length)
{
    esp_err_t ret = nvs_get_blob(nvs, key, NULL, length);
    if (ret != ESP_OK) 
    {
        ESP_LOGE(TAG, "%s %s", key, esp_err_to_name(ret));
        *length = 0;
    }
    return ret;
}

esp_err_t nvs_get(char* key, void* value, size_t length)
{
    if (length < 1)
    {
        nvs_get_length(key, &length);
    }

    esp_err_t ret = nvs_get_blob(nvs, key, value, &length);
    if (ret != ESP_OK) 
    {
        ESP_LOGE(TAG, "%s %s", key, esp_err_to_name(ret));
        *((uint8_t*)value) = 0;
    }
    return ret;
}

esp_err_t reset_device()
{
    ESP_LOGI(TAG, "Reseting Device");
    
    // uint8_t conf_status = 0;
    // esp_err_t err = nvs_set_blob(nvs, "config_status", (void*)&conf_status, sizeof(conf_status));
    return nvs_set_u8(nvs, "configured", (uint8_t)false);
}

esp_err_t check_configuration_status(bool* configured)
{
    return nvs_get_u8(nvs, "configured", (uint8_t*)configured);
}

esp_err_t get_network_info(esp_netif_ip_info_t* info)
{
    size_t length = sizeof(*info);
    esp_err_t err = nvs_get_blob(nvs, "network_info", (void*)info, &length);
    if( err != ESP_OK ){
        ESP_LOGE(TAG, "Error getting network info (%s)", esp_err_to_name(err));
    }
    return err;
}

esp_err_t set_network_info(esp_netif_ip_info_t info)
{
    esp_err_t err = nvs_set_blob(nvs, "network_info", (void*)&info, sizeof(info));
    if( err != ESP_OK ){
        ESP_LOGE(TAG, "Error setting network info (%s)", esp_err_to_name(err));
    }
    return err;
}

esp_err_t get_log_data(uint16_t* head, bool* full){
    esp_err_t err = nvs_get_u16(nvs, "log_head", head);
    err |= nvs_get_u8(nvs, "full_flag", (uint8_t*)full);

    if( err != ESP_OK ){
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t update_log_data(uint16_t head, bool full)
{
    esp_err_t err = nvs_set_u16(nvs, "log_head", head);
    err |= nvs_set_u8(nvs, "full_flag", (uint8_t)full);

    if( err != ESP_OK ){
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t init_data()
{
    // Initialize settings
    esp_err_t err = nvs_set_u8(nvs, "initialized", (uint8_t)true);
#if CONFIG_UPSTREAM_DNS_GOOGLE
    err = nvs_set_blob(nvs, "upstream_server", GOOGLE_IP, IP4_STRLEN_MAX);
#elif CONFIG_UPSTREAM_DNS_CLOUDFARE
    err = nvs_set_blob(nvs, "upstream_server", CLOUDFARE_IP, IP4_STRLEN_MAX);
#elif CONFIG_UPSTREAM_DNS_OPENDNS
    err = nvs_set_blob(nvs, "upstream_server", OPENDNS_IP, IP4_STRLEN_MAX);
#elif CONFIG_UPSTREAM_DNS_ADGUARD
    err = nvs_set_blob(nvs, "upstream_server", ADGUARD_IP, IP4_STRLEN_MAX);
#endif
    err |= nvs_set_blob(nvs, "url", CONFIG_DEFAULT_DEVICE_URL, HOSTNAME_STRLEN_MAX);
    err |= nvs_set_blob(nvs, "update_url", CONFIG_DEFAULT_UPDATE_URI, HOSTNAME_STRLEN_MAX + CONFIG_HTTPD_MAX_URI_LEN);
    err |= nvs_set_u8(nvs, "configured", (uint8_t)false);

    // initialize variables that will be used for circular buffer of logs
    err |= update_log_data(MAX_LOGS, false);

    // initialize network info
    esp_netif_ip_info_t info = {0};
#if CONFIG_PROVISION_DISABLE
    inet_aton(CONFIG_IP, &(info.ip));
    inet_aton(CONFIG_GW, &(info.gw));
    inet_aton(CONFIG_NM, &(info.netmask));

    err |= nvs_set_u8(nvs, "configured", (uint8_t)true);
#endif
    err |= set_network_info(info);

    if( err != ESP_OK ){
        return ESP_FAIL;
    }
    return ESP_OK;
}

static bool first_power_on()
{
    bool initialized;
    esp_err_t err = nvs_get_u8(nvs, "initialized", (uint8_t*)&initialized);
    if( err ==  ESP_ERR_NVS_NOT_FOUND ){
        ESP_LOGW(TAG, "Flash has not been initialized");
        return true;
    }
    return false;
}

esp_err_t init_nvs()
{
    ESP_LOGI(TAG, "Initializing NVS...");
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGE(TAG, "Error initializing flash");
        err = nvs_flash_erase();
        if (err == ESP_OK){
            err = nvs_flash_init();
        }
    }
    if (err != ESP_OK){
        return err;
    }

    if ( (err = nvs_open("storage", NVS_READWRITE, &nvs)) != ESP_OK )
        return err;
        
    ESP_LOGD(TAG, "NVS Initialized");
    return ESP_OK;
}

static esp_err_t init_spiffs()
{
    ESP_LOGI(TAG, "Initializing SPIFFS");
    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 10,
      .format_if_mount_failed = true
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ret;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Spiffs Partition size: total: %d, used: %d", total, used);
    return ESP_OK;
}

esp_err_t initialize_flash()
{    
    esp_err_t err = init_nvs();
    err |= init_spiffs();
    if( err != ESP_OK ){
        return ESP_FAIL;
    }
    
    if( first_power_on() )
    {
        ESP_LOGI(TAG, "First power up, initializing required data...");
        err = init_data();
        err |= create_log_file();
        err |= store_default_blacklists();
        if( err != ESP_OK ){
            return ESP_FAIL;
        }
    }

    ESP_LOGI(TAG, "Flash Initialized");
    return ESP_OK;
}