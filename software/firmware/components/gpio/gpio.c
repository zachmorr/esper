#include "gpio.h"
#include "error.h"
#include "events.h"
#include "flash.h"
#include "dns.h"
#include "error.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"
static const char *TAG = "GPIO";

#define PURPLE      148,0,211
#define BLUE        0,0,255
#define WEAK_BLUE   0,0,60
#define GREEN       0,255,0
#define RED         255,0,0
#define YELLOW      255,255,0

#define LED_SPEED_MODE LEDC_LOW_SPEED_MODE
#define MAX_DUTY_CYCLE 1023
#define FADE_TIME 1000

#define RED_LED_CHANNEL LEDC_CHANNEL_0
#define GREEN_LED_CHANNEL LEDC_CHANNEL_1
#define BLUE_LED_CHANNEL LEDC_CHANNEL_2

static bool gpio_enabled = true;
static int button = -1;
static int red_led = -1;
static int green_led = -1;
static int blue_led = -1;

static TaskHandle_t button_task_handle;
static TaskHandle_t led_task_handle;

static void button_state_change(void* arg)
{
    xTaskNotifyFromISR(button_task_handle, 0, eNoAction, NULL);
}

static void button_task(void* args)
{
    uint32_t press_timestamp = 0xFFFFFFFF;
    uint32_t length = 0;
    while(1)
    {
        xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
        ESP_LOGD(TAG, "Reset Pin intr, val: %d\n", gpio_get_level(button));
        if(gpio_get_level(button) == 0)
        {
            press_timestamp = esp_log_timestamp();
        }
        else
        {
            length = esp_log_timestamp() - press_timestamp;
            if( length > 5000 )
            {
                ESP_LOGW(TAG, "Resetting device");
                reset_device();
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
                toggle_bit(BLOCKING_BIT);
            }
        }
    }
}

static esp_err_t set_rgb(uint8_t red, uint8_t green, uint8_t blue)
{
    // ledc_set_fade_with_time(LED_SPEED_MODE, RED_LED_CHANNEL, red*4, FADE_TIME);
    // ledc_fade_start(LED_SPEED_MODE, RED_LED_CHANNEL, LEDC_FADE_NO_WAIT);
    ledc_set_duty(LED_SPEED_MODE, RED_LED_CHANNEL, red*4);
    ledc_update_duty(LED_SPEED_MODE, RED_LED_CHANNEL);

    // ledc_set_fade_with_time(LED_SPEED_MODE, GREEN_LED_CHANNEL, green*4, FADE_TIME);
    // ledc_fade_start(LED_SPEED_MODE, GREEN_LED_CHANNEL, LEDC_FADE_NO_WAIT);
    ledc_set_duty(LED_SPEED_MODE, GREEN_LED_CHANNEL, green*4);
    ledc_update_duty(LED_SPEED_MODE, GREEN_LED_CHANNEL);

    // ledc_set_fade_with_time(LED_SPEED_MODE, BLUE_LED_CHANNEL, blue*4, FADE_TIME);
    // ledc_fade_start(LED_SPEED_MODE, BLUE_LED_CHANNEL, LEDC_FADE_NO_WAIT);
    ledc_set_duty(LED_SPEED_MODE, BLUE_LED_CHANNEL, blue*4);
    ledc_update_duty(LED_SPEED_MODE, BLUE_LED_CHANNEL);

    return ESP_OK;
}

TaskHandle_t getLEDTaskHandle()
{
    return led_task_handle;
}

static void led_task(void* args)
{
    ESP_LOGI(TAG, "Listening for events");

    uint32_t state = 0;
    while(1)
    {
        xTaskNotifyWait(0, 0, &state, portMAX_DELAY);
        
        if( check_bit(ERROR_BIT) )
        {
            set_rgb(RED);
        }
        else if( check_bit(INITIALIZING_BIT) )
        {
            set_rgb(PURPLE);
        }
        else if( check_bit(PROVISIONING_BIT) )
        {
            set_rgb(GREEN);
        }
        else if( check_bit(BLOCKING_BIT) )
        {
            set_rgb(BLUE);
        }
        else {
            set_rgb(WEAK_BLUE);
        }
    }
}

esp_err_t initialize_gpio()
{
    ERROR_CHECK(get_gpio_config(&gpio_enabled, &button, &red_led, &green_led, &blue_led))

    if( !gpio_enabled )
        return ESP_OK;

    gpio_pad_select_gpio(button);
    gpio_set_direction(button, GPIO_MODE_DEF_INPUT);
    gpio_set_pull_mode(button, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(button, GPIO_INTR_ANYEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(button, button_state_change, NULL);

    ledc_timer_config_t led_timer_config = {
        .speed_mode = LED_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 5000,
        .duty_resolution = LEDC_TIMER_10_BIT,
    };
    ledc_timer_config(&led_timer_config);

    ledc_channel_config_t led_channel_config_blue = {
        .gpio_num = blue_led,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = BLUE_LED_CHANNEL,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
    };
    ledc_channel_config(&led_channel_config_blue);

    ledc_channel_config_t led_channel_config_red = {
        .gpio_num = red_led,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = RED_LED_CHANNEL,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
    };
    ledc_channel_config(&led_channel_config_red);

    ledc_channel_config_t led_channel_config_green = {
        .gpio_num = green_led,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = GREEN_LED_CHANNEL,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
    };
    ledc_channel_config(&led_channel_config_green);
    // ledc_fade_func_install(0);
    set_rgb(0,0,0);

    xTaskCreatePinnedToCore(button_task, "button_task", 3000, NULL, 2, &button_task_handle, tskNO_AFFINITY);
    xTaskCreatePinnedToCore(led_task, "led_task", 3000, NULL, 2, &led_task_handle, tskNO_AFFINITY);
    ESP_LOGI(TAG, "GPIO Initialized");
    return ESP_OK;
}
