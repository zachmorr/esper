#include "application.h"
#include "station.h"
#include "dns.h"
#include "storage.h"
#include "url.h"
#include "logging.h"
#include "datetime.h"
#include "ota.h"

#include <sys/param.h>
#include "esp_wifi.h"
#include <esp_http_server.h>
#include "esp_spiffs.h"
#include "esp_timer.h"
#include "lwip/netdb.h"

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

static esp_err_t log_csv_get_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Request for log.csv");
    long start = esp_timer_get_time();

    uint32_t query_len = httpd_req_get_url_query_len(req) + 1;
    if (query_len > 1 && query_len < 25) // 25 is an arbitrary number that is larger than the request should ever be, assuming a log file size of 1MB
    {
        char query_buf[query_len];
        httpd_req_get_url_query_str(req, query_buf, query_len);

        char param[query_len];
        uint32_t size, page;
        if (httpd_query_key_value(query_buf, "size", param, sizeof(param)) == ESP_OK && (size = atoi(param)) > 0) 
        {
            ESP_LOGD(TAG, "Found URL query parameter => size=%d", size);

            if (httpd_query_key_value(query_buf, "page", param, sizeof(param)) == ESP_OK && (page = atoi(param)) > 0) 
            {
                page -= 1;
                ESP_LOGD(TAG, "Found URL query parameter => page=%d", page);

                uint16_t log_head; bool full_flag;
                get_log_head(&log_head, &full_flag);

                uint16_t log_entries = 0;
                if (full_flag)
                    log_entries = MAX_LOGS;
                else
                    log_entries = MAX_LOGS-log_head;

                ESP_LOGI(TAG, "Log Info: head=%d full=%d size=%d", log_head, full_flag, log_entries);

                if (size*page < log_entries && size <= MAX_LOGS)
                {
                    httpd_resp_set_type(req, "text/plain"); 

                    char size_buf[10];
                    sprintf(size_buf, "%d\n", log_entries);
                    httpd_resp_sendstr_chunk(req, size_buf);
                    httpd_resp_sendstr_chunk(req, "Time,Domain,Client,Blocked\n");

                    FILE* log = fopen("/spiffs/log", "r");
                    if(log)
                    {
                        uint16_t current_log_cursor = (log_head + page*size) % MAX_LOGS;
                        fseek(log, current_log_cursor*sizeof(Log_Entry), SEEK_SET);
                        for(int i = 0; i < size; i++)
                        {
                            if (current_log_cursor == MAX_LOGS)
                            {
                                if(full_flag)
                                {
                                    current_log_cursor = 0;
                                    fseek(log, 0, SEEK_SET);
                                }
                                else
                                {
                                    break;
                                }
                            }
                            ESP_LOGD(TAG, "Log cursor: %d(%d)", current_log_cursor, current_log_cursor*sizeof(Log_Entry));

                            Log_Entry entry = {0};
                            if( fread(&entry, sizeof(entry), 1, log) > 0 )
                            {
                                char buf[sizeof(entry)];
                                sprintf(buf, "%s,%s,%s,%d\n", get_time_str(entry.time), entry.url.string, 
                                                            inet_ntoa(entry.client), entry.blocked);
                                ESP_LOGD(TAG, "%s", buf);
                                httpd_resp_sendstr_chunk(req, buf);

                            } 
                            current_log_cursor++;
                        }
                        
                        fclose(log);
                    }
                    httpd_resp_sendstr_chunk(req, NULL);
                }
                else
                {
                    ESP_LOGW(TAG, "Request parameters for log.csv are out of bounds");
                    httpd_resp_set_status(req, "400 Bad Request");
                    httpd_resp_set_type(req, "text/plain");
                    httpd_resp_sendstr(req, "Request parameters for log.csv are out of bounds");
                }
            }
            else
            {
                ESP_LOGW(TAG, "Invalid page parameter for log.csv");
                httpd_resp_set_status(req, "400 Bad Request");
                httpd_resp_set_type(req, "text/plain");
                httpd_resp_sendstr(req, "Invalid page parameter");
            }
        }
        else
        {
            ESP_LOGW(TAG, "Invalid size parameter for log.csv");
            httpd_resp_set_status(req, "400 Bad Request");
            httpd_resp_set_type(req, "text/plain");
            httpd_resp_sendstr(req, "Invalid size parameter");
        }
    }
    else
    {
        ESP_LOGW(TAG, "Invalid request for log.csv");
        httpd_resp_set_status(req, "400 Bad Request");
        httpd_resp_set_type(req, "text/plain");
        httpd_resp_sendstr(req, "Invalid request parameters for log.csv");
    }
    
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

