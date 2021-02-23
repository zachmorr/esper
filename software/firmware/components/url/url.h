#ifndef URL_H
#define URL_H

#include <esp_system.h>

#define MAX_BLACKLIST_SIZE 50000
#define MAX_WHITELIST_SIZE 20000
#define MAX_URL_LENGTH 255

#define URL_ERR_TOO_LONG 0x200
#define URL_ERR_LIST_UNAVAILBLE 0x201
#define URL_ERR_LIST_FUll 0x202
#define URL_ERR_NOT_FOUND 0x203
#define URL_ERR_ALREADY_EXISTS 0x204
#define URL_ERR_INVALID_URL 0x205

typedef struct {
    uint8_t length;
    char string[MAX_URL_LENGTH];
} URL;

char* get_blacklist(int* size);
void return_blacklist();
esp_err_t add_to_blacklist(URL url);
esp_err_t remove_from_blacklist(URL url);

esp_err_t store_default_blacklists();
esp_err_t initialize_blocklists();

bool valid_url(URL url);
IRAM_ATTR bool in_blacklist(URL url);
IRAM_ATTR URL convert_qname_to_url(char* qname_ptr);

#endif