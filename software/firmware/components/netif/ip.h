#ifndef IP_H
#define IP_H

#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

extern EventGroupHandle_t ip_event_group;
extern const int SCAN_FINISHED_BIT;
extern const int DISCONNECTED_BIT;
extern const int CONNECTED_BIT;
extern const int PROVISIONED_BIT;
extern const int STATIC_IP_BIT;

esp_err_t initialize_interfaces();
esp_err_t turn_on_accesspoint();
esp_err_t turn_off_accesspoint();
esp_err_t finish_provisioning();
esp_err_t wait_for_provisioning_to_finish();
esp_err_t start_interfaces();
bool provision_status();

#endif