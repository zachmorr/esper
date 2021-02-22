#include "application.h"
#include "home.h"
#include "blacklist.h"
#include "settings.h"
#include <esp_http_server.h>

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"
static const char *TAG = "HTTP";

static httpd_handle_t server;


esp_err_t start_application_webserver()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;
    config.max_uri_handlers = 30;
    config.stack_size = 8192;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    esp_err_t err = httpd_start(&server, &config);

    // setup_image_handlers();
    err |= setup_homepage_handlers(server);
    err |= setup_blacklist_handlers(server);
    err |= setup_settings_handlers(server);
    if( err != ESP_OK )
        return ESP_FAIL;
    
    ESP_LOGI(TAG, "Ready to serve application page");
    return ESP_OK;
}

void stop_application_webserver()
{
    ESP_ERROR_CHECK(httpd_stop(server));
}
