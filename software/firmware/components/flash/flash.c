#include "flash.h"
// #include "dns.h"
// #include "url.h"

#include "esp_system.h"
#include "string.h"
#include "nvs_flash.h"
#include "esp_spiffs.h"
// #include "esp_http_server.h"

#define LOG_LOCAL_LEVEL ESP_LOG_WARN
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

esp_err_t store_default_blacklists()
{
    ESP_LOGI(TAG, "Saving Blacklist");
    FILE* b = fopen("/spiffs/blacklist", "w");
    if(b == NULL)
    {
        ESP_LOGE(TAG, "Error opening blacklist file");
        return ESP_FAIL;
    }

    extern const unsigned char blacklist_txt_start[] asm("_binary_default_blacklist_txt_start");
    extern const unsigned char blacklist_txt_end[]   asm("_binary_default_blacklist_txt_end");
    const size_t blacklist_txt_size = (blacklist_txt_end - blacklist_txt_start);

    fwrite(blacklist_txt_start, blacklist_txt_size, 1, b);
    fclose(b);

    return ESP_OK;
}

static esp_err_t create_log_file()
{
    ESP_LOGI(TAG, "Creating log file");
    FILE* log = fopen("/spiffs/log", "w");
    if (log)
    {
        // ftruncate not implemented yet
        ESP_LOGD(TAG, "Expanding log file to %d bytes", MAX_LOGS*sizeof(Log_Entry));
        Log_Entry entry = {0};
        for(int i = 0; i < MAX_LOGS; i++)
        {
            fwrite(&entry, sizeof(Log_Entry), 1, log);
        }
        ESP_LOGD(TAG, "Log file %ld bytes large", ftell(log));
        fclose(log);

        uint16_t size = MAX_LOGS; 
        bool full = false;

        nvs_set("log_head", (void*)&size, sizeof(size));
        nvs_set("full_flag", (void*)&full, sizeof(full));
        return ESP_OK;
    }
    else
    {
        return ESP_FAIL;
    }
}

static init_settings()
{
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
        ESP_LOGE(TAG, "Flash has not been initialized");
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
        ESP_LOGI(TAG, "Initializing required data...");
        err = init_settings();
        err |= create_log_file();
        err |= store_default_blacklists();
    }

    if( err != ESP_OK ){
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Flash Initialized");
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
    return err |= nvs_get_u8(nvs, "configured", (uint8_t*)configured);;
}