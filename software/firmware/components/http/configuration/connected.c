#include "connected.h"
#include "flash.h"
#include "url.h"
#include "logging.h"
#include "esp_wifi.h"
#include <esp_http_server.h>
#include "cJSON.h"
#include "freertos/timers.h"

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"
static const char *TAG = "HTTP";

static esp_err_t connection_json_get_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Request for /connection.json");

    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_status(req, "200 OK");

    cJSON *connection = cJSON_CreateObject(); 
    
    wifi_config_t wifi_config;
    esp_wifi_get_config(ESP_IF_WIFI_STA, &wifi_config);
    cJSON_AddStringToObject(connection, "ssid", (char*)wifi_config.ap.ssid);

    char ip[16];
    nvs_get("ip", (void*)ip, 16);
    cJSON_AddStringToObject(connection, "ip", ip);

    httpd_resp_sendstr(req, cJSON_Print(connection));
    return ESP_OK;
}

static esp_err_t connected_get_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Request for /connected");

    httpd_resp_set_type(req, "text/html; charset=UTF-8");

    extern const unsigned char  connected_html_start[] asm("_binary_connected_html_start");
    extern const unsigned char  connected_html_end[]   asm("_binary_connected_html_end");
    const size_t  connected_html_size = (connected_html_end -  connected_html_start);
    httpd_resp_send_chunk(req, (const char *)connected_html_start, connected_html_size);
    httpd_resp_sendstr_chunk(req, NULL);

    return ESP_OK;
}

static void restart( TimerHandle_t xTimer )
{
    ESP_LOGW(TAG, "Restarting...");
    esp_restart();
}

static esp_err_t finish_setup_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Finishing setup");

    if (store_default_lists() != ESP_OK)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Unable to create blacklist file");
        return ESP_OK;
    }

    // if (create_log_file() != ESP_OK)
    // {
    //     httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Unable allocate space for log file");
    //     return ESP_OK;
    // }

    // if (set_defaults() != ESP_OK)
    // {
    //     httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Unable to set default settings");
    //     return ESP_OK;
    // }

    ESP_LOGI(TAG, "Configuration finished");
    xTimerHandle restartTimer = xTimerCreate("Restart timer", pdMS_TO_TICKS(1000), pdFALSE, (void*)0, restart);
    xTimerStart(restartTimer, 0);
    
    httpd_resp_set_status(req, "200 OK");
    httpd_resp_sendstr(req, "");
    return ESP_OK;
}

esp_err_t configure_connected_handler(httpd_handle_t server)
{
    httpd_uri_t connected = {
        .uri       = "/connected",
        .method    = HTTP_GET,
        .handler   = connected_get_handler,
        .user_ctx  = ""
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &connected));

    httpd_uri_t connection_json = {
        .uri       = "/connection.json",
        .method    = HTTP_GET,
        .handler   = connection_json_get_handler,
        .user_ctx  = ""
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &connection_json));

    httpd_uri_t finished = {
        .uri       = "/finish",
        .method    = HTTP_POST,
        .handler   = finish_setup_handler,
        .user_ctx  = ""
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &finished));

    return ESP_OK;
}