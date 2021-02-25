#include "settings.h"
#include "dns.h"
#include "ota.h"
#include "flash.h"
// #include "station.h"
#include "url.h"
#include "cJSON.h"
#include "esp_ota_ops.h"
#include "esp_netif.h"


#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"
static const char *TAG = "HTTP";

static esp_err_t settings_get_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Request for settings.html");
    long start = esp_timer_get_time();

    httpd_resp_set_type(req, "text/html; charset=UTF-8");
    extern const unsigned char  settings_html_start[] asm("_binary_settings_html_start");
    extern const unsigned char  settings_html_end[]   asm("_binary_settings_html_end");
    const size_t  settings_html_size = (settings_html_end -  settings_html_start);
    httpd_resp_send(req, (const char *)settings_html_start,  settings_html_size);
    ESP_LOGD(TAG, "Settings.html processing Time: %lld ms", (esp_timer_get_time()-start)/1000);

    return ESP_OK;
}

static esp_err_t settings_json_get_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Request for settings.json");   
    cJSON *json = cJSON_CreateObject();
    if ( json == NULL )
    {
        cJSON_Delete(json);
        ESP_LOGW(TAG, "Error creating json object");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Error creating JSON object");
        return ESP_OK;
    }

    bool blocking = blocking_on();
    cJSON_AddBoolToObject(json, "blocking", blocking);

    esp_netif_ip_info_t info;
    get_network_info(&info);
    char* ip = inet_ntoa(info.ip);
    // nvs_get("ip", (void*)ip, 0);
    cJSON_AddStringToObject(json, "ip", ip);

    char url[MAX_URL_LENGTH];
    nvs_get("url", (void*)url, 0);
    cJSON_AddStringToObject(json, "url", url);

    char dnssrv[IP4ADDR_STRLEN_MAX];
    nvs_get("upstream_server", (void*)dnssrv, 0);
    cJSON_AddStringToObject(json, "dnssrv", dnssrv);

    char updatesrv[MAX_URL_LENGTH+HTTPD_MAX_URI_LEN];
    nvs_get("update_url", (void*)updatesrv, 0);
    cJSON_AddStringToObject(json, "updatesrv", updatesrv);
    
    const esp_app_desc_t* firmware = esp_ota_get_app_description();
    cJSON_AddStringToObject(json, "version", firmware->version);

    bool update_available = is_update_available();
    cJSON_AddBoolToObject(json, "update", update_available);

    httpd_resp_set_type(req, "application/json;");
    httpd_resp_set_status(req, "200 OK");
    httpd_resp_sendstr(req, cJSON_Print(json));
    cJSON_Delete(json);

    return ESP_OK;
}

static esp_err_t settings_json_post_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "%d %s %d", req->method, req->uri, req->content_len);

    if ( req->content_len > (MAX_URL_LENGTH*2+CONFIG_HTTPD_MAX_URI_LEN+IP4ADDR_STRLEN_MAX*2+100) )
    {
        ESP_LOGW(TAG, "Could not add, URL too long");
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Content length too large");
        return ESP_OK;
    }

    // check for valid json
    char data[req->content_len];
    httpd_req_recv(req, data, req->content_len);
    cJSON* json = cJSON_Parse(data);
    if (json == NULL)
    {
        ESP_LOGW(TAG, "Invalid json");
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid json");
        cJSON_Delete(json);
        return ESP_OK;
    }

    // Get ip
    cJSON* ip = cJSON_GetObjectItem(json, "ip");
    if ( !cJSON_IsString(ip) || ip->valuestring == NULL)
    {
        ESP_LOGW(TAG, "Invalid IP");
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid IP");
        cJSON_Delete(json);
        return ESP_OK;
    }
    ESP_LOGI(TAG, "IP: %s", ip->valuestring);
    esp_netif_ip_info_t info;
    get_network_info(&info);
    inet_aton(ip->valuestring, &(info.ip));
    set_network_info(info);
    // nvs_set("ip", (void*)ip->valuestring, strlen(ip->valuestring)+1);

    // Get url
    cJSON* url = cJSON_GetObjectItem(json, "url");
    if ( !cJSON_IsString(url) || url->valuestring == NULL)
    {
        ESP_LOGW(TAG, "Invalid url");
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid url");
        cJSON_Delete(json);
        return ESP_OK;
    }
    ESP_LOGI(TAG, "url: %s", url->valuestring);
    nvs_set("url", (void*)url->valuestring, strlen(url->valuestring)+1);

    // Get dnssrv
    cJSON* dnssrv = cJSON_GetObjectItem(json, "dnssrv");
    if ( !cJSON_IsString(dnssrv) || dnssrv->valuestring == NULL)
    {
        ESP_LOGW(TAG, "Invalid dnssrv");
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid dnssrv");
        cJSON_Delete(json);
        return ESP_OK;
    }
    ESP_LOGI(TAG, "dnssrv: %s", dnssrv->valuestring);
    nvs_set("upstream_server", (void*)dnssrv->valuestring, strlen(dnssrv->valuestring)+1);

    // Get updatesrv
    cJSON* updatesrv = cJSON_GetObjectItem(json, "updatesrv");
    if ( !cJSON_IsString(updatesrv) || updatesrv->valuestring == NULL)
    {
        ESP_LOGW(TAG, "Invalid updatesrv");
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid updatesrv");
        cJSON_Delete(json);
        return ESP_OK;
    }
    ESP_LOGI(TAG, "updatesrv: %s", updatesrv->valuestring);
    nvs_set("update_url", (void*)updatesrv->valuestring, strlen(updatesrv->valuestring)+1);

    // Reload things that rely on one of the settings
    // set_static_ip();
    set_device_url();
    initialize_upstream_socket();
    check_for_update();

    httpd_resp_set_status(req, "200 OK");
    httpd_resp_sendstr(req, "");
    cJSON_Delete(json);

    return ESP_OK;
}