static esp_err_t blacklist_txt_get_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Request for blacklist.txt");
    long start = esp_timer_get_time();
    httpd_resp_set_type(req, "text/plain; charset=UTF-8");

    int url_in_blacklist;
    char* blacklist = get_blacklist(&url_in_blacklist);
    ESP_LOGI(TAG, "%d URLs in blacklist", url_in_blacklist);

    int array_index = 0;
    for(int i = 0; i < url_in_blacklist; i++)
    {
        URL url = {0};
        url.length = blacklist[array_index];
        memcpy(url.string, blacklist+array_index+sizeof(url.length), url.length);

        char buf[url.length+2]; // +2 for \n and \0
        snprintf(buf,url.length+2, "%s\n", url.string);
        httpd_resp_sendstr_chunk(req, buf);
        array_index += url.length+sizeof(url.length);
    }
    return_blacklist();

    httpd_resp_sendstr_chunk(req, NULL);
    ESP_LOGD(TAG, "Blacklist.txt processing Time: %lld ms", (esp_timer_get_time()-start)/1000);
    
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

    if (req->content_len > MAX_URL_LENGTH)
    {
        ESP_LOGW(TAG, "Could not add, URL too long");
        httpd_resp_set_status(req, "400 Bad Request");
        httpd_resp_set_type(req, "text/plain");
        httpd_resp_sendstr(req, "URL too long");
    }
    else
    {
        URL url = {0};
        if ((url.length = httpd_req_recv(req, url.string, req->content_len)) <= 0) {
            return ESP_FAIL;
        }

        if(valid_url(url))
        {
            esp_err_t ret = add_to_blacklist(url);
            if (ret == ESP_OK)
            {
                ESP_LOGI(TAG, "Added %.*s(%d) to blacklist", url.length, url.string, url.length);
                httpd_resp_set_status(req, "200 OK");
                httpd_resp_set_type(req, "text/plain");
                httpd_resp_sendstr(req, "Success");
            }
            else if (ret == URL_ERR_LIST_FUll)
            {
                ESP_LOGW(TAG, "Could not add %.*s(%d), not enough memory", url.length, url.string, url.length);
                httpd_resp_set_status(req, "500");
                httpd_resp_set_type(req, "text/plain");
                httpd_resp_sendstr(req, "Blacklist is full, try deleting entries");
            }
            else if (ret == URL_ERR_LIST_UNAVAILBLE)
            {
                ESP_LOGW(TAG, "Could not add %.*s(%d), error opening blacklist.txt", url.length, url.string, url.length);
                httpd_resp_set_status(req, "500");
                httpd_resp_set_type(req, "text/plain");
                httpd_resp_sendstr(req, "Could not open blacklist file");
            }
            else if(ret == URL_ERR_ALREADY_EXISTS)
            {
                ESP_LOGW(TAG, "Could not add %.*s(%d), already in blacklist", url.length, url.string, url.length);
                httpd_resp_set_status(req, "400 Bad Request");
                httpd_resp_set_type(req, "text/plain");
                httpd_resp_sendstr(req, "Already in blacklist");
            }
        }
        else
        {
            ESP_LOGW(TAG, "Could not add %.*s(%d), invalid URL", url.length, url.string, url.length);
            httpd_resp_set_status(req, "400 Bad Request");
            httpd_resp_set_type(req, "text/plain");
            httpd_resp_sendstr(req, "Invalid URL");
        }
    }
    ESP_LOGD(TAG, "Blacklist add processing Time: %lld ms", (esp_timer_get_time()-start)/1000);
    return ESP_OK;
}

