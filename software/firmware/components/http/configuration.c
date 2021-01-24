#include "configuration.h"
#include "accesspoint.h"
#include "storage.h"
#include "url.h"
#include "logging.h"

#include <sys/param.h>
#include "esp_wifi.h"
#include <esp_http_server.h>

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"
static const char *TAG = "HTTP";

static const char *apple_hotspot_detect = "/hotspot-detect.html";
static const char *apple_hotspot_detect_response = "";
static const char *windows10_hotspot_detect = "/connecttest.txt";
static const char *windows10_hotspot_detect_response = "Microsoft Connect Test";

static httpd_handle_t server;
static bool connected = false;


// static esp_err_t favicon_get_handler(httpd_req_t *req)
// {
//     ESP_LOGI(TAG, "Request for favicon.ico");

//     extern const unsigned char favicon_ico_start[] asm("_binary_favicon_ico_start");
//     extern const unsigned char favicon_ico_end[]   asm("_binary_favicon_ico_end");
//     const size_t favicon_ico_size = (favicon_ico_end - favicon_ico_start);

//     httpd_resp_set_type(req, "image/x-icon");
//     httpd_resp_send(req, (const char *)favicon_ico_start, favicon_ico_size);
//     return ESP_OK;
// }

static esp_err_t send_to_wifi_select(httpd_req_t *req)
{
    ESP_LOGD(TAG, "Redirecting request for %s", req->uri);
    httpd_resp_set_status(req, "307 Temporary Redirect");
    httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_send(req, NULL, 0);  // Response body can be empty
    return ESP_OK;
}

static esp_err_t wifi_txt_get_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Request for wifi.txt");
    httpd_resp_set_type(req, "text/plain; charset=UTF-8");

    uint16_t count;
    wifi_ap_record_t* list;
    get_scan_results(&count, &list);

    char buf[60];
    for(int i = 0; i < count; i++)
    {
        snprintf(buf, strlen((char*)list[i].ssid)+2, "%s\n", (char*)list[i].ssid);
        httpd_resp_sendstr_chunk(req, buf);
    }    

    httpd_resp_sendstr_chunk(req, NULL);

    return ESP_OK;
}

static esp_err_t select_wifi_get_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Request for /");
    httpd_resp_set_type(req, "text/html; charset=UTF-8");

    extern const unsigned char  provwifi_html_start[] asm("_binary_provwifi_html_start");
    extern const unsigned char  provwifi_html_end[]   asm("_binary_provwifi_html_end");
    const size_t  provwifi_html_size = (provwifi_html_end -  provwifi_html_start);
    httpd_resp_send_chunk(req, (const char *)provwifi_html_start, provwifi_html_size);
    httpd_resp_sendstr_chunk(req, NULL);

    return ESP_OK;
}

static esp_err_t parse_auth_data(char* buf, int bytes)
{
    int seperator_index = -1;
    for(int i = 0; i < bytes; i++)
    {
        if(buf[i] == '&')
        {
            seperator_index = i;

        }
    }

    if(seperator_index > 0)
    {  
        buf[seperator_index] = '\0'; // This splits the buffer into two null terminated strings
        wifi_config_t wifi_config = {0};
        strcpy((char*)wifi_config.sta.ssid, buf);
        strcpy((char*)wifi_config.sta.password, buf+seperator_index+1);
        // wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
        // wifi_config.sta.pmf_cfg.capable = true;
        // wifi_config.sta.pmf_cfg.required = false;
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));

        ESP_LOGI(TAG, "SSID: %s (%d)", (char*)&wifi_config.sta.ssid, strlen((char*)&wifi_config.sta.ssid));
        ESP_LOGI(TAG, "PASS: %s (%d)", (char*)&wifi_config.sta.password, strlen((char*)&wifi_config.sta.password));

    }

    return ESP_OK;
}

static esp_err_t wifi_auth_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "POST to /submitauth");

    if (req->content_len > 1000)
        return ESP_FAIL;
        
    char buf[req->content_len+1];
    int size;

    if ((size = httpd_req_recv(req, buf, req->content_len)) <= 0) {
        return ESP_FAIL;
    }

    /* Log data received */
    buf[req->content_len] = '\0';
    ESP_LOGI(TAG, "=========== RECEIVED DATA ==========");
    ESP_LOGI(TAG, "%s", buf);
    ESP_LOGI(TAG, "====================================");

    parse_auth_data(buf, size);
    if(test_authentication())
    {
        ESP_LOGI(TAG, "Connection Successful");
        httpd_resp_set_status(req, "200 OK");
        httpd_resp_set_type(req, "text/plain; charset=UTF-8");
        httpd_resp_sendstr(req, "Success");
        connected = true;
    } 
    else 
    {
        ESP_LOGI(TAG, "Could not connect to wifi");
        httpd_resp_set_status(req, "400 Bad Request");
        httpd_resp_set_type(req, "text/plain; charset=UTF-8");
        httpd_resp_sendstr(req, "Bad Credentials");
        connected = false;
    }
    
    return ESP_OK;
}

