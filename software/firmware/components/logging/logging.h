#ifndef LOGGING_H
#define LOGGING_H

#include <esp_system.h>
#include "url.h"

#define MAX_LOGS 100

#define LOG_ERR_LOG_UNAVAILABLE 0X300

typedef struct {
    time_t time;
    uint16_t type;
    uint32_t client;
    URL url;
    bool blocked;
} Log_Entry;

esp_err_t get_log_head(uint16_t* head, bool* flag);

esp_err_t log_query(URL url, bool blocked, uint32_t client);
esp_err_t create_log_file();
esp_err_t initialize_logging();

#endif