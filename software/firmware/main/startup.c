#include "error.h"
#include "wifi.h"
#include "flash.h"
#include "logging.h"
#include "datetime.h"
#include "gpio.h"
// #include "station.h"
// #include "accesspoint.h"
// #include "dns.h"
// #include "captive_dns.h"
// #include "configuration.h"
// #include "application.h"
// #include "url.h"
// #include "ota.h"
#include "esp_wifi.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
#include "esp_log.h"
// #include "esp_ota_ops.h"

static const char *TAG = "APP_BOOT";

static esp_err_t init_app()
{
    ERROR_CHECK(initialize_gpio())
    ERROR_CHECK(set_led_state(STARTUP, SET))
    ERROR_CHECK(initialize_flash())

    // ERROR_CHECK(set_sta_config(CONFIG_SSID, CONFIG_PASSWORD))
    // ERROR_CHECK(init_wifi())
    // ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA))
    // ERROR_CHECK(init_wifi_sta_netif())
    // ERROR_CHECK(esp_wifi_start())
    // ERROR_CHECK(esp_wifi_connect())

    // ERROR_CHECK(set_sta_config(CONFIG_SSID, CONFIG_PASSWORD))
    // ERROR_CHECK(set_ap_config(CONFIG_AP_SSID, CONFIG_AP_PASSWORD, CONFIG_AP_CONNECTIONS))
    ERROR_CHECK(init_wifi())
    ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA))
    ERROR_CHECK(init_wifi_sta_netif())
    ERROR_CHECK(init_wifi_ap_netif())
    ERROR_CHECK(esp_wifi_start())
    ERROR_CHECK(wifi_scan())

    wifi_ap_record_t* ap_list = scan_results();
    for( int i = 0; i < MAX_SCAN_RECORDS; i++ ){
        ESP_LOGI(TAG, "%s", ap_list[i].ssid);
    }
    // ERROR_CHECK(print_scan_results())
    // ERROR_CHECK(esp_wifi_connect())



    // bool configured = false;
    // check_configuration_status(&configured);
    // if(configured)
    // {
    //     ESP_LOGI(TAG, "Already configured, starting application...");
    //     ERROR_CHECK(initialize_blocklists())
    //     ERROR_CHECK(initialize_logging())
    //     ERROR_CHECK(wifi_init_sta())
    //     ERROR_CHECK(start_application_webserver())
    //     ERROR_CHECK(initialize_sntp())
    //     ERROR_CHECK(start_dns())
    //     start_update_checking_task();
    //     set_led_state(STARTUP, CLEAR);
    //     set_led_state(BLOCKING, SET);
    // }
    // else
    // {
    //     ESP_LOGI(TAG, "Not configured, starting wifi provisioning...");
    //     ERROR_CHECK(wifi_init_apsta())
    //     ERROR_CHECK(start_captive_dns())
    //     ERROR_CHECK(start_configuration_webserver())
    //     set_led_state(STARTUP, CLEAR);
    //     set_led_state(CONFIGURING, SET);
    // }

    return ESP_OK;
}

void app_main()
{
    // esp_log_level_set("heap_init", ESP_LOG_ERROR);
    // esp_log_level_set("spi_flash", ESP_LOG_ERROR);
    esp_log_level_set("wifi", ESP_LOG_ERROR);
    esp_log_level_set("wifi_init", ESP_LOG_ERROR);
    // esp_log_level_set("phy", ESP_LOG_ERROR); 
    // esp_log_level_set("esp_netif_handlers", ESP_LOG_ERROR);
    esp_log_level_set("system_api", ESP_LOG_ERROR);
    // esp_log_level_set("esp_eth.netif.glue", ESP_LOG_ERROR); 
    ESP_LOGI(TAG, "Starting...");
    esp_err_t err = init_app();

    // const esp_partition_t *running = esp_ota_get_running_partition();
    // esp_ota_img_states_t ota_state;
    // if (esp_ota_get_state_partition(running, &ota_state) == ESP_OK) {
    //     if (ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
    //         if (err == ESP_OK) {
    //             ESP_LOGI(TAG, "Diagnostics completed successfully! Continuing execution ...");
    //             esp_ota_mark_app_valid_cancel_rollback();
    //         } else {
    //             ESP_LOGE(TAG, "Diagnostics failed! Start rollback to the previous version ...");
    //             esp_ota_mark_app_invalid_rollback_and_reboot();
    //         }
    //     }
    // }

    if (err != ESP_OK){
        ESP_LOGE(TAG, "Error during startup!");
        set_led_state(ERROR, SET);
    }
}