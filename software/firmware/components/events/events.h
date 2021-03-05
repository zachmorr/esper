#ifndef EVENTS_H
#define EVENTS_H

#include "esp_system.h"
#include "freertos/FreeRTOS.h"

extern const int SCAN_FINISHED_BIT;
extern const int CONNECTED_BIT;
extern const int DISCONNECTED_BIT;
extern const int PROVISIONING_BIT;
extern const int PROVISIONED_BIT;
extern const int STATIC_IP_BIT;
extern const int BLOCKING_BIT;

esp_err_t init_event_group();
esp_err_t set_bit(int bit);
esp_err_t clear_bit(int bit);
bool check_bit(int bit);
esp_err_t wait_for(int bit, TickType_t xTicksToWait);
esp_err_t toggle_bit(int bit);

#endif

