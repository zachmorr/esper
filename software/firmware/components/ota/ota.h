#ifndef OTA_H
#define OTA_H

#include <esp_system.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define DEFAULT_UPDATE_URL "openesper.com/OpenEsper.bin"

esp_err_t start_ota();
TaskHandle_t get_ota_task_handle();
esp_err_t delete_ota_task();
esp_err_t start_update_checking_task();
void check_for_update();
bool is_update_available();

#endif