static esp_err_t toggle_blocking_handler(httpd_req_t *req){
    ESP_LOGI(TAG, "Request to toggle blocking");

    if ( toggle_blocking() != ESP_OK )
    {
        ESP_LOGW(TAG, "Error toggling blocking status");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Error toggling blocking status");
        return ESP_OK;
    }

    cJSON *json = cJSON_CreateObject();
    if ( json == NULL )
    {
        cJSON_Delete(json);
        ESP_LOGW(TAG, "Error creating json object");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Error creating JSON object");
        return ESP_OK;
    }

    bool blocking = blocking_on();
    cJSON_AddBoolToObject(json, "blocking", blocking);

    httpd_resp_set_type(req, "application/json;");
    httpd_resp_set_status(req, "200 OK");
    httpd_resp_sendstr(req, cJSON_Print(json));
    cJSON_Delete(json);

    return ESP_OK;
}

static esp_err_t updatefirmware_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Request for firmware update");

    start_ota();

    httpd_resp_set_status(req, "200 OK");
    httpd_resp_sendstr(req, "Done");

    return ESP_OK;
}

static esp_err_t ota_status_get_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Request for ota status");

    TaskHandle_t ota_task_handle = get_ota_task_handle();

    if (ota_task_handle != NULL)
    {
        eTaskState ota_state = eTaskGetState(ota_task_handle);

        ESP_LOGI(TAG, "Request for firmware update status (%d)", ota_state);
        httpd_resp_set_type(req, "text/plain; charset=UTF-8");
        httpd_resp_sendstr(req, "running...");
    }
    else
    {
        httpd_resp_sendstr(req, "done");
    }

    return ESP_OK;
}

esp_err_t setup_settings_handlers(httpd_handle_t server){
    esp_err_t err;

    httpd_uri_t settings = {
        .uri       = "/settings",
        .method    = HTTP_GET,
        .handler   = settings_get_handler,
        .user_ctx  = ""
    };
    err = httpd_register_uri_handler(server, &settings);

    httpd_uri_t settings_json_get = {
        .uri       = "/settings.json",
        .method    = HTTP_GET,
        .handler   = settings_json_get_handler,
        .user_ctx  = ""
    };
    err |= httpd_register_uri_handler(server, &settings_json_get);

    httpd_uri_t settings_json_post = {
        .uri       = "/settings.json",
        .method    = HTTP_POST,
        .handler   = settings_json_post_handler,
        .user_ctx  = ""
    };
    err |= httpd_register_uri_handler(server, &settings_json_post);

    httpd_uri_t set_blocking_status = {
        .uri       = "/toggleblock",
        .method    = HTTP_POST,
        .handler   = toggle_blocking_handler,
        .user_ctx  = ""
    };
    err |= httpd_register_uri_handler(server, &set_blocking_status);

    httpd_uri_t update_firmware = {
        .uri       = "/updatefirmware",
        .method    = HTTP_POST,
        .handler   = updatefirmware_handler,
        .user_ctx  = ""
    };
    err |= httpd_register_uri_handler(server, &update_firmware);

    httpd_uri_t ota_status = {
        .uri       = "/updatestatus",
        .method    = HTTP_GET,
        .handler   = ota_status_get_handler,
        .user_ctx  = ""
    };
    err |= httpd_register_uri_handler(server, &ota_status);

    if( err != ESP_OK )
        return ESP_FAIL;
    return ESP_OK;
}