#ifndef WIFI_H
#define WIFI_H

#include "esp_system.h"
#include "esp_wifi.h"

#define MAX_SCAN_RECORDS 5

esp_err_t set_sta_config(char* ssid, char* pass);
esp_err_t update_sta_config();
esp_err_t set_ap_config(char* ssid, char* pass, int connections);
esp_err_t update_ap_config();
esp_err_t init_wifi_sta_netif();
esp_err_t init_wifi_ap_netif();
esp_err_t init_wifi();
esp_err_t wifi_scan();
wifi_ap_record_t* get_scan_results();
esp_err_t attempt_to_connect(bool* result);

#endif