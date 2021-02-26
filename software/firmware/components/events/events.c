#include "events.h"

EventGroupHandle_t event_group;
const int SCAN_FINISHED_BIT = BIT0;
const int CONNECTED_BIT = BIT1;
const int DISCONNECTED_BIT = BIT2;
const int PROVISIONING_BIT = BIT3;
const int PROVISIONED_BIT = BIT4;
const int STATIC_IP_BIT = BIT5;

esp_err_t init_event_group()
{
    event_group = xEventGroupCreate();
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