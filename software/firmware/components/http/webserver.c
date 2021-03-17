#include "webserver.h"
#include "blacklist.h"
#include "home.h"
#include "settings.h"
#include "connected.h"
#include "wifi_select.h"
#include "error.h"

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"
static const char *TAG = "HTTP";

static const char *apple_hotspot_detect_response = "";
static const char *windows10_hotspot_detect_response = "Microsoft Connect Test";

static httpd_handle_t server;

esp_err_t start_webserver()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;
    config.max_uri_handlers = 30;
    config.stack_size = 8192;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    ERROR_CHECK(httpd_start(&server, &config))

    return ESP_OK;
}

esp_err_t start_application_webserver()
{
    ESP_LOGI(TAG, "Starting Application Webserver");
    ERROR_CHECK(setup_homepage_handlers(server))
    ERROR_CHECK(setup_blacklist_handlers(server))
    ERROR_CHECK(setup_settings_handlers(server))
    return ESP_OK;
}

static esp_err_t apple_captive_portal_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Request for /hotspot-detect.html");
    httpd_resp_set_type(req, "text/plain; charset=UTF-8");
    httpd_resp_send(req, apple_hotspot_detect_response, strlen(apple_hotspot_detect_response));

    return ESP_OK;
}

static httpd_uri_t apple_captive_portal = {
    .uri       = "/hotspot-detect.html",
    .method    = HTTP_GET,
    .handler   = apple_captive_portal_handler,
    .user_ctx  = ""
};

static esp_err_t windows10_captive_portal_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Request for /connecttest.txt");
    httpd_resp_set_type(req, "text/plain; charset=UTF-8");
    httpd_resp_send(req, windows10_hotspot_detect_response, strlen(windows10_hotspot_detect_response));

    return ESP_OK;
}

static httpd_uri_t windows10_captive_portal = {
    .uri       = "/connecttest.txt",
    .method    = HTTP_GET,
    .handler   = windows10_captive_portal_handler,
    .user_ctx  = ""
};

static esp_err_t configure_captive_portal_handlers(httpd_handle_t server)
{
    ERROR_CHECK(httpd_register_uri_handler(server, &apple_captive_portal))
    ERROR_CHECK(httpd_register_uri_handler(server, &windows10_captive_portal))

    return ESP_OK;
}

static esp_err_t teardown_captive_portal_handlers(httpd_handle_t server)
{
    ERROR_CHECK(httpd_unregister_uri_handler(server, apple_captive_portal.uri, apple_captive_portal.method))
    ERROR_CHECK(httpd_unregister_uri_handler(server, windows10_captive_portal.uri, windows10_captive_portal.method))

    return ESP_OK;
}

esp_err_t start_provisioning_webserver()
{
    ESP_LOGI(TAG, "Starting Provisioning Webserver");
    ERROR_CHECK(configure_captive_portal_handlers(server))
    ERROR_CHECK(configure_wifi_select_handler(server))
    ERROR_CHECK(configure_connected_handler(server))
    return ESP_OK;
}

esp_err_t stop_provisioning_webserver()
{
    ESP_LOGI(TAG, "Starting Provisioning Webserver");
    ERROR_CHECK(teardown_captive_portal_handlers(server))
    ERROR_CHECK(teardown_wifi_select_handler(server))
    ERROR_CHECK(teardown_connected_handler(server))
    return ESP_OK;
}

esp_err_t stop_webserver()
{
    ERROR_CHECK(httpd_stop(server));
    return ESP_OK;
}