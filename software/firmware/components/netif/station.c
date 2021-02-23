#include "station.h"
#include "error.h"
#include "flash.h"
#include "gpio.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "lwip/netdb.h"

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"
static const char *TAG = "STA";

static EventGroupHandle_t netif_event_group;
#define CONNECTED_BIT   BIT0
#define FAIL_BIT        BIT1
#define ETH_BIT         BIT2
#define WIFI_BIT        BIT3

esp_netif_t *eth_netif;
esp_eth_handle_t eth_handle;
esp_netif_t *wifi_netif;


static void sta_wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    switch (event_id) {
        case WIFI_EVENT_WIFI_READY:
            ESP_LOGI(TAG, "WIFI_EVENT_WIFI_READY");
            break;
        case WIFI_EVENT_STA_START:
            ESP_LOGI(TAG, "WIFI_EVENT_STA_START");
            esp_wifi_connect();
            break;
        case WIFI_EVENT_STA_STOP:
            ESP_LOGI(TAG, "WIFI_EVENT_STA_STOP");
            break;
        case WIFI_EVENT_STA_CONNECTED:
            ESP_LOGI(TAG, "WIFI_EVENT_STA_CONNECTED");
            xEventGroupSetBits(netif_event_group, WIFI_BIT);
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED %d", ((wifi_event_sta_disconnected_t*)event_data)->reason);
            xEventGroupClearBits(netif_event_group, WIFI_BIT);
            xEventGroupSetBits(netif_event_group, FAIL_BIT);
            esp_wifi_connect();
            break;
        case WIFI_EVENT_STA_AUTHMODE_CHANGE:
            ESP_LOGI(TAG, "WIFI_EVENT_STA_AUTHMODE_CHANGE");
            break;
        default:
            break;
    }
}

static void sta_eth_event_handler(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data)
{
    switch (event_id) {
        case ETHERNET_EVENT_CONNECTED:
            ESP_LOGI(TAG, "ETHERNET_EVENT_CONNECTED");
            xEventGroupSetBits(netif_event_group, ETH_BIT);
            esp_wifi_stop();
            break;
        case ETHERNET_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "ETHERNET_EVENT_DISCONNECTED");
            xEventGroupClearBits(netif_event_group, ETH_BIT);
            esp_wifi_start();
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

static void sta_ip_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    switch (event_id) {
        case IP_EVENT_ETH_GOT_IP:
            ESP_LOGI(TAG, "IP_EVENT_ETH_GOT_IP");
            xEventGroupSetBits(netif_event_group, CONNECTED_BIT);
            break;
        case IP_EVENT_STA_GOT_IP:
            ESP_LOGI(TAG, "IP_EVENT_STA_GOT_IP");
            xEventGroupSetBits(netif_event_group, CONNECTED_BIT);
            break;
        case IP_EVENT_STA_LOST_IP:
            ESP_LOGI(TAG, "IP_EVENT_STA_LOST_IP");
            break;
        default:
            break;
    }
}

static esp_err_t init_ethernet()
{
    ESP_LOGI(TAG, "Initializing Ethernet");
    esp_netif_config_t netif_cfg = ESP_NETIF_DEFAULT_ETH();
    eth_netif = esp_netif_new(&netif_cfg);
    if( !eth_netif )
        return ESP_FAIL;

    // Set default handlers to process TCP/IP stuffs
    esp_err_t err = esp_eth_set_default_handlers(eth_netif);
    if( err != ESP_OK )
        return err;

    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    phy_config.phy_addr = 0;
    phy_config.reset_gpio_num = -1;
    esp_eth_phy_t *phy = esp_eth_phy_new_lan8720(&phy_config);
    if ( !phy )
        return ESP_FAIL;

    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    mac_config.smi_mdc_gpio_num = 23;
    mac_config.smi_mdio_gpio_num = 18;
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&mac_config);
    if ( !mac )
        return ESP_FAIL;

    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);
    eth_handle = NULL;
    err = esp_eth_driver_install(&config, &eth_handle);
    err |= esp_netif_attach(eth_netif, esp_eth_new_netif_glue(eth_handle));
    if( err != ESP_OK )
        return ESP_FAIL;

    return ESP_OK;
}

