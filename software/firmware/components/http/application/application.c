#include "application.h"
#include "home.h"
#include "blacklist.h"
#include "settings.h"
#include <esp_http_server.h>

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"
static const char *TAG = "HTTP";

static httpd_handle_t server;


// static esp_err_t favicon_get_handler(httpd_req_t *req)
// {
//     ESP_LOGI(TAG, "Request for favicon.ico");
//     long start = esp_timer_get_time();

//     extern const unsigned char favicon_ico_start[] asm("_binary_favicon_ico_start");
//     extern const unsigned char favicon_ico_end[]   asm("_binary_favicon_ico_end");
//     const size_t favicon_ico_size = (favicon_ico_end - favicon_ico_start);

//     httpd_resp_set_type(req, "image/x-icon");
//     httpd_resp_send(req, (const char *)favicon_ico_start, favicon_ico_size);
//     ESP_LOGD(TAG, "Favicon.ico processing time: %lld ms", (esp_timer_get_time()-start)/1000);
//     return ESP_OK;
// }

// static esp_err_t logo_get_handler(httpd_req_t *req)
// {
//     ESP_LOGI(TAG, "Request for logo.png");
//     long start = esp_timer_get_time();

//     extern const unsigned char logo_png_start[] asm("_binary_logo_png_start");
//     extern const unsigned char logo_png_end[]   asm("_binary_logo_png_end");
//     const size_t logo_png_size = (logo_png_end - logo_png_start);

//     httpd_resp_set_type(req, "image/png");
//     httpd_resp_send(req, (const char *)logo_png_start, logo_png_size);
//     ESP_LOGD(TAG, "Logo.png processing Time: %lld ms", (esp_timer_get_time()-start)/1000);
//     return ESP_OK;
// }

// static void setup_image_handlers(){
//     httpd_uri_t favicon = {
//         .uri       = "/favicon.ico",
//         .method    = HTTP_GET,
//         .handler   = favicon_get_handler,
//         .user_ctx  = ""
//     };
//     httpd_register_uri_handler(server, &favicon);

//     httpd_uri_t logo = {
//         .uri       = "/logo.png",
//         .method    = HTTP_GET,
//         .handler   = logo_get_handler,
//         .user_ctx  = ""
//     };
//     httpd_register_uri_handler(server, &logo);
// }

void start_application_webserver()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;
    config.max_uri_handlers = 30;
    config.stack_size = 8192;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    ESP_ERROR_CHECK(httpd_start(&server, &config));

    // setup_image_handlers();
    setup_homepage_handlers(server);
    setup_blacklist_handlers(server);
    setup_settings_handlers(server);
    
    ESP_LOGI(TAG, "Ready to serve application page");
}

void stop_application_webserver()
{
    ESP_ERROR_CHECK(httpd_stop(server));
}
