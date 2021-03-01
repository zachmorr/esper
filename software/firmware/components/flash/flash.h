#ifndef FLASH_H
#define FLASH_H

#include <esp_system.h>
#include "esp_netif.h"

#define GOOGLE_IP "8.8.8.8"
#define CLOUDFARE_IP "1.1.1.1"
#define OPENDNS_IP "208.67.222.222"
#define ADGUARD_IP "94.140.14.14"

#define IP4_STRLEN_MAX 16
#define HOSTNAME_STRLEN_MAX 255

esp_err_t nvs_set(char* key, void* value, size_t length);
esp_err_t nvs_get(char* key, void* value, size_t length);
esp_err_t nvs_get_length(char* key, size_t* length);

esp_err_t get_upstream_dns(char* str);
esp_err_t set_upstream_dns(char* str);
esp_err_t get_device_url(char* str);
esp_err_t set_device_url(char* str);
esp_err_t get_update_url(char* str);
esp_err_t set_update_url(char* str);

esp_err_t get_network_info(esp_netif_ip_info_t* info);
esp_err_t set_network_info(esp_netif_ip_info_t info);
esp_err_t get_enabled_interfaces(bool* eth, bool* wifi);
esp_err_t get_ethernet_phy_config(uint32_t* phy, uint32_t* addr, uint32_t* rst, uint32_t* mdc, uint32_t* mdio);

esp_err_t get_log_data(uint16_t* head, bool* full);
esp_err_t update_log_data(uint16_t head, bool full);

esp_err_t initialize_flash();

#endif