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

static const char *TAG = "APP_BOOT";

void app_main()
{
    esp_log_level_set("heap_init", ESP_LOG_ERROR);
    esp_log_level_set("spi_flash", ESP_LOG_ERROR);
    esp_log_level_set("wifi", ESP_LOG_ERROR);
    esp_log_level_set("wifi_init", ESP_LOG_ERROR);
    esp_log_level_set("phy", ESP_LOG_ERROR); 
    esp_log_level_set("esp_netif_handlers", ESP_LOG_ERROR);
    esp_log_level_set("system_api", ESP_LOG_ERROR);
    esp_log_level_set("esp_eth.netif.glue", ESP_LOG_ERROR); 

    ESP_LOGI(TAG, "Starting...");

    ESP_ERROR_CHECK(initialize_gpio());
    set_led_state(STARTUP, SET);
    ESP_ERROR_CHECK(initialize_storage());
    bool configured = check_configuration_status();
    if(configured)
    {
        ESP_LOGI(TAG, "Already configured, starting application...");
        ESP_ERROR_CHECK(initialize_blocklists());
        ESP_ERROR_CHECK(initialize_logging());
        ESP_ERROR_CHECK(wifi_init_sta());
        ESP_ERROR_CHECK(initialize_sntp());
        start_dns();
        start_application_webserver();
        start_update_checking_task();
        set_led_state(STARTUP, CLEAR);
        set_led_state(BLOCKING, SET);
    }
    else
    {
        ESP_LOGI(TAG, "Not configured, starting wifi provisioning service...");
        wifi_init_apsta();
        start_captive_dns();
        start_configuration_webserver();
        set_led_state(STARTUP, CLEAR);
        set_led_state(CONFIGURING, SET);
    }
}