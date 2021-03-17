#ifndef OTA_H
#define OTA_H

#include <esp_system.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

esp_err_t rollback_ota();
esp_err_t start_ota();
eTaskState get_ota_task_status();
char* get_ota_status_string();

char* get_update_check_status_string();
esp_err_t start_update_checking_task();
void check_for_update();

#endif