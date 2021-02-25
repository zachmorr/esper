#ifndef ACCESSPOINT_H
#define ACCESSPOINT_H

#include "esp_wifi.h"
#include <esp_system.h>

#define AP_SSID "Config"
#define AP_PASS ""
#define AP_MAX_CONNECTIONS 2

esp_err_t wifi_init_apsta();
esp_err_t start_wifi_scan();
uint16_t get_scan_results(wifi_ap_record_t** list);
bool test_authentication();

#endif