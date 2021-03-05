#ifndef GPIO_H
#define GPIO_H

#include <esp_system.h>

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