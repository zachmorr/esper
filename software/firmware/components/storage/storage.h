#ifndef STORAGE_H
#define STORAGE_H

#include <esp_system.h>

esp_err_t nvs_set(char* key, void* value, size_t length);
esp_err_t nvs_get(char* key, void* value, size_t length);
esp_err_t nvs_get_length(char* key, size_t* length);

esp_err_t initialize_storage();
bool check_configuration_status();
esp_err_t set_defaults();
esp_err_t reset_device();

#endif