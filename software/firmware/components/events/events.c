#include "events.h"
#include "error.h"

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"
static const char *TAG = "EVENT";

EventGroupHandle_t event_group;
const int SCAN_FINISHED_BIT = BIT0;
const int CONNECTED_BIT = BIT1;
const int DISCONNECTED_BIT = BIT2;
const int PROVISIONING_BIT = BIT3;
const int PROVISIONED_BIT = BIT4;
const int STATIC_IP_BIT = BIT5;
const int BLOCKING_BIT = BIT6;

esp_err_t init_event_group()
{
    event_group = xEventGroupCreate();
    if( event_group == NULL )
        return ESP_FAIL;
    else
        return ESP_OK;
}

esp_err_t set_bit(int bit)
{
    xEventGroupSetBits(event_group, bit);
    return ESP_OK;
}

esp_err_t clear_bit(int bit)
{
    xEventGroupClearBits(event_group, bit);
    return ESP_OK;
}

bool check_bit(int bit)
{
    return (xEventGroupGetBits(event_group) & bit);
}

esp_err_t wait_for(int bit, TickType_t xTicksToWait)
{
    xEventGroupWaitBits(event_group, bit, pdFALSE, pdFALSE, xTicksToWait);
    return ESP_OK;
}

esp_err_t toggle_bit(int bit)
{
    if( check_bit(bit) )
    {
        ERROR_CHECK(clear_bit(bit))
    }
    else
    {
        ERROR_CHECK(set_bit(bit))
    }

    return ESP_OK;
}