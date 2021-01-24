#ifndef STORAGE_H
#define STORAGE_H

#include <esp_system.h>

typedef enum {
    STR,
    UINT8,
    UINT16,
    UINT32
} StorageType;

esp_err_t get_log_values(uint16_t* log_head, bool* full_flag);
esp_err_t update_log_values(uint16_t log_head, bool full_flag);

char* get_upstream_server(char* server);
esp_err_t update_upstream_server(char* server);

char* get_static_ip(char* ip);
esp_err_t update_static_ip(char* ip);

char* get_netmask(char* ip);
esp_err_t update_netmask(char* ip);

char* get_gateway(char* ip);
esp_err_t update_gateway(char* ip);

esp_err_t get_from_nvs(StorageType type, char* key, void* value);
esp_err_t store_in_nvs(StorageType type, char* key, void* value);

esp_err_t initialize_storage();
bool check_configuration_status();
esp_err_t finish_configuration();
esp_err_t reset_device();

#endif