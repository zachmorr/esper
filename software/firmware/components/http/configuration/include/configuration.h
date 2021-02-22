#ifndef CONFIGURATION_HTTP_H
#define CONFIGURATION_HTTP_H

#include <esp_http_server.h>

esp_err_t start_configuration_webserver();
void stop_configuration_webserver();

#endif