static esp_err_t blacklist_delete_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Request to remove url from blacklist");
    long start = esp_timer_get_time();
    if (req->content_len > MAX_URL_LENGTH)
    {
        ESP_LOGI(TAG, "Could not delete, URL too long");
        httpd_resp_set_status(req, "400 Bad Request");
        httpd_resp_set_type(req, "text/plain");
        httpd_resp_sendstr(req, "URL too long");
    }
    else
    {
        URL url = {0};
        if ((url.length = httpd_req_recv(req, url.string, req->content_len)) <= 0) {
            return ESP_FAIL;
        }

        if(valid_url(url))
        {
            esp_err_t ret = remove_from_blacklist(url);
            if (ret == ESP_OK)
            {
                ESP_LOGI(TAG, "%.*s(%d) removed from blacklist", url.length, url.string, url.length);
                httpd_resp_set_status(req, "200 OK");
                httpd_resp_set_type(req, "text/plain");
                httpd_resp_sendstr(req, "Success");
            }
            else if (ret == URL_ERR_NOT_FOUND)
            {
                ESP_LOGW(TAG, "%.*s(%d) not found in blacklist", url.length, url.string, url.length);
                httpd_resp_set_status(req, "500");
                httpd_resp_set_type(req, "text/plain");
                httpd_resp_sendstr(req, "Blacklist is full, try deleting entries");
            }
            else if (ret == URL_ERR_LIST_UNAVAILBLE)
            {
                ESP_LOGW(TAG, "Could not remove %.*s(%d), error opening blacklist.txt", url.length, url.string, url.length);
                httpd_resp_set_status(req, "500");
                httpd_resp_set_type(req, "text/plain");
                httpd_resp_sendstr(req, "Could not open blacklist file");
            }
        }
        else
        {
            ESP_LOGW(TAG, "Could not add %.*s(%d), invalid URL", url.length, url.string, url.length);
            httpd_resp_set_status(req, "400 Bad Request");
            httpd_resp_set_type(req, "text/plain");
            httpd_resp_sendstr(req, "Invalid URL");
        }
    }
    ESP_LOGD(TAG, "Blacklist delete processing Time: %lld ms", (esp_timer_get_time()-start)/1000);
    return ESP_OK;
}

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

static esp_err_t server_change_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Request to change upstream server");

    if (req->content_len > INET_ADDRSTRLEN)
    {
        ESP_LOGW(TAG, "Could not add, IP too long");
        httpd_resp_set_status(req, "400 Bad Request");
        httpd_resp_set_type(req, "text/plain");
        httpd_resp_sendstr(req, "IP too long");
    }
    else
    {
        char buf[req->content_len+1];
        buf[req->content_len] = '\0';
        if (httpd_req_recv(req, buf, req->content_len) <= 0) {
            return ESP_FAIL;
        }
        ESP_LOGI(TAG, "%s", buf);
        initialize_upstream_socket(buf);
        update_upstream_server(buf);
    }
    return ESP_OK;
}

static esp_err_t get_current_server_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Request for current upstream server");

    char upstream_server[IP4ADDR_STRLEN_MAX];
    get_upstream_server(upstream_server);

    httpd_resp_set_status(req, "200 OK");
    httpd_resp_set_type(req, "text/plain");
    httpd_resp_sendstr(req, upstream_server);

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

    if (ota_task_handle != NULL){
        eTaskState ota_state = eTaskGetState(ota_task_handle);

        ESP_LOGI(TAG, "Request for firmware update status (%d)", ota_state);
        httpd_resp_set_type(req, "text/plain; charset=UTF-8");
        if (ota_state == eSuspended)
        {
            delete_ota_task();
            httpd_resp_sendstr(req, "done");
        }
        else
        {
            httpd_resp_sendstr(req, "running...");
        }
    }

    return ESP_OK;
}

static esp_err_t restart_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Request for restart");
    esp_restart();
}

static esp_err_t blocking_status_handler(httpd_req_t *req){
    ESP_LOGI(TAG, "Request for blocking status");

    httpd_resp_set_type(req, "text/plain;");
    httpd_resp_set_status(req, "200 OK");

    bool blocking = blocking_on();
    if(blocking)
    {
        httpd_resp_sendstr(req, "blocking on");
    }
    else
    {
        httpd_resp_sendstr(req, "blocking off");
    }

    return ESP_OK;
    
}

static esp_err_t toggle_blocking_handler(httpd_req_t *req){
    ESP_LOGI(TAG, "Request to toggle blocking");

    toggle_blocking();
    httpd_resp_set_status(req, "200 OK");
    httpd_resp_sendstr(req, "Success");

    return ESP_OK;
}

