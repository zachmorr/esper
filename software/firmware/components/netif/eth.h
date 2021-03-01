#ifndef ETH_H
#define ETH_H

#include "esp_system.h"
#include "esp_eth.h"
#include "esp_netif.h"

enum Phy {
    NONE,
    LAN8720,
    IP101,
    RTL8201,
    DP83848
};

esp_err_t init_eth_netif(esp_netif_t** eth_netif);
esp_err_t init_eth_handle(esp_eth_handle_t* eth_handle);
esp_err_t init_eth();

#endif