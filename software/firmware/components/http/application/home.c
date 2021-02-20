#include "home.h"
#include "logging.h"
#include "datetime.h"
#include "lwip/inet.h"
#include "cJSON.h"

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"
static const char *TAG = "HTTP";


static esp_err_t log_json_get_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Request for log.csv");
    long start = esp_timer_get_time();

    // Default values if query string doesn't exist
    uint32_t size = 10;
    uint32_t page = 0;

    // Check that the query len is appropriate
    uint32_t query_len = httpd_req_get_url_query_len(req) + 1;
    if (query_len > 25) // 25 is an arbitrary number that is larger than the request should ever be, assuming a log file size of 1MB
    {
        ESP_LOGW(TAG, "Query too large");
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Query too large");
        return ESP_OK;
    }
    else if (query_len > 1)
    {
        // check that query string is valid
        char query_buf[query_len];
        if ( httpd_req_get_url_query_str(req, query_buf, query_len) != ESP_OK)
        {
            ESP_LOGW(TAG, "Invalid query string");
            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid query string");
            return ESP_OK;
        }

        // retreive size and page from query string
        char s[5], p[10];
        if ( httpd_query_key_value(query_buf, "size", s, sizeof(s)) != ESP_OK || sscanf(s, "%i", &size) < 1 ||
             httpd_query_key_value(query_buf, "page", p, sizeof(p)) != ESP_OK || sscanf(p, "%i", &page) < 1) 
        {
            ESP_LOGW(TAG, "Invalid parameters");
            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid parameters");
            return ESP_OK;
        }

        if (size > 50)
            size = 50;
    }
    ESP_LOGI(TAG, "Page %d Size %d", page, size);

    // Retreive log information
    uint16_t log_head; bool full_flag;
    if (get_log_head(&log_head, &full_flag) != ESP_OK)
    {
        ESP_LOGW(TAG, "Unable to get log location");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Unable to get log location");
        return ESP_OK;
    }

    // Determine number of log entries
    uint16_t log_entries = 0;
    if (full_flag)
        log_entries = MAX_LOGS;
    else
        log_entries = MAX_LOGS-log_head;

    ESP_LOGI(TAG, "Log Info: head=%d full=%d size=%d", log_head, full_flag, log_entries);

    // Check that page & size parameters aren't larger than number of entries
    if ( log_entries > 0 && (size*page >= log_entries || size > MAX_LOGS) )
    {
        ESP_LOGW(TAG, "Page & size out of bounds");
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Page & size out of bounds");
        return ESP_OK;
    }

    // Open log file
    FILE* log = fopen("/spiffs/log", "r");
    if (log == NULL)
    {
        ESP_LOGW(TAG, "Unable to open log");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Unable open log file");
        return ESP_OK;
    }

    // Initialize json object with empty array
    cJSON* json = cJSON_CreateObject();
    cJSON* log_entry_array = cJSON_CreateArray();
    if ( json == NULL || log_entry_array == NULL )
    {
        cJSON_Delete(json);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Error creating JSON object");
        return ESP_OK;
    }
    cJSON_AddItemToObject(json, "log_entries", log_entry_array);

    // Calculate current cursor position and move to position in file
    uint16_t current_log_cursor = (log_head + page*size) % MAX_LOGS;
    fseek(log, current_log_cursor*sizeof(Log_Entry), SEEK_SET);
    for(int i = 0; i < size; i++)
    {
        // Increment cursor
        if (current_log_cursor == MAX_LOGS)
        {
            if(full_flag)
            {
                current_log_cursor = 0;
                fseek(log, 0, SEEK_SET);
            }
            else
                break;
        }
        ESP_LOGV(TAG, "Log cursor: %d(%d)", current_log_cursor, current_log_cursor*sizeof(Log_Entry));

        // Build json entry
        Log_Entry entry = {0};
        if( fread(&entry, sizeof(entry), 1, log) > 0 )
        {
            cJSON* e = cJSON_CreateObject();
            if ( e != NULL)
            {
                cJSON_AddStringToObject(e, "time", get_time_str(entry.time));
                cJSON_AddStringToObject(e, "url", entry.url.string);
                cJSON_AddStringToObject(e, "client", inet_ntoa(entry.client));
                cJSON_AddBoolToObject(e, "blocked", entry.blocked);
                cJSON_AddItemToArray(log_entry_array, e);
            }
            else
                ESP_LOGW(TAG, "Error adding entry to json array");
        }
        current_log_cursor++;
    }
    fclose(log);

    httpd_resp_set_type(req, "application/json"); 
    httpd_resp_set_status(req, "200 OK");
    httpd_resp_sendstr(req, cJSON_Print(json));
    cJSON_Delete(json);

    ESP_LOGD(TAG, "Log.csv processing time: %lld ms", (esp_timer_get_time()-start)/1000);
    return ESP_OK;
}

static esp_err_t homepage_get_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Request for homepage.html");
    long start = esp_timer_get_time();

    httpd_resp_set_type(req, "text/html; charset=UTF-8");
    extern const unsigned char  home_html_start[] asm("_binary_home_html_start");
    extern const unsigned char  home_html_end[]   asm("_binary_home_html_end");
    const size_t  home_html_size = (home_html_end -  home_html_start);
    httpd_resp_send(req, (const char *)home_html_start,  home_html_size);
    ESP_LOGD(TAG, "Homepage.html processing Time: %lld ms", (esp_timer_get_time()-start)/1000);

    return ESP_OK;
}

esp_err_t setup_homepage_handlers(httpd_handle_t server){
    esp_err_t err;

    httpd_uri_t root = {
        .uri       = "/",
        .method    = HTTP_GET,
        .handler   = homepage_get_handler,
        .user_ctx  = ""
    };
    err = httpd_register_uri_handler(server, &root);

    httpd_uri_t homepage = {
        .uri       = "/homepage",
        .method    = HTTP_GET,
        .handler   = homepage_get_handler,
        .user_ctx  = ""
    };
    err |= httpd_register_uri_handler(server, &homepage);

    httpd_uri_t log_json = {
        .uri       = "/log.json",
        .method    = HTTP_GET,
        .handler   = log_json_get_handler,
        .user_ctx  = ""
    };
    err |= httpd_register_uri_handler(server, &log_json);
    if( err != ESP_OK )
        return ESP_FAIL;

    return ESP_OK;
}