static void setup_image_handlers(){
    // httpd_uri_t favicon = {
    //     .uri       = "/favicon.ico",
    //     .method    = HTTP_GET,
    //     .handler   = favicon_get_handler,
    //     .user_ctx  = ""
    // };
    // httpd_register_uri_handler(server, &favicon);

//     httpd_uri_t logo = {
//         .uri       = "/logo.png",
//         .method    = HTTP_GET,
//         .handler   = logo_get_handler,
//         .user_ctx  = ""
//     };
//     httpd_register_uri_handler(server, &logo);
}

static void setup_homepage_handlers(){
    httpd_uri_t root = {
        .uri       = "/",
        .method    = HTTP_GET,
        .handler   = homepage_get_handler,
        .user_ctx  = ""
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &root));

    httpd_uri_t homepage = {
        .uri       = "/homepage",
        .method    = HTTP_GET,
        .handler   = homepage_get_handler,
        .user_ctx  = ""
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &homepage));

    httpd_uri_t log_csv = {
        .uri       = "/log.csv",
        .method    = HTTP_GET,
        .handler   = log_csv_get_handler,
        .user_ctx  = ""
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &log_csv));
}

static void setup_blacklist_handlers(){
    httpd_uri_t blacklist = {
        .uri       = "/blacklist",
        .method    = HTTP_GET,
        .handler   = blacklist_get_handler,
        .user_ctx  = ""
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &blacklist));

    httpd_uri_t blacklist_add = {
        .uri       = "/blacklist/add",
        .method    = HTTP_POST,
        .handler   = blacklist_add_handler,
        .user_ctx  = ""
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &blacklist_add));

    httpd_uri_t blacklist_delete = {
        .uri       = "/blacklist/delete",
        .method    = HTTP_POST,
        .handler   = blacklist_delete_handler,
        .user_ctx  = ""
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &blacklist_delete));

    httpd_uri_t blacklist_txt = {
        .uri       = "/blacklist.txt",
        .method    = HTTP_GET,
        .handler   = blacklist_txt_get_handler,
        .user_ctx  = ""
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &blacklist_txt));
}

static void setup_settings_handlers(){
    httpd_uri_t settings = {
        .uri       = "/settings",
        .method    = HTTP_GET,
        .handler   = settings_get_handler,
        .user_ctx  = ""
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &settings));

    httpd_uri_t server_change = {
        .uri       = "/server",
        .method    = HTTP_POST,
        .handler   = server_change_handler,
        .user_ctx  = ""
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &server_change));

    httpd_uri_t get_server = {
        .uri       = "/currentserver",
        .method    = HTTP_GET,
        .handler   = get_current_server_handler,
        .user_ctx  = ""
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &get_server));

    httpd_uri_t update_firmware = {
        .uri       = "/updatefirmware",
        .method    = HTTP_POST,
        .handler   = updatefirmware_handler,
        .user_ctx  = ""
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &update_firmware));

    httpd_uri_t ota_status = {
        .uri       = "/updatestatus",
        .method    = HTTP_GET,
        .handler   = ota_status_get_handler,
        .user_ctx  = ""
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &ota_status));

    httpd_uri_t restart_esp = {
        .uri       = "/restart",
        .method    = HTTP_POST,
        .handler   = restart_handler,
        .user_ctx  = ""
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &restart_esp));

    httpd_uri_t get_blocking_status = {
        .uri       = "/blockingstatus",
        .method    = HTTP_GET,
        .handler   = blocking_status_handler,
        .user_ctx  = ""
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &get_blocking_status));

    httpd_uri_t set_blocking_status = {
        .uri       = "/blockingstatus",
        .method    = HTTP_POST,
        .handler   = toggle_blocking_handler,
        .user_ctx  = ""
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &set_blocking_status));
}

void start_application_webserver()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;
    config.max_uri_handlers = 30;
    config.stack_size = 8192;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    ESP_ERROR_CHECK(httpd_start(&server, &config));

    setup_image_handlers();
    setup_homepage_handlers();
    setup_blacklist_handlers();
    setup_settings_handlers();
    
    ESP_LOGI(TAG, "Ready to serve application page");
}

void stop_application_webserver()
{
    ESP_ERROR_CHECK(httpd_stop(server));
}
