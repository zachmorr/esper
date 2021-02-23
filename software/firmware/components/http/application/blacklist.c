#include "blacklist.h"
#include "flash.h"
#include "url.h"
#include "cJSON.h"

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"
static const char *TAG = "HTTP";

static esp_err_t blacklist_json_get_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Request for blacklist.json");
    long start = esp_timer_get_time();

    int url_in_blacklist;
    char* blacklist = get_blacklist(&url_in_blacklist);
    if (blacklist == NULL)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Error retreiving blacklist");
        return ESP_OK;
    }
    ESP_LOGI(TAG, "%d URLs in blacklist", url_in_blacklist);

    // Initialize json object with empty array
    cJSON* json = cJSON_CreateObject();
    cJSON* blacklist_array = cJSON_CreateArray();
    if ( json == NULL || blacklist_array == NULL )
    {
        cJSON_Delete(json);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Error creating JSON object");
        return ESP_OK;
    }
    cJSON_AddItemToObject(json, "blacklist", blacklist_array);

    // Add urls in blacklist to json object
    int array_index = 0;
    for(int i = 0; i < url_in_blacklist; i++)
    {
        // Create url struct
        URL url = {0};
        url.length = blacklist[array_index];
        memcpy(url.string, blacklist+array_index+sizeof(url.length), url.length);

        cJSON* str = cJSON_CreateString(url.string);
        cJSON_AddItemToArray(blacklist_array, str);

        array_index += url.length+sizeof(url.length);
    }
    return_blacklist();

    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_status(req, "200 OK");
    httpd_resp_sendstr(req, cJSON_Print(json));
    cJSON_Delete(json);

    ESP_LOGD(TAG, "blacklist.json processing Time: %lld ms", (esp_timer_get_time()-start)/1000); 
    return ESP_OK;
}

static esp_err_t blacklist_get_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Request for blacklist.html");
    long start = esp_timer_get_time();

    httpd_resp_set_type(req, "text/html; charset=UTF-8");
    extern const unsigned char  blacklist_html_start[] asm("_binary_blacklist_html_start");
    extern const unsigned char  blacklist_html_end[]   asm("_binary_blacklist_html_end");
    const size_t  blacklist_html_size = (blacklist_html_end -  blacklist_html_start);
    httpd_resp_send(req, (const char *)blacklist_html_start,  blacklist_html_size);

    ESP_LOGD(TAG, "Blacklist.html rocessing Time: %lld ms", (esp_timer_get_time()-start)/1000);
    return ESP_OK;
}

static esp_err_t blacklist_add_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Request to add url to blacklist");
    long start = esp_timer_get_time();

    if (req->content_len > MAX_URL_LENGTH + 11)
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

    // Get url
    cJSON* json_url = cJSON_GetObjectItem(json, "url");
    if ( !cJSON_IsString(json_url) || json_url->valuestring == NULL)
    {
        ESP_LOGW(TAG, "Invalid key/value");
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid key/value");
        cJSON_Delete(json);
        return ESP_OK;
    }

    // Create url struct
    URL url = {};
    url.length = strlen(json_url->valuestring);
    memcpy(url.string, json_url->valuestring, url.length);

    esp_err_t err = add_to_blacklist(url);
    switch (err) {
        case ESP_OK:
            ESP_LOGI(TAG, "Added %.*s(%d) to blacklist", url.length, url.string, url.length);
            httpd_resp_set_status(req, "200 OK");
            httpd_resp_sendstr(req, "");
            break;

        case URL_ERR_INVALID_URL:
            ESP_LOGW(TAG, "Could not add %.*s(%d), invalid url", url.length, url.string, url.length);
            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid url");
            break;

        case URL_ERR_LIST_FUll:
            ESP_LOGW(TAG, "Could not add %.*s(%d), not enough memory", url.length, url.string, url.length);
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Not enough memory");
            break;

        case URL_ERR_LIST_UNAVAILBLE:
            ESP_LOGW(TAG, "Could not add %.*s(%d), error opening blacklist.json", url.length, url.string, url.length);
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Could not open blacklist file");
            break;

        case URL_ERR_ALREADY_EXISTS:
            ESP_LOGW(TAG, "Could not add %.*s(%d), already in blacklist", url.length, url.string, url.length);
            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Already in blacklist");
            break;

        default:
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Unknown error");
            break;

    }

    cJSON_Delete(json);
    ESP_LOGD(TAG, "Blacklist add processing Time: %lld ms", (esp_timer_get_time()-start)/1000);
    return ESP_OK;
}

static esp_err_t blacklist_delete_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Request to remove url from blacklist");
    long start = esp_timer_get_time();

    if (req->content_len > MAX_URL_LENGTH + 11)
    {
        ESP_LOGW(TAG, "Could not delete, URL too long");
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

    // Get url
    cJSON* json_url = cJSON_GetObjectItem(json, "url");
    if ( !cJSON_IsString(json_url) || json_url->valuestring == NULL)
    {
        ESP_LOGW(TAG, "Invalid key/value");
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid key/value");
        cJSON_Delete(json);
        return ESP_OK;
    }

    // Create url struct
    URL url = {};
    url.length = strlen(json_url->valuestring);
    memcpy(url.string, json_url->valuestring, url.length);

    esp_err_t err = remove_from_blacklist(url);
    switch (err) {
        case ESP_OK:
            ESP_LOGI(TAG, "%.*s(%d) removed from blacklist", url.length, url.string, url.length);
            httpd_resp_set_status(req, "200 OK");
            httpd_resp_sendstr(req, "");
            break;

        case URL_ERR_NOT_FOUND:
            ESP_LOGW(TAG, "%.*s(%d) not found in blacklist", url.length, url.string, url.length);
            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Url not in blacklist");
            break;

        case URL_ERR_LIST_UNAVAILBLE:
            ESP_LOGW(TAG, "Could not remove %.*s(%d), error opening blacklist.json", url.length, url.string, url.length);
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Could not open blacklist file");
            break;

        default:
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Unknown error");
            break;
    }

    cJSON_Delete(json);
    ESP_LOGD(TAG, "Blacklist delete processing Time: %lld ms", (esp_timer_get_time()-start)/1000);
    return ESP_OK;
}


esp_err_t setup_blacklist_handlers(httpd_handle_t server){
    esp_err_t err;
    
    httpd_uri_t blacklist = {
        .uri       = "/blacklist",
        .method    = HTTP_GET,
        .handler   = blacklist_get_handler,
        .user_ctx  = ""
    };
    err = httpd_register_uri_handler(server, &blacklist);

    httpd_uri_t blacklist_add = {
        .uri       = "/blacklist/add",
        .method    = HTTP_POST,
        .handler   = blacklist_add_handler,
        .user_ctx  = ""
    };
    err |= httpd_register_uri_handler(server, &blacklist_add);

    httpd_uri_t blacklist_delete = {
        .uri       = "/blacklist/delete",
        .method    = HTTP_POST,
        .handler   = blacklist_delete_handler,
        .user_ctx  = ""
    };
    err |= httpd_register_uri_handler(server, &blacklist_delete);

    httpd_uri_t blacklist_json = {
        .uri       = "/blacklist.json",
        .method    = HTTP_GET,
        .handler   = blacklist_json_get_handler,
        .user_ctx  = ""
    };
    err |= httpd_register_uri_handler(server, &blacklist_json);

    if( err != ESP_OK )
        return ESP_FAIL;
    return ESP_OK;
}