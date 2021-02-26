#ifndef WIFI_H
#define WIFI_H

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_netif.h"

#define MAX_SCAN_RECORDS 5

// extern EventGroupHandle_t wifi_event_group;
// extern const int SCAN_FINISHED;
// extern const int DISCONNECTED;
// extern const int CONNECTED;

esp_err_t init_wifi_sta_netif(esp_netif_t** sta_netif);
esp_err_t init_wifi_ap_netif(esp_netif_t** ap_netif);

esp_err_t init_wifi();

esp_err_t wifi_scan();
wifi_ap_record_t* scan_results();
esp_err_t attempt_to_connect(bool* result);

#endif