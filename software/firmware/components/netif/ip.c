#include "ip.h"
#include "events.h"
#include "error.h"
#include "eth.h"
#include "wifi.h"
#include "string.h"
#include "flash.h"
#include "lwip/sockets.h"

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"
static const char *TAG = "IP";

static bool eth_en = false;
static bool wifi_en = false;

static esp_netif_t* wifi_sta_netif = NULL;
static esp_netif_t* wifi_ap_netif = NULL;
static esp_netif_t* eth_netif = NULL;
static esp_eth_handle_t eth_handle = NULL;


static void ip_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    switch (event_id) {
        case IP_EVENT_ETH_GOT_IP:
            ESP_LOGI(TAG, "IP_EVENT_ETH_GOT_IP");
            set_bit(CONNECTED_BIT);

            // Static IP may not be assigned yet if wifi is disabled, so assign it here
            if( !check_bit(STATIC_IP_BIT) )
            {
                ESP_LOGI(TAG, "Saving IP");
                set_network_info(((ip_event_got_ip_t*)event_data)->ip_info);
            }

            break;
        case IP_EVENT_STA_GOT_IP:
            ESP_LOGI(TAG, "IP_EVENT_STA_GOT_IP");
            set_bit(CONNECTED_BIT);

            if( check_bit(PROVISIONING_BIT) )
            {
                ESP_LOGI(TAG, "Saving IP");
                set_network_info(((ip_event_got_ip_t*)event_data)->ip_info);
            }

            break;
        case IP_EVENT_STA_LOST_IP:
            ESP_LOGI(TAG, "IP_EVENT_STA_LOST_IP");
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

    ERROR_CHECK(get_enabled_interfaces(&eth_en, &wifi_en))
    if( eth_en )
    {
        ERROR_CHECK(init_eth())
        ERROR_CHECK(init_eth_netif(&eth_netif))
        ERROR_CHECK(init_eth_handle(&eth_handle))
        ERROR_CHECK(esp_netif_attach(eth_netif, esp_eth_new_netif_glue(eth_handle)))
    }

    if( wifi_en )
    {
        ERROR_CHECK(init_wifi())
        ERROR_CHECK(init_wifi_sta_netif(&wifi_sta_netif))
        ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA))
        ERROR_CHECK(esp_wifi_start())

        // // Check provisioning status
        // wifi_config_t config = {0};
        // ERROR_CHECK(esp_wifi_get_config(ESP_IF_WIFI_STA, &config))
        bool provisioned = false;
        ERROR_CHECK(get_provisioning_status(&provisioned))
        if( provisioned )
            ERROR_CHECK(set_bit(PROVISIONED_BIT))
        else
            ERROR_CHECK(clear_bit(PROVISIONED_BIT))
    }
    else
    {
        ERROR_CHECK(set_bit(PROVISIONED_BIT))
    }

    return ESP_OK;
}

esp_err_t turn_on_accesspoint()
{
    ESP_LOGI(TAG, "Turning Accesspoint on");
    ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA))
    ERROR_CHECK(init_wifi_ap_netif(&wifi_ap_netif))

    return ESP_OK;
}

esp_err_t turn_off_accesspoint()
{
    ESP_LOGI(TAG, "Turning Accesspoint off");
    esp_netif_destroy(wifi_ap_netif);
    return ESP_OK;
}

esp_err_t set_static_ip(esp_netif_t* interface)
{
    
    esp_netif_ip_info_t ip_info;
    ERROR_CHECK(get_network_info(&ip_info))

    // Static IP may not be assigned yet if wifi is disabled, so check that it exists first
    if( ip_info.ip.addr )
    {
        esp_netif_dhcpc_stop(interface);
        esp_netif_set_ip_info(interface, &ip_info);
        ERROR_CHECK(set_bit(STATIC_IP_BIT))
        ESP_LOGI(TAG, "Assigned static IP to interface");
    }

    // Set main DNS provider, used to connected to SNTP server
    esp_netif_dns_info_t dns = {0};
    char upstream_server[IP4ADDR_STRLEN_MAX];
    ERROR_CHECK(get_upstream_dns(upstream_server))
    ip4addr_aton(upstream_server, (ip4_addr_t*)&dns.ip.u_addr.ip4);
    dns.ip.type = IPADDR_TYPE_V4;
    
    ERROR_CHECK(esp_netif_set_dns_info(interface, ESP_NETIF_DNS_MAIN, &dns))

    return ESP_OK;
}

esp_err_t start_interfaces()
{
    ESP_LOGI(TAG, "Starting interfaces");

    if( eth_en )
    {
        ERROR_CHECK(esp_eth_start(eth_handle))
        ERROR_CHECK(set_static_ip(eth_netif))
    }

    if( wifi_en )
    {
        ERROR_CHECK(set_static_ip(wifi_sta_netif))
        // ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA))
        ERROR_CHECK(esp_wifi_connect())
    }

    return ESP_OK;
}