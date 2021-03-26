#include "error.h"
#include "events.h"
#include "flash.h"
#include "gpio.h"
#include "ip.h"
#include "webserver.h"
#include "url.h"
#include "logging.h"
#include "datetime.h"
#include "dns.h"
#include "ota.h"

#include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
#include "esp_log.h"
#include "esp_ota_ops.h"

static const char *TAG = "APP_BOOT";

#define CHECK(x) if( x != ESP_OK ) rollback();

esp_err_t set_logging_levels()
{
    esp_log_level_set("heap_init", ESP_LOG_ERROR);
    // esp_log_level_set("spi_flash", ESP_LOG_ERROR);
    esp_log_level_set("wifi", ESP_LOG_ERROR);
    esp_log_level_set("wifi_init", ESP_LOG_ERROR);
    // esp_log_level_set("phy", ESP_LOG_ERROR); 
    // esp_log_level_set("esp_netif_handlers", ESP_LOG_ERROR);
    esp_log_level_set("system_api", ESP_LOG_ERROR);
    esp_log_level_set("esp_eth.netif.glue", ESP_LOG_ERROR);

    return ESP_OK;
}

void task_stacks(void* params)
{
    ESP_LOGI(TAG, "Starting Task Task");
    char buf[1000];
    while(1)
    {
        vTaskDelay( 5000/portTICK_PERIOD_MS );
        ESP_LOGI(TAG, "Min Free Heap: %d", xPortGetMinimumEverFreeHeapSize());
        ESP_LOGI(TAG, "Running Tasks: %d", uxTaskGetNumberOfTasks());

        vTaskList(buf);
        ESP_LOGI(TAG, "\nName\t\tState\tPriority\tStack\tNum\n%s", buf);
    }
}

esp_err_t initialize()
{
    ESP_LOGI(TAG, "Initializing...");
    ERROR_CHECK(set_bit(INITIALIZING_BIT))
    ERROR_CHECK(initialize_interfaces())
    ERROR_CHECK(initialize_blocklists())
    ERROR_CHECK(initialize_logging())
    ERROR_CHECK(start_webserver())
    ERROR_CHECK(clear_bit(INITIALIZING_BIT))

    return ESP_OK;
}

esp_err_t start_provisioning()
{
    ESP_LOGI(TAG, "Starting provisioning...");
    ERROR_CHECK(set_bit(PROVISIONING_BIT))
    ERROR_CHECK(turn_on_accesspoint())
    ERROR_CHECK(start_provisioning_webserver())

    ERROR_CHECK(wait_for(PROVISIONED_BIT, portMAX_DELAY))
    ERROR_CHECK(clear_bit(PROVISIONING_BIT))
    ERROR_CHECK(stop_provisioning_webserver())
    ERROR_CHECK(turn_off_accesspoint())

    return ESP_OK;
}

esp_err_t start_application()
{
    ESP_LOGI(TAG, "Starting application...");
    ERROR_CHECK(start_interfaces())
    ERROR_CHECK(wait_for(CONNECTED_BIT, portMAX_DELAY))

    ERROR_CHECK(initialize_sntp())
    ERROR_CHECK(start_dns())
    ERROR_CHECK(start_update_checking_task())
    ERROR_CHECK(start_application_webserver())

    return ESP_OK;
}

void cancel_rollback()
{
    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_ota_img_states_t ota_state;
    if (esp_ota_get_state_partition(running, &ota_state) == ESP_OK) {
        if (ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
            ESP_LOGI(TAG, "Diagnostics completed successfully! Continuing execution ...");
            esp_ota_mark_app_valid_cancel_rollback();
        }
    }
}

void rollback()
{
    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_ota_img_states_t ota_state;
    if (esp_ota_get_state_partition(running, &ota_state) == ESP_OK) {
        if (ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
            ESP_LOGE(TAG, "Diagnostics failed! Start rollback to the previous version ...");
            esp_ota_mark_app_invalid_rollback_and_reboot();
        }
    }
}

void app_main()
{
    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_ota_img_states_t ota_state;
    if (esp_ota_get_state_partition(running, &ota_state) == ESP_OK) {
        ESP_LOGI(TAG, "Current partition state (%d)", ota_state);
    }

    set_logging_levels();
    CHECK(initialize_flash())
    CHECK(initialize_gpio())
    CHECK(init_event_group())
    CHECK(initialize())

    if( !check_bit(PROVISIONED_BIT) )
    {
        CHECK(start_provisioning())
    }

    CHECK(start_application())
    cancel_rollback();

    // xTaskCreatePinnedToCore(task_stacks, "task_task", 4000, NULL, 2, NULL, tskNO_AFFINITY);
}