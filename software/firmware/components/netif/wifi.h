#ifndef WIFI_H
#define WIFI_H

#include "esp_system.h"
#include "esp_wifi.h"

esp_err_t set_sta_config(char* ssid, char* pass);
esp_err_t update_sta_config();
esp_err_t set_ap_config(char* ssid, char* pass, int connections);
esp_err_t update_ap_config();
esp_err_t init_wifi_netif();
esp_err_t wifi_start();
esp_err_t set_wifi_mode(wifi_mode_t mode);

#endif