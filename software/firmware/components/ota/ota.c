#include "ota.h"
#include "gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"

#define UPDATE_URL "https://upgrade.esperdns.co/EsperDNS.bin"
static const char *TAG = "OTA";

static TaskHandle_t ota_task_handle = NULL;

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
        ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    }
    return ESP_OK;
}

void ota_task(void *pvParameter)
{
    ESP_LOGI(TAG, "Starting OTA ");
    set_led_state(OTA, SET);
    extern const uint8_t server_cert_pem_start[] asm("_binary_ca_cert_pem_start");
    //extern const uint8_t server_cert_pem_end[] asm("_binary_ca_cert_pem_end");

    esp_http_client_config_t config = {
        .url = UPDATE_URL,
        .cert_pem = (char *)server_cert_pem_start,
        .event_handler = _http_event_handler,
    };

    esp_err_t ret = esp_https_ota(&config);
    if(ret == ESP_FAIL)
    {
        ESP_LOGE(TAG, "Firmware update failed");
    }
    else
    {
        ESP_LOGI(TAG, "Done writing new firmware");
    }
    set_led_state(OTA, CLEAR);
    vTaskSuspend(NULL);
    while(1)
    {
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

TaskHandle_t get_ota_task_handle()
{
    return ota_task_handle;
}

esp_err_t delete_ota_task()
{
    if(ota_task_handle != NULL)
    {
        vTaskDelete(ota_task_handle);
        ota_task_handle = NULL;
    }

    return ESP_OK;
}

esp_err_t start_ota(){
    xTaskCreatePinnedToCore(&ota_task, "ota_task", 
                            8192, NULL, 5, &ota_task_handle, 0);
    return ESP_OK;
}