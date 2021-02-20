#include "storage.h"
#include "dns.h"
#include "url.h"

#include "esp_system.h"
#include "string.h"
#include "nvs_flash.h"
#include "esp_spiffs.h"
#include "esp_http_server.h"

#define LOG_LOCAL_LEVEL ESP_LOG_WARN
#include "esp_log.h"
static const char *TAG = "STORAGE";

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

esp_err_t initialize_storage()
{    
    //Initialize NVS
    ESP_LOGI(TAG, "Initializing Flash...");
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
        
    ESP_LOGD(TAG, "Flash Initialized");

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
        return ESP_FAIL;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Spiffs Partition size: total: %d, used: %d", total, used);
    ESP_LOGI(TAG, "Storage Initialized");

    return ESP_OK;
}

esp_err_t set_defaults()
{
    ESP_LOGD(TAG, "Setting keys in nvs to default values and flipping config bit");

    esp_err_t err = nvs_set_blob(nvs, "upstream_server", DEFAULT_UPSTREAM_DNS, IP4ADDR_STRLEN_MAX);
    err |= nvs_set_blob(nvs, "url", DEFAULT_DEVICE_URL, MAX_URL_LENGTH);
    err |= nvs_set_blob(nvs, "update_server", DEFAULT_UPSTREAM_DNS, MAX_URL_LENGTH+HTTPD_MAX_URI_LEN);

    uint8_t conf_status = 1;
    err |= nvs_set_blob(nvs, "config_status", (void*)&conf_status, sizeof(conf_status));

    if (err)
        return ESP_FAIL;
    else
        return ESP_OK;
}

esp_err_t reset_device()
{
    ESP_LOGI(TAG, "Reseting Device");
    uint8_t conf_status = 0;
    esp_err_t err = nvs_set_blob(nvs, "config_status", (void*)&conf_status, sizeof(conf_status));
    return err;
}

esp_err_t check_configuration_status(bool* configured)
{
    ESP_LOGD(TAG, "Reading config_status");
    return nvs_get("config_status", (void*)configured, sizeof(*configured));
}