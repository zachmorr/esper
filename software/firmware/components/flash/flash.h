#ifndef FLASH_H
#define FLASH_H

#include <esp_system.h>

#define GOOGLE_IP "8.8.8.8"
#define CLOUDFARE_IP "1.1.1.1"
#define OPENDNS_IP "208.67.222.222"
#define ADGUARD_IP "94.140.14.14"

#define IP4_STRLEN_MAX 16
#define HOSTNAME_STRLEN_MAX 255

esp_err_t nvs_set(char* key, void* value, size_t length);
esp_err_t nvs_get(char* key, void* value, size_t length);
esp_err_t nvs_get_length(char* key, size_t* length);

esp_err_t initialize_flash();
esp_err_t check_configuration_status(bool* configured);
esp_err_t set_defaults();
esp_err_t reset_device();

#endif