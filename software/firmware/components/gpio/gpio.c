#include "gpio.h"
#include "storage.h"
#include "dns.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"
static const char *TAG = "GPIO";

#define CHECK_BIT(state, bit) (state & 1UL<<bit)

#define BLUE_LED_NUM GPIO_NUM_16
#define BLUE_LED_CHANNEL LEDC_CHANNEL_0

#define RED_LED_NUM GPIO_NUM_32
#define RED_LED_CHANNEL LEDC_CHANNEL_1

#define GREEN_LED_NUM GPIO_NUM_33
#define GREEN_LED_CHANNEL LEDC_CHANNEL_2

#define LED_SPEED_MODE LEDC_LOW_SPEED_MODE
#define MAX_DUTY_CYCLE 1023
#define FADE_TIME 1000

#define BUTTON_PIN 4

static TaskHandle_t reset;
static uint32_t current_state = 0;

static void button_state_change(void* arg)
{
    xTaskNotifyFromISR(reset, 0, eNoAction, NULL);
}

static void button_task(void* args)
{
    uint32_t press_timestamp = 0xFFFFFFFF;
    uint32_t length = 0;
    while(1)
    {
        xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
        ESP_LOGD(TAG, "Reset Pin intr, val: %d\n", gpio_get_level(BUTTON_PIN));
        if(gpio_get_level(BUTTON_PIN) == 0)
        {
            press_timestamp = esp_log_timestamp();
        }
        else
        {
            length = esp_log_timestamp() - press_timestamp;
            if( length > 5000 )
            {
                ESP_LOGW(TAG, "Resetting device");
                ESP_ERROR_CHECK(reset_device());
                esp_restart();
            }
            // else if (length > 200 )
            // {
            //     char buf[500];
            //     //vTaskDelay(5000/ portTICK_PERIOD_MS);
            //     ESP_LOGI("STATS", "Run Time Stats");
            //     vTaskGetRunTimeStats(buf);
            //     ESP_LOGI("STATS", "\n%s", buf);
            //     ESP_LOGI("STATS", "\n\nFree Heap: %d", xPortGetFreeHeapSize());
            // }
            else
            {
                toggle_blocking();
            }
        }
    }
}

esp_err_t initialize_gpio()
{
    esp_err_t err;
    gpio_pad_select_gpio(BUTTON_PIN);
    err = gpio_set_direction(BUTTON_PIN, GPIO_MODE_DEF_INPUT);
    err |= gpio_set_pull_mode(BUTTON_PIN, GPIO_PULLUP_ONLY);
    err |= gpio_set_intr_type(BUTTON_PIN, GPIO_INTR_ANYEDGE);
    err |= gpio_install_isr_service(0);
    err |= gpio_isr_handler_add(BUTTON_PIN, button_state_change, NULL);

    if (err != ESP_OK){
        return IO_ERR_BUTTON_INIT;
    }

    ledc_timer_config_t led_timer_config = {
        .speed_mode = LED_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 5000,
        .duty_resolution = LEDC_TIMER_10_BIT,
    };
    err |= ledc_timer_config(&led_timer_config);

    ledc_channel_config_t led_channel_config_blue = {
        .gpio_num = BLUE_LED_NUM,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = BLUE_LED_CHANNEL,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
    };
    err |= ledc_channel_config(&led_channel_config_blue);

    ledc_channel_config_t led_channel_config_red = {
        .gpio_num = RED_LED_NUM,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = RED_LED_CHANNEL,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
    };
    err |= ledc_channel_config(&led_channel_config_red);

    ledc_channel_config_t led_channel_config_green = {
        .gpio_num = GREEN_LED_NUM,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = GREEN_LED_CHANNEL,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
    };
    err |= ledc_channel_config(&led_channel_config_green);
    err |= ledc_fade_func_install(0);

    if (err != ESP_OK){
        return IO_ERR_LED_INIT;
    }

    xTaskCreatePinnedToCore(button_task, "button_task", 3000, NULL, 2, &reset, tskNO_AFFINITY);
    ESP_LOGI(TAG, "GPIO Initialized");
    return ESP_OK;
}

static esp_err_t set_rgb(uint8_t red, uint8_t green, uint8_t blue)
{
    ledc_set_fade_with_time(LED_SPEED_MODE, RED_LED_CHANNEL, red*4, FADE_TIME);
    ledc_fade_start(LED_SPEED_MODE, RED_LED_CHANNEL, LEDC_FADE_NO_WAIT);

    ledc_set_fade_with_time(LED_SPEED_MODE, GREEN_LED_CHANNEL, green*4, FADE_TIME);
    ledc_fade_start(LED_SPEED_MODE, GREEN_LED_CHANNEL, LEDC_FADE_NO_WAIT);

    ledc_set_fade_with_time(LED_SPEED_MODE, BLUE_LED_CHANNEL, blue*4, FADE_TIME);
    ledc_fade_start(LED_SPEED_MODE, BLUE_LED_CHANNEL, LEDC_FADE_NO_WAIT);

    return ESP_OK;
}

esp_err_t set_led_state(int state, bool toggle)
{
    if(toggle == SET)
    {
        current_state |= 1UL << state;
    }
    else if(toggle == CLEAR)
    {
        current_state &= ~(1UL << state);
    }
    ESP_LOGD(TAG, "Current LED State: %d", current_state);

    if (CHECK_BIT(current_state, STARTUP))
    {
        ESP_LOGV(TAG, "Setting LED to orange");
        set_rgb(255, 165, 0);
    }
    else
    {
        if (CHECK_BIT(current_state, CONFIGURING))
        {
            ESP_LOGV(TAG, "Setting LED to green");
            set_rgb(0, 255, 0);
        }
        else
        {
            if (CHECK_BIT(current_state, OTA))
            {
                ESP_LOGV(TAG, "Setting LED to fade yellow");
                set_rgb(255, 255, 0);
            }
            else
            {
                if (CHECK_BIT(current_state, BLOCKING))
                {
                    ESP_LOGV(TAG, "Setting LED to blue");
                    set_rgb(0, 0, 255);
                }
                else
                {
                    ESP_LOGV(TAG, "Setting LED to weak blue");
                    set_rgb(0, 0, 60);
                }
            }
        }
    }
    return ESP_OK;
}