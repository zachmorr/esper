#include "configuration.h"
#include "wifi_select.h"
#include "connected.h"

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"
static const char *TAG = "HTTP";

static const char *apple_hotspot_detect = "/hotspot-detect.html";
static const char *apple_hotspot_detect_response = "";
static const char *windows10_hotspot_detect = "/connecttest.txt";
static const char *windows10_hotspot_detect_response = "Microsoft Connect Test";

static httpd_handle_t server;
static bool connected = false;


static esp_err_t apple_captive_portal_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Request for /%s", apple_hotspot_detect);
    httpd_resp_set_type(req, "text/plain; charset=UTF-8");
    httpd_resp_send(req, apple_hotspot_detect_response, strlen(apple_hotspot_detect_response));

    return ESP_OK;
}

static esp_err_t windows10_captive_portal_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Request for /%s", windows10_hotspot_detect);
    httpd_resp_set_type(req, "text/plain; charset=UTF-8");
    httpd_resp_send(req, windows10_hotspot_detect_response, strlen(windows10_hotspot_detect_response));

    return ESP_OK;
}

static esp_err_t configure_captive_portal_handlers(httpd_handle_t server)
{
    httpd_uri_t apple_captive_portal = {
        .uri       = apple_hotspot_detect,
        .method    = HTTP_GET,
        .handler   = apple_captive_portal_handler,
        .user_ctx  = ""
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &apple_captive_portal));

    httpd_uri_t windows10_captive_portal = {
        .uri       = windows10_hotspot_detect,
        .method    = HTTP_GET,
        .handler   = windows10_captive_portal_handler,
        .user_ctx  = ""
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &windows10_captive_portal));

    return ESP_OK;
}

void start_configuration_webserver()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting HTTP server");
    ESP_ERROR_CHECK(httpd_start(&server, &config));

    configure_captive_portal_handlers(server);
    configure_wifi_select_handler(server);
    configure_connected_handler(server);

    ESP_LOGI(TAG, "Ready to serve configuration page");
}

void stop_configuration_webserver()
{
    ESP_ERROR_CHECK(httpd_stop(server));
}