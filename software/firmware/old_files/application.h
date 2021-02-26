#ifndef APPLICATION_HTTP_H
#define APPLICATION_HTTP_H

#include <esp_http_server.h>

esp_err_t start_application_webserver();
void stop_application_webserver();

#endif