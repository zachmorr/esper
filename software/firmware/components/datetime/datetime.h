#ifndef DATETIME_H
#define DATETIME_H

#include <esp_system.h>

char* get_time_str(time_t time);
esp_err_t initialize_sntp();

#endif