#include "datetime.h"

#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "lwip/err.h"
#include "lwip/apps/sntp.h"

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"
static const char *TAG = "TIME";

static char strftime_buf[64];

char* get_time_str(time_t time)
{
    struct tm timeinfo;
    localtime_r(&time, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%l:%M:%S%p", &timeinfo);
    return strftime_buf;
}

esp_err_t initialize_sntp()
{
    ESP_LOGI(TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();

    time_t now = 0;
    struct tm timeinfo = { 0 };
    time(&now);
    localtime_r(&now, &timeinfo);

    // wait for time to be set
    int retry = 0;
    const int retry_count = 10;
    while(timeinfo.tm_year < (2016 - 1900) && ++retry < retry_count) {
        ESP_LOGD(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
    }

    if(retry == retry_count)
    {
        ESP_LOGE(TAG, "Could Not Get Time");
        return ESP_FAIL;
    }
    else
    {
        setenv("TZ", "PST8PDT,M3.2.0/2,M11.1.0", 1);
        tzset();
        ESP_LOGI(TAG, "Current Time: %s", get_time_str(now));
        return ESP_OK;
    }
}