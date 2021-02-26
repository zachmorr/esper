#ifndef ERROR_H
#define ERROR_H

#define STRING(x) #x

#define NULL_CHECK(x) if( x == NULL ){ return ESP_FAIL; }

#define ERROR_CHECK(x) do {                                                                 \
            esp_err_t tmp_err = (x);                                                        \
            if (tmp_err != ESP_OK) {                                                        \
                ESP_LOGE(TAG, STRING(x) " (%s)(%X)", esp_err_to_name(tmp_err), tmp_err);    \
                return ESP_FAIL;                                                            \
            }                                                                               \
        } while(0);


#define WIFI_ERR_BASE               0x200
#define WIFI_ERR_MODE_NULL          (WIFI_ERR_BASE + 1)
#define WIFI_ERR_NULL_NETIF         (WIFI_ERR_BASE + 2)
#define WIFI_ERR_NOT_CONFIGURED     (WIFI_ERR_BASE + 3)

#define URL_ERR_BASE                0x300
#define URL_ERR_TOO_LONG            (URL_ERR_BASE + 1)
#define URL_ERR_LIST_UNAVAILBLE     (URL_ERR_BASE + 2)
#define URL_ERR_LIST_FUll           (URL_ERR_BASE + 3)
#define URL_ERR_NOT_FOUND           (URL_ERR_BASE + 4)
#define URL_ERR_ALREADY_EXISTS      (URL_ERR_BASE + 5)
#define URL_ERR_INVALID_URL         (URL_ERR_BASE + 6)

#define LOG_ERR_BASE                0x400
#define LOG_ERR_LOG_UNAVAILABLE     (LOG_ERR_BASE + 1)

#define IO_ERR_BASE                 0x500
#define IO_ERR_BUTTON_INIT          (IO_ERR_BASE + 1)
#define IO_ERR_LED_INIT             (IO_ERR_BASE + 2)

#define DNS_ERR_BASE                 0x600
#define DNS_ERR_SOCKET_INIT          (DNS_ERR_BASE + 1)
#define DNS_ERR_INVALID_QNAME          (DNS_ERR_BASE + 2)

#endif