static esp_err_t init_wifi()
{
    ESP_LOGI(TAG, "Initializing Wifi");

    esp_netif_config_t netif_cfg = ESP_NETIF_DEFAULT_WIFI_STA();
    wifi_netif = esp_netif_new(&netif_cfg);
    if( !wifi_netif )
        return ESP_FAIL;

    esp_netif_attach_wifi_station(wifi_netif);
    esp_wifi_set_default_wifi_sta_handlers();
    wifi_init_config_t init_cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_err_t err = esp_wifi_init(&init_cfg);
    if( err != ESP_OK )
        return ESP_FAIL;

    wifi_config_t wifi_config = {0};
#if CONFIG_PROVISION_DISABLE
    strcpy((char*)wifi_config.sta.ssid, CONFIG_SSID);
    strcpy((char*)wifi_config.sta.password, CONFIG_PASSWORD);
#else
    esp_wifi_get_config(ESP_IF_WIFI_STA, &wifi_config);
#endif

    ESP_LOGI(TAG, "SSID - %s", wifi_config.sta.ssid);
    ESP_LOGI(TAG, "PASS - %s", wifi_config.sta.password);

    err = esp_wifi_set_mode(WIFI_MODE_STA);
    err |= esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    if( err != ESP_OK )
        return ESP_FAIL;

    return ESP_OK;
}

esp_err_t set_static_ip(){
    // Give interfaces static IPs
    esp_netif_dhcpc_stop(eth_netif);
    esp_netif_dhcpc_stop(wifi_netif);

    esp_netif_ip_info_t ip_info;
    if( get_network_info(&ip_info) != ESP_OK ){
        return ESP_FAIL;
    }
        
    // char ip[IP4ADDR_STRLEN_MAX+1] = {0};

    // nvs_get("nm", (void*)ip, IP4ADDR_STRLEN_MAX);
    // inet_pton(AF_INET, ip, &ip_info.netmask);

    // nvs_get("gw", (void*)ip, IP4ADDR_STRLEN_MAX);
    // inet_pton(AF_INET, ip, &ip_info.gw);

    // nvs_get("ip", (void*)ip, IP4ADDR_STRLEN_MAX);
    // inet_pton(AF_INET, ip, &ip_info.ip);

    esp_netif_set_ip_info(eth_netif, &ip_info);
    esp_netif_set_ip_info(wifi_netif, &ip_info);

    // Set main DNS provider, used to connected to SNTP server
    esp_netif_dns_info_t dns = {0};
    char upstream_server[IP4ADDR_STRLEN_MAX];
    nvs_get("upstream_server", (void*)upstream_server, IP4ADDR_STRLEN_MAX);
    ip4addr_aton(upstream_server, (ip4_addr_t*)&dns.ip.u_addr.ip4);
    dns.ip.type = IPADDR_TYPE_V4;
    
    esp_err_t err = esp_netif_set_dns_info(eth_netif, ESP_NETIF_DNS_MAIN, &dns);
    err |= esp_netif_set_dns_info(wifi_netif, ESP_NETIF_DNS_MAIN, &dns);
    if( err != ESP_OK )
        return ESP_FAIL;

    return ESP_OK;
}

esp_err_t wifi_init_sta()
{
    ESP_LOGI(TAG, "Initializing Station");

    esp_err_t err;
    err = esp_netif_init();
    err |= esp_event_loop_create_default();
    err |= esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &sta_wifi_event_handler, NULL);
    err |= esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &sta_eth_event_handler, NULL);
    err |= esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &sta_ip_event_handler, NULL);
    if( err != ESP_OK )
        return ESP_FAIL;
    
    netif_event_group = xEventGroupCreate();
    err = init_ethernet();
    err |= init_wifi();
    ERROR_CHECK(set_static_ip());
    err |= esp_eth_start(eth_handle);
    //err |= esp_wifi_start();
    if( err != ESP_OK )
        return ESP_FAIL;

    // Wait for connection before returning
    EventBits_t bits = 0;
    while (!(bits & CONNECTED_BIT))
    {
        if (bits & FAIL_BIT) 
        {
            set_led_state(DISCONNECTED, SET);
        }
        bits = xEventGroupWaitBits(netif_event_group,
                CONNECTED_BIT | FAIL_BIT,
                pdTRUE, pdFALSE, portMAX_DELAY);
    }

    set_led_state(DISCONNECTED, CLEAR);
    return ESP_OK;
}