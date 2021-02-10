#ifndef DNS_H
#define DNS_H

#include <esp_system.h>
#include "lwip/sockets.h"

#define DEFAULT_UPSTREAM_DNS "1.1.1.1"
#define DEFAULT_DEVICE_URL "esper.com"

#define DNS_PORT 53
#define MAX_PACKET_SIZE 512

#define A_RECORD_ANSWER_SIZE 16
#define AAAA_RECORD_ANSWER_SIZE 28

typedef struct{
    uint16_t id; // identification number
 
    uint8_t rd :1; // recursion desired
    uint8_t tc :1; // truncated message
    uint8_t aa :1; // authoritive answer
    uint8_t opcode :4; // purpose of message
    uint8_t qr :1; // query/response flag
 
    uint8_t rcode :4; // response code
    uint8_t cd :1; // checking disabled
    uint8_t ad :1; // authenticated data
    uint8_t z :1; // its z! reserved
    uint8_t ra :1; // recursion available
 
    uint16_t q_count; // number of question entries
    uint16_t ans_count; // number of answer entries
    uint16_t auth_count; // number of authority entriesS
    uint16_t add_count; // number of resource entries
} DNS_Header;

enum QRFlag {
    QUERY,
    ANSWER
};

enum RecordTypes {
    A=1,
    NS=2,
    AAAA=28,
};

typedef struct{
    DNS_Header* header;
    char* qname_ptr;
    uint16_t qtype;
    uint16_t qclass;
} DNS_Query;

typedef struct
{
    struct sockaddr_in src_address;
    uint8_t data[MAX_PACKET_SIZE];
    uint32_t length;
    int64_t recv_timestamp;
} Packet;

typedef struct 
{
    struct sockaddr_in src_address;
	uint16_t id;
    int64_t response_latency;
} Client;

esp_err_t initialize_upstream_socket();
void start_dns();
IRAM_ATTR uint8_t parse_query(Packet* packet, DNS_Query* query);
esp_err_t toggle_blocking();
IRAM_ATTR bool blocking_on();
esp_err_t set_device_url();

#endif