static esp_err_t connection_txt_get_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Request for /connection.txt");

    httpd_resp_set_type(req, "text/plain; charset=UTF-8");

    char buf[80];
    wifi_config_t wifi_config;
    esp_wifi_get_config(ESP_IF_WIFI_STA, &wifi_config);
    sprintf(buf, "%s\n", wifi_config.ap.ssid);
    httpd_resp_sendstr_chunk(req, buf);
    sprintf(buf, "%s\n", wifi_config.ap.password);
    httpd_resp_sendstr_chunk(req, buf);

    httpd_resp_sendstr_chunk(req, NULL);

    return ESP_OK;
}

static esp_err_t connected_get_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Request for /connected");

    httpd_resp_set_type(req, "text/html; charset=UTF-8");

    extern const unsigned char  provsettings_html_start[] asm("_binary_provsettings_html_start");
    extern const unsigned char  provsettings_html_end[]   asm("_binary_provsettings_html_end");
    const size_t  provsettings_html_size = (provsettings_html_end -  provsettings_html_start);
    httpd_resp_send_chunk(req, (const char *)provsettings_html_start, provsettings_html_size);
    httpd_resp_sendstr_chunk(req, NULL);

    return ESP_OK;
}

static void restart(void* arg)
{
    ESP_LOGI(TAG, "Request for restart");
    vTaskDelay(3000/ portTICK_PERIOD_MS);
    esp_restart();
}

static esp_err_t finished_get_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Request for /finished");
    
    ESP_ERROR_CHECK(store_default_lists());
    ESP_ERROR_CHECK(create_log_file());
    ESP_ERROR_CHECK(finish_configuration());

    httpd_resp_set_type(req, "text/html");
    extern const unsigned char  provfinished_html_start[] asm("_binary_provfinished_html_start");
    extern const unsigned char  provfinished_html_end[]   asm("_binary_provfinished_html_end");
    const size_t  provfinished_html_size = (provfinished_html_end -  provfinished_html_start);
    httpd_resp_send(req, (const char *)provfinished_html_start,  provfinished_html_size);

    xTaskCreatePinnedToCore(restart, "restarting", 8000, NULL, 5, 
                            NULL, tskNO_AFFINITY);

    return ESP_OK;
}

static esp_err_t get_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Request for %s", req->uri);

    if(strcmp(req->uri, "/") == 0)
    {
        return select_wifi_get_handler(req);
    }
    else if(!connected)
    {
        return send_to_wifi_select(req);
    }
    else{
        if(strcmp(req->uri, "/connected") == 0)
        {
            return connected_get_handler(req);
        }
        else if(strcmp(req->uri, "/finished") == 0)
        {
            return finished_get_handler(req);
        }
        else
        {
            return send_to_wifi_select(req);
        }
    }
}

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

static esp_err_t start_scan_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "POST to /scan");
    start_wifi_scan();
    httpd_resp_set_status(req, "200 OK");
    httpd_resp_set_type(req, "text/plain; charset=UTF-8");
    httpd_resp_sendstr(req, "Success");

    return ESP_OK;
}

void start_configuration_webserver()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting HTTP server");
    ESP_ERROR_CHECK(httpd_start(&server, &config));
    
    // httpd_uri_t favicon = {
    //     .uri       = "/favicon.ico",
    //     .method    = HTTP_GET,
    //     .handler   = favicon_get_handler,
    //     .user_ctx  = ""
    // };
    // httpd_register_uri_handler(server, &favicon);

    httpd_uri_t wifi_txt = {
        .uri       = "/wifi.txt",
        .method    = HTTP_GET,
        .handler   = wifi_txt_get_handler,
        .user_ctx  = ""
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &wifi_txt));

    httpd_uri_t start_scan = {
        .uri       = "/scan",
        .method    = HTTP_POST,
        .handler   = start_scan_handler,
        .user_ctx  = ""
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &start_scan));

    httpd_uri_t submit = {
        .uri       = "/submitauth",
        .method    = HTTP_POST,
        .handler   = wifi_auth_handler,
        .user_ctx  = ""
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &submit));

    httpd_uri_t connection_txt = {
        .uri       = "/connection.txt",
        .method    = HTTP_GET,
        .handler   = connection_txt_get_handler,
        .user_ctx  = ""
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &connection_txt));

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

    httpd_uri_t webpage = {
        .uri       = "/*",
        .method    = HTTP_GET,
        .handler   = get_handler,
        .user_ctx  = ""
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &webpage));

    ESP_LOGI(TAG, "Ready to serve configuration page");
}

void stop_configuration_webserver()
{
    ESP_ERROR_CHECK(httpd_stop(server));
}
