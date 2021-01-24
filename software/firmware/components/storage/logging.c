#include "logging.h"
#include "storage.h"

#include "sys/unistd.h"
#include "unistd.h"
#include "sys/stat.h"
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"
static const char *TAG = "LOGGING";

#define LOG_QUEUE_SIZE 50
static QueueHandle_t log_queue;
static SemaphoreHandle_t log_mutex;
static TaskHandle_t logging;

// No need for a tail since it will always be either the same as log_head or the EOF
static uint16_t log_head;
static bool full_flag;

void get_log_head(uint16_t* head, bool* flag)
{
    xSemaphoreTake(log_mutex, portMAX_DELAY);
    *head = log_head;
    *flag = full_flag;
    xSemaphoreGive(log_mutex);
}

esp_err_t log_query(URL url, bool blocked, uint32_t client)
{
    Log_Entry entry = {0};
    time(&entry.time);
    entry.url = url;
    entry.blocked = blocked;
    entry.client = client;

    BaseType_t err = xQueueSend(log_queue, &entry, 0);
    if(err == errQUEUE_FULL)
    {
        ESP_LOGW(TAG, "Log queue Full, could not add entry");
    }
    else
    {
        xTaskNotify(logging, 1, eSetValueWithoutOverwrite);
        ESP_LOGV(TAG, "Entry Added to Queue");
    }

    return ESP_OK;
}

static void logging_task(void* args)
{
    get_log_values(&log_head, &full_flag);
    ESP_LOGI(TAG, "Logging Task Started");
    while(1)
    {
        xTaskNotifyWait(0, 0xFFFFFFFF, NULL, portMAX_DELAY);
        long start = esp_timer_get_time();

        // get all entries waiting to be logged
        uint8_t entries_in_queue = uxQueueMessagesWaiting(log_queue);
        ESP_LOGI(TAG, "Adding %d entries to log", entries_in_queue);

        FILE* log = fopen("/spiffs/log", "r+");
        if(log)
        {
            for(int i = 0; i < entries_in_queue; i++)
            {
                Log_Entry entry = {0};
                esp_err_t err = xQueueReceive(log_queue, &entry, portMAX_DELAY);
                if(err == pdFALSE)
                {
                    ESP_LOGW(TAG, "Error reading entry %d from log_queue", i);
                }
                else
                {
                    // Entries are retrieved from queue FIFO (oldest first) so log_head moves backwards to keep newest entries at the front
                    xSemaphoreTake(log_mutex, portMAX_DELAY);
                    if(log_head <= 0)  // Reset head position if it is at the end
                    {
                        log_head = MAX_LOGS;
                        full_flag = true;
                    }

                    log_head--;
                    ESP_LOGV(TAG, "Adding %*s to log address %d(%d)", entry.url.length, entry.url.string, log_head, log_head*sizeof(Log_Entry));

                    if(fseek(log, log_head*sizeof(Log_Entry), SEEK_SET))
                        ESP_LOGW(TAG, "Could not fseek to %d(%d)", log_head, log_head*sizeof(Log_Entry));

                    if(fwrite(&entry, sizeof(Log_Entry), 1, log))
                        update_log_values(log_head, full_flag);
                    else
                        ESP_LOGW(TAG, "Error adding %*s to log address %d", entry.url.length, entry.url.string, log_head);

                    xSemaphoreGive(log_mutex);
                }
            }
            fclose(log);
        }
        else
        {
            ESP_LOGW(TAG, "Error opening log file");
        }
        ESP_LOGI(TAG, "Time to add log entries: %.2lf ms", (esp_timer_get_time()-start)/1000.0);
    }
}

esp_err_t create_log_file()
{
    ESP_LOGI(TAG, "Creating log file");
    FILE* log = fopen("/spiffs/log", "w");
    if (log)
    {
        // ftruncate not implemented yet
        ESP_LOGD(TAG, "Expanding log file to %d bytes", MAX_LOGS*sizeof(Log_Entry));
        Log_Entry entry = {0};
        for(int i = 0; i < MAX_LOGS; i++)
        {
            fwrite(&entry, sizeof(Log_Entry), 1, log);
        }
        ESP_LOGD(TAG, "Log file %ld bytes large", ftell(log));
        fclose(log);
        update_log_values(MAX_LOGS, false);
        return ESP_OK;
    }
    else
    {
        return LOG_ERR_LOG_UNAVAILABLE;
    }
}

esp_err_t initialize_logging()
{
    log_mutex = xSemaphoreCreateMutex();
    log_queue = xQueueCreate(LOG_QUEUE_SIZE, sizeof(Log_Entry));

    xTaskCreatePinnedToCore(logging_task, "logging_task", 
                            10000, NULL, 2, &logging, 0);

    return ESP_OK;
}