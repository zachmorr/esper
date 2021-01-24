#ifndef ACCESSPOINT_H
#define ACCESSPOINT_H

#include "esp_wifi.h"
#include <esp_system.h>

#define AP_SSID "Config"
#define AP_PASS ""
#define AP_MAX_CONNECTIONS 2

void wifi_init_apsta();
void start_wifi_scan();
void get_scan_results(uint16_t* count, wifi_ap_record_t** list);
bool test_authentication();

#endif