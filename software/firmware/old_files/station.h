#ifndef STATION_H
#define STATION_H

#include <esp_system.h>

esp_err_t wifi_init_sta();
esp_err_t wait_for_wifi();
esp_err_t set_static_ip();

#endif