#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <esp_http_server.h>

esp_err_t start_webserver();
esp_err_t stop_webserver();
esp_err_t start_application_webserver();
esp_err_t start_provisioning_webserver();
esp_err_t stop_provisioning_webserver();

#endif