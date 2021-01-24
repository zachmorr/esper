#include "storage.h"
#include "dns.h"

#include <esp_system.h>
#include <string.h>
#include "nvs_flash.h"
#include "esp_spiffs.h"

#define LOG_LOCAL_LEVEL ESP_LOG_WARN
#include "esp_log.h"
static const char *TAG = "STORAGE";

static nvs_handle nvs;


esp_err_t get_log_values(uint16_t* log_head, bool* full_flag)
{
    nvs_get_u16(nvs, "log_head", log_head);
    nvs_get_u8(nvs, "full_flag", (uint8_t*)full_flag);
    return ESP_OK;
}

esp_err_t update_log_values(uint16_t log_head, bool full_flag)
{
    ESP_LOGD(TAG, "Settings log values to %d %d", log_head, full_flag);
    nvs_set_u16(nvs, "log_head", log_head);
    nvs_set_u8(nvs, "full_flag", (uint8_t)full_flag);
    return ESP_OK;
}

char* get_upstream_server(char* server)
{
    size_t size;
    ESP_ERROR_CHECK(nvs_get_str(nvs, "server", NULL, &size));
    ESP_ERROR_CHECK(nvs_get_str(nvs, "server", server, &size));
    return server;
}

esp_err_t update_upstream_server(char* server)
{
    ESP_ERROR_CHECK(nvs_set_str(nvs, "server", server));
    return ESP_OK;
}

char* get_static_ip(char* ip)
{
    size_t size;
    ESP_ERROR_CHECK(nvs_get_str(nvs, "ip", NULL, &size));
    ESP_ERROR_CHECK(nvs_get_str(nvs, "ip", ip, &size));
    return ip;
}

esp_err_t update_static_ip(char* ip)
{
    ESP_ERROR_CHECK(nvs_set_str(nvs, "ip", ip));
    return ESP_OK;
}

char* get_netmask(char* ip)
{
    size_t size;
    ESP_ERROR_CHECK(nvs_get_str(nvs, "nm", NULL, &size));
    ESP_ERROR_CHECK(nvs_get_str(nvs, "nm", ip, &size));
    return ip;
}

esp_err_t update_netmask(char* ip)
{
    ESP_ERROR_CHECK(nvs_set_str(nvs, "nm", ip));
    return ESP_OK;
}

char* get_gateway(char* ip)
{
    size_t size;
    ESP_ERROR_CHECK(nvs_get_str(nvs, "gw", NULL, &size));
    ESP_ERROR_CHECK(nvs_get_str(nvs, "gw", ip, &size));
    return ip;
}

esp_err_t update_gateway(char* ip)
{
    ESP_ERROR_CHECK(nvs_set_str(nvs, "gw", ip));
    return ESP_OK;
}

esp_err_t get_from_nvs(StorageType type, char* key, void* value)
{
    switch(type)
    {
        case STR:
            ESP_LOGD(TAG, "Getting string \"%s\" from nvs", key);
            size_t size;
            ESP_ERROR_CHECK(nvs_get_str(nvs, key, NULL, &size));
            ESP_ERROR_CHECK(nvs_get_str(nvs, key, (char*)value, &size));
            ESP_LOGD(TAG, "%s: %s", key, (char*)value);
            return ESP_OK;
        case UINT8:
            ESP_LOGD(TAG, "Getting u8 \"%s\" from nvs", key);
            ESP_ERROR_CHECK(nvs_get_u8(nvs, key, (uint8_t*)value));
            ESP_LOGD(TAG, "%s: %d", key, *(uint8_t*)value);
            return ESP_OK;
        case UINT16:
            ESP_LOGD(TAG, "Getting u16 \"%s\" from nvs", key);
            ESP_ERROR_CHECK(nvs_get_u16(nvs, key, (uint16_t*)value));
            ESP_LOGD(TAG, "%s: %d", key, *(uint16_t*)value);
            return ESP_OK;
        case UINT32:
            ESP_LOGD(TAG, "Getting u32 \"%s\" from nvs", key);
            ESP_ERROR_CHECK(nvs_get_u32(nvs, key, (uint32_t*)value));
            ESP_LOGD(TAG, "%s: %d", key, *(uint32_t*)value);
            return ESP_OK;
        default:
            return ESP_FAIL;
    }
}

esp_err_t store_in_nvs(StorageType type, char* key, void* value)
{
    switch(type)
    {
        case STR:
            ESP_LOGD(TAG, "Storing %s in %s", (char*)value, key);
            ESP_ERROR_CHECK(nvs_set_str(nvs, key, (char*)value));
            return ESP_OK;
        case UINT8:
            ESP_LOGD(TAG, "Storing %d in %s", *(uint8_t*)value, key);
            ESP_ERROR_CHECK(nvs_set_u8(nvs, key, *(uint8_t*)value));
            return ESP_OK;
        case UINT16:
            ESP_LOGD(TAG, "Storing %d in %s", *(uint16_t*)value, key);
            ESP_ERROR_CHECK(nvs_set_u16(nvs, key, *(uint16_t*)value));
            return ESP_OK;
        case UINT32:
            ESP_LOGD(TAG, "Storing %d in %s", *(uint32_t*)value, key);
            ESP_ERROR_CHECK(nvs_set_u32(nvs, key, *(uint32_t*)value));
            return ESP_OK;
        default:
            return ESP_FAIL;
    }
}

esp_err_t initialize_storage()
{    
    //Initialize NVS
    ESP_LOGI(TAG, "Initializing Flash...");
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGE(TAG, "Error initializing flash");
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &nvs));
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

esp_err_t finish_configuration()
{
    ESP_LOGD(TAG, "Setting keys in nvs to default values and flipping ");
    ESP_ERROR_CHECK(nvs_set_u16(nvs, "logposition", 0));
    ESP_ERROR_CHECK(nvs_set_str(nvs, "server", DEFAULT_UPSTREAM_DNS));
    ESP_ERROR_CHECK(nvs_set_u8(nvs, "config status", 1));
    return ESP_OK;
}

esp_err_t reset_device()
{
    ESP_LOGI(TAG, "Reseting Device");
    ESP_ERROR_CHECK(nvs_set_u8(nvs, "config status", 0));
    return ESP_OK;
}

bool check_configuration_status()
{
    uint8_t config_status;
    ESP_LOGD(TAG, "Reading config status");
    esp_err_t err = nvs_get_u8(nvs, "config status", &config_status);

    if(err == ESP_OK)
    {
        ESP_LOGI(TAG, "Config Status: %d", config_status);
        if(config_status)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        ESP_LOGE(TAG, "NVS Read Returned: %s", esp_err_to_name(err));
        return false;
    }
}

