#ifndef GPIO_H
#define GPIO_H

#include <esp_system.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

TaskHandle_t getLEDTaskHandle();
esp_err_t initialize_gpio();

#endif