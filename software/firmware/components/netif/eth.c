#include "eth.h"
#include "error.h"
#include "esp_system.h"
#include "esp_eth.h"
#include "esp_netif.h"
#include "esp_event.h"

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"
static const char *TAG = "ETH";

esp_netif_t* eth_netif = NULL;
esp_eth_handle_t eth_handle = NULL;

static void eth_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id) {
        case ETHERNET_EVENT_CONNECTED:
            ESP_LOGI(TAG, "ETHERNET_EVENT_CONNECTED");
            break;
        case ETHERNET_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "ETHERNET_EVENT_DISCONNECTED");
            break;
        case ETHERNET_EVENT_START:
            ESP_LOGI(TAG, "ETHERNET_EVENT_START");
            break;
        case ETHERNET_EVENT_STOP:
            ESP_LOGI(TAG, " ETHERNET_EVENT_STOP");
            break;
        default:
            break;
    }
}

esp_err_t init_eth_netif(esp_netif_t** eth_netif)
{
    esp_netif_config_t netif_cfg =     {
        .base = ESP_NETIF_BASE_DEFAULT_ETH,      
        .driver = NULL,                          
        .stack = ESP_NETIF_NETSTACK_DEFAULT_ETH, 
    };

    *eth_netif = esp_netif_new(&netif_cfg);
    NULL_CHECK(eth_netif)

    // Set default handlers to process TCP/IP stuffs
    ERROR_CHECK(esp_eth_set_default_handlers(*eth_netif))

    return ESP_OK;
}

esp_err_t init_eth_handle(esp_eth_handle_t* eth_handle)
{
#ifdef CONFIG_ETHERNET_ENABLE
    eth_phy_config_t phy_config =     { 
        .phy_addr = CONFIG_ETH_PHY_ADDR, 
        .reset_timeout_ms = 100,         
        .autonego_timeout_ms = 4000, 
        .reset_gpio_num = CONFIG_ETH_PHY_RST_GPIO, 
    };


#ifdef CONFIG_ETH_PHY_LAN8720
    esp_eth_phy_t *phy = esp_eth_phy_new_lan8720(&phy_config);
#elif CONFIG_ETH_PHY_IP101
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config);
#elif CONFIG_ETH_PHY_RTL8201
    esp_eth_phy_t *phy = esp_eth_phy_new_rtl8201(&phy_config);
#elif CONFIG_ETH_PHY_DP83848
    esp_eth_phy_t *phy = esp_eth_phy_new_dp83848(&phy_config);
#endif
    NULL_CHECK(phy)

    eth_mac_config_t mac_config = {
        .sw_reset_timeout_ms = 100, 
        .rx_task_stack_size = 4096, 
        .rx_task_prio = 15,         
        .smi_mdc_gpio_num = CONFIG_ETH_MDC_GPIO,     
        .smi_mdio_gpio_num = CONFIG_ETH_MDIO_GPIO,    
        .flags = 0,               
    };

    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&mac_config);
    NULL_CHECK(mac)

    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);
    ERROR_CHECK(esp_eth_driver_install(&config, eth_handle))
#endif

    return ESP_OK;
}

esp_err_t init_eth()
{
    ESP_LOGI(TAG, "Initializing Ethernet...");
    ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL))
    // ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &ip_event_handler, NULL))

    return ESP_OK;
}