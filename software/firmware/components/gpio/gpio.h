#ifndef GPIO_H
#define GPIO_H

#include <esp_system.h>

#define IO_ERR_BUTTON_INIT 0x300
#define IO_ERR_LED_INIT 0x301

enum state {
    STARTUP,
    CONFIGURING,
    BLOCKING,
    OTA,
    ERROR,
    DISCONNECTED
};

enum toggle {
    CLEAR,
    SET
};

esp_err_t set_led_state(int state, bool toggle);
esp_err_t initialize_gpio();
#endif