#ifndef OTA_H
#define OTA_H

#include <esp_system.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

esp_err_t start_ota();
TaskHandle_t get_ota_task_handle();
esp_err_t delete_ota_task();

#endif