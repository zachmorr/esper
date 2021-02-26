#include "ip.h"
#include "error.h"
#include "eth.h"
#include "wifi.h"
#include "string.h"
#include "flash.h"
#include "lwip/ip4.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"
static const char *TAG = "IP";

EventGroupHandle_t ip_event_group;
const int SCAN_FINISHED_BIT = BIT0;
const int DISCONNECTED_BIT = BIT1;
const int CONNECTED_BIT = BIT2;
const int PROVISIONED_BIT = BIT2;
const int STATIC_IP_BIT = BIT2;

static esp_netif_t* wifi_sta_netif = NULL;
static esp_netif_t* wifi_ap_netif = NULL;
static esp_netif_t* eth_netif = NULL;
static esp_eth_handle_t eth_handle = NULL;


static void ip_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    switch (event_id) {
        case IP_EVENT_ETH_GOT_IP:
            ESP_LOGI(TAG, "IP_EVENT_ETH_GOT_IP");
            xEventGroupSetBits(ip_event_group, CONNECTED_BIT);
            break;
        case IP_EVENT_STA_GOT_IP:
            ESP_LOGI(TAG, "IP_EVENT_STA_GOT_IP");
            xEventGroupSetBits(ip_event_group, CONNECTED_BIT);
            break;
        case IP_EVENT_STA_LOST_IP:
            ESP_LOGI(TAG, "IP_EVENT_STA_LOST_IP");
            xEventGroupSetBits(ip_event_group, DISCONNECTED_BIT);
            break;
        case IP_EVENT_AP_STAIPASSIGNED:
            ESP_LOGI(TAG, "IP_EVENT_AP_STAIPASSIGNED");
            break;
        default:
            break;
    }
}

esp_err_t initialize_interfaces()
{
    ESP_LOGI(TAG, "Initializing Interfaces");
    esp_netif_init();
    esp_event_loop_create_default();
    ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &ip_event_handler, NULL))
    ip_event_group = xEventGroupCreate();

#ifdef CONFIG_ETHERNET_ENABLE
    ERROR_CHECK(init_eth())
    ERROR_CHECK(init_eth_netif(&eth_netif))
    ERROR_CHECK(init_eth_handle(&eth_handle))
    ERROR_CHECK(esp_netif_attach(eth_netif, esp_eth_new_netif_glue(eth_handle)))
    // ERROR_CHECK(esp_eth_start(eth_handle))
#endif

#ifdef CONFIG_WIFI_ENABLE
    ERROR_CHECK(init_wifi())
    ERROR_CHECK(init_wifi_sta_netif(&wifi_sta_netif))
    // ERROR_CHECK(esp_wifi_start())

    wifi_config_t config = {0};
    ERROR_CHECK(esp_wifi_get_config(ESP_IF_WIFI_STA, &config))

    if( strlen((const char*)config.ap.ssid) == 0 )
        xEventGroupClearBits(ip_event_group, PROVISIONED_BIT);
    else
        xEventGroupSetBits(ip_event_group, PROVISIONED_BIT);
#else
    xEventGroupClearBits(ip_event_group, PROVISIONED_BIT);
#endif

    return ESP_OK;
}

esp_err_t turn_on_accesspoint()
{
    ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA))
    ERROR_CHECK(init_wifi_ap_netif(&wifi_ap_netif))
    ERROR_CHECK(esp_wifi_start())

    return ESP_OK;
}

esp_err_t turn_off_accesspoint()
{
    esp_netif_destroy(wifi_ap_netif);
    return ESP_OK;
}

esp_err_t set_static_ip(esp_netif_t* interface)
{
    // Give interfaces static IPs
    esp_netif_ip_info_t ip_info;
    ERROR_CHECK(get_network_info(&ip_info))

    // if interface is empty turn on DHCP to get an IP
    if( ip_info.ip.addr )
    {
        esp_netif_dhcpc_stop(interface);
        esp_netif_set_ip_info(interface, &ip_info);
    }

    // Set main DNS provider, used to connected to SNTP server
    esp_netif_dns_info_t dns = {0};
    char upstream_server[IP4ADDR_STRLEN_MAX];
    nvs_get("upstream_server", (void*)upstream_server, IP4ADDR_STRLEN_MAX);
    ip4addr_aton(upstream_server, (ip4_addr_t*)&dns.ip.u_addr.ip4);
    dns.ip.type = IPADDR_TYPE_V4;
    
    ERROR_CHECK(esp_netif_set_dns_info(eth_netif, ESP_NETIF_DNS_MAIN, &dns))

    return ESP_OK;
}

esp_err_t start_interfaces()
{
#ifdef CONFIG_ETHERNET_ENABLE
    ERROR_CHECK(esp_eth_start(eth_handle))
#endif

#ifdef CONFIG_WIFI_ENABLE
    ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA))
    ERROR_CHECK(esp_wifi_start())
#endif

    return ESP_OK;
}

bool provision_status()
{
    if( xEventGroupGetBits(ip_event_group) & PROVISIONED_BIT )
        return true;
    else
        return false;
}