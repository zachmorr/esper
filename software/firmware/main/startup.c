#include "storage.h"
#include "logging.h"
#include "datetime.h"
#include "gpio.h"
#include "station.h"
#include "accesspoint.h"
#include "dns.h"
#include "captive_dns.h"
#include "configuration.h"
#include "application.h"
#include "url.h"
#include "ota.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_ota_ops.h"

static const char *TAG = "APP_BOOT";

#define ERROR_CHECK(err, x) if( (err = x) != ESP_OK ) goto end; 

void app_main()
{
    // esp_log_level_set("heap_init", ESP_LOG_ERROR);
    // esp_log_level_set("spi_flash", ESP_LOG_ERROR);
    esp_log_level_set("wifi", ESP_LOG_ERROR);
    esp_log_level_set("wifi_init", ESP_LOG_ERROR);
    // esp_log_level_set("phy", ESP_LOG_ERROR); 
    // esp_log_level_set("esp_netif_handlers", ESP_LOG_ERROR);
    // esp_log_level_set("system_api", ESP_LOG_ERROR);
    // esp_log_level_set("esp_eth.netif.glue", ESP_LOG_ERROR); 
    ESP_LOGI(TAG, "Starting...");

    esp_err_t err;
    ERROR_CHECK(err, initialize_gpio())
    ERROR_CHECK(err, set_led_state(STARTUP, SET))
    ERROR_CHECK(err, initialize_storage())

    nvs_set("ip", (void*)"192.168.2.60", (size_t)IP4ADDR_STRLEN_MAX);
    nvs_set("nm", (void*)"255.255.255.0", (size_t)IP4ADDR_STRLEN_MAX);
    nvs_set("gw", (void*)"192.168.2.1", (size_t)IP4ADDR_STRLEN_MAX);

    bool configured;
    ERROR_CHECK(err, check_configuration_status(&configured))
    if(configured)
    {
        ESP_LOGI(TAG, "Already configured, starting application...");
        ERROR_CHECK(err, initialize_blocklists())
        ERROR_CHECK(err, initialize_logging())
        ERROR_CHECK(err, wifi_init_sta())
        ERROR_CHECK(err, start_application_webserver())
        ERROR_CHECK(err, initialize_sntp())
        ERROR_CHECK(err, start_dns())
        start_update_checking_task();
        set_led_state(STARTUP, CLEAR);
        set_led_state(BLOCKING, SET);
    }
    else
    {
        ESP_LOGI(TAG, "Not configured, starting wifi provisioning...");
        ERROR_CHECK(err, wifi_init_apsta())
        ERROR_CHECK(err, start_captive_dns())
        ERROR_CHECK(err, start_configuration_webserver())
        set_led_state(STARTUP, CLEAR);
        set_led_state(CONFIGURING, SET);
    }

end: ;
    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_ota_img_states_t ota_state;
    if (esp_ota_get_state_partition(running, &ota_state) == ESP_OK) {
        if (ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
            if (err == ESP_OK) {
                ESP_LOGI(TAG, "Diagnostics completed successfully! Continuing execution ...");
                esp_ota_mark_app_valid_cancel_rollback();
            } else {
                ESP_LOGE(TAG, "Diagnostics failed! Start rollback to the previous version ...");
                esp_ota_mark_app_invalid_rollback_and_reboot();
            }
        }
    }

    if (err != ESP_OK){
        ESP_LOGE(TAG, "Error during startup!");
        set_led_state(ERROR, SET);
    }
}