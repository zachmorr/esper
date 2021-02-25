#ifndef IP_H
#define IP_H

#include "esp_system.h"

esp_err_t initialize_interfaces();
esp_err_t turn_on_accesspoint();
esp_err_t turn_off_accesspoint();
esp_err_t finish_provisioning();
esp_err_t wait_for_provisioning_to_finish();
esp_err_t start_interfaces();
bool provision_status();

#endif