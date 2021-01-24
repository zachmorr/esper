#include "dns.h"
#include "storage.h"
#include "url.h"
#include "logging.h"
#include "datetime.h"
#include "gpio.h"

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "esp_timer.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "lwip/ip_addr.h"

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"
static const char *TAG = "DNS";

#define PACKET_QUEUE_SIZE 10
#define CLIENT_QUEUE_SIZE 50

static int sock;
    // add mutex too dns_server_addr
static struct sockaddr_in dns_server_addr;
static socklen_t addrlen = sizeof(struct sockaddr_in);

static bool blocking_status = true;
static SemaphoreHandle_t blocking_mutex;

static TaskHandle_t dns;
static TaskHandle_t listening;

static QueueHandle_t packet_queue;
static Client client_queue[CLIENT_QUEUE_SIZE];


esp_err_t initialize_upstream_socket(char* ip)
{
    dns_server_addr.sin_family = PF_INET;
    dns_server_addr.sin_port = htons(DNS_PORT);
    ip4addr_aton(ip, (ip4_addr_t *)&dns_server_addr.sin_addr.s_addr);
    ESP_LOGI(TAG, "DNS Server Address: %s", inet_ntoa(dns_server_addr.sin_addr.s_addr));

    return ESP_OK;
}

static void initialize_socket()
{
    // Setup recv_sock
    ESP_LOGV(TAG, "Initializing Socket");
	if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		ESP_LOGW(TAG, "Failed To Create Socket");
		esp_restart(); // ¯\_(ツ)_/¯ 
	}

    struct sockaddr_in my_addr;
	my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(DNS_PORT);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sock, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0)
    {
    	ESP_LOGW(TAG, "Failed To Bind Socket");
    	esp_restart(); // ¯\_(ツ)_/¯ 
    }
    ESP_LOGD(TAG, "Socket %d created and bound on port %d", sock, DNS_PORT);

    char upstream_server[IP4ADDR_STRLEN_MAX];
    get_upstream_server(upstream_server);
    initialize_upstream_socket(upstream_server);
}

static IRAM_ATTR void listening_task(void* paramerters)
{
    ESP_LOGI(TAG, "Listening...");
    while(1)
    {
        //uint8_t buf[MAX_PACKET_SIZE] = {0};
        Packet packet = {0};
        uint32_t recvlen = recvfrom(sock, packet.data, MAX_PACKET_SIZE, 0, (struct sockaddr *)&packet.src_address, &addrlen);
        if(recvlen > 0)
        {
            ESP_LOGD(TAG, "Received %d Byte Packet from %s", recvlen, inet_ntoa(packet.src_address.sin_addr.s_addr));
            packet.length = recvlen;
            packet.recv_timestamp = esp_timer_get_time();

            BaseType_t err = xQueueSend(packet_queue, &packet, 0);
            if(err == errQUEUE_FULL)
            {
                ESP_LOGW(TAG, "Queue Full, could not add packet");
            }
            else
            {
                ESP_LOGV(TAG, "Packet Added to Queue");
            }
        } 
        else
        {
            // handle errors here
            ESP_LOGW(TAG, "Error Receiving Packet");
        }
    }
}

static IRAM_ATTR esp_err_t block_query(DNS_Header* header, Packet* packet, uint16_t qtype)
{   
    // DNS Answer (0.0.0.0) to send for blacklisted ip4
    const char ip4_response[] = "\xc0\x0c\x00\x01\x00\x01\x00\x00\x00\x02\x00\x04\x00\x00\x00\x00";
    // DNS Answer (::) to send for blacklisted ip6
    const char ip6_response[] = "\xc0\x0c\x00\x1c\x00\x01\x00\x00\x00\x02\x00\x10\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

    header->qr = 1; // change packet to answer
    header->aa = 1; // respect my authoritah
    header->ans_count = htons(1);

    size_t len = 0;
    uint8_t answer_packet[8192];
    memcpy(answer_packet, packet->data, packet->length);
    if(qtype == A)
    {
        memcpy(answer_packet+packet->length, ip4_response, A_RECORD_ANSWER_SIZE);
        len = packet->length+A_RECORD_ANSWER_SIZE;
        ESP_LOGD(TAG, "Sending fake A record to %s",inet_ntoa(packet->src_address.sin_addr.s_addr));
    }
    else if(qtype == AAAA)
    {
        memcpy(answer_packet+packet->length, ip6_response, AAAA_RECORD_ANSWER_SIZE);
        len = packet->length+AAAA_RECORD_ANSWER_SIZE;
        ESP_LOGD(TAG, "Sending fake AAAA record to %s",inet_ntoa(packet->src_address.sin_addr.s_addr));
    }

    int sendlen = sendto(sock, answer_packet, len, 0, 
                        (struct sockaddr *)&packet->src_address, addrlen);
    if(sendlen <= 0)
    {
        ESP_LOGW(TAG, "Failed sending fake dns response");
    }
    else
    {
        ESP_LOGD(TAG, "Sent");
    }
    return ESP_OK;
}

static IRAM_ATTR esp_err_t redirect_response(DNS_Header* header, Packet* packet, uint16_t qtype)
{
    // DNS Answer (empty ip section) for redirecting esperdns.app
    const char ip4_redirect_response[] = "\xc0\x0c\x00\x01\x00\x01\x00\x00\x00\x02\x00\x04";

    size_t len = 0;
    uint8_t answer_packet[8192];
    if(qtype == A)
    {
        header->qr = 1; // change packet to answer
        header->aa = 1; // respect my authoritah
        header->ans_count = htons(1);

        memcpy(answer_packet, packet->data, packet->length);
        memcpy(answer_packet+packet->length, ip4_redirect_response, A_RECORD_ANSWER_SIZE-4);

        struct in_addr ip;
        char ip_str[IP4ADDR_STRLEN_MAX];
        inet_pton(AF_INET, get_netmask(ip_str), &ip);
        memcpy(answer_packet+packet->length+sizeof(ip4_redirect_response)-1, &ip.s_addr, 4);

        len = packet->length+A_RECORD_ANSWER_SIZE;
        ESP_LOGD(TAG, "Sending fake A record for esperdns.app");
    }
    else if(qtype == AAAA)
    {
        header->qr = 1; // change packet to answer
        header->rd = 1; // recursion desired
        header->ra = 1; // recursion available
        header->rcode = 3; // No such name

        memcpy(answer_packet, packet->data, packet->length);
        len = packet->length;
        ESP_LOGD(TAG, "Sending fake AAAA record for esperdns.app");
    }

    int sendlen = sendto(sock, answer_packet, len, 0, 
                        (struct sockaddr *)&packet->src_address, addrlen);
    if(sendlen <= 0)
    {
        ESP_LOGW(TAG, "Failed sending fake dns response");
    }
    else
    {
        ESP_LOGD(TAG, "Sent");
    }
    return ESP_OK;
}

static IRAM_ATTR esp_err_t forward_query(DNS_Header* header, Packet* packet)
{
    ESP_LOGD(TAG, "Forwarding to %s",  inet_ntoa(dns_server_addr.sin_addr.s_addr));

    int sendlen = sendto(sock, packet->data, packet->length, 0, (struct sockaddr *)&dns_server_addr, addrlen);
    if(sendlen <= 0)
    {
        ESP_LOGW(TAG, "Failed to foward request");
    }
    else
    {
        ESP_LOGD(TAG, "Sent %d bytes", sendlen);
        
        for(int i = CLIENT_QUEUE_SIZE-1; i >= 1; i--) 
        {
            client_queue[i] = client_queue[i-1];
        }
        client_queue[0].src_address = packet->src_address;
        client_queue[0].id = header->id;
        client_queue[0].response_latency = esp_timer_get_time();
    }
    return ESP_OK;
}

static IRAM_ATTR esp_err_t forward_answer(DNS_Header* header, Packet* packet, URL* url)
{
    for(int i = 0; i < CLIENT_QUEUE_SIZE; i++)
    {
        if(header->id == client_queue[i].id)
        {
            ESP_LOGD(TAG, "Answer for %*s to %s", url->length, url->string, 
                        inet_ntoa(client_queue[i].src_address.sin_addr.s_addr));

            int sendlen = sendto(sock, packet->data, packet->length, 0, 
                                (struct sockaddr *)&client_queue[i].src_address, addrlen);

            if(sendlen <= 0)
            {
                ESP_LOGW(TAG, "Failed to forward answer");
            }
            ESP_LOGV(TAG, "Sent %d bytes", sendlen);
            ESP_LOGI(TAG, "Response Time for %*s: %lld ms", url->length, url->string, 
                    (esp_timer_get_time()-client_queue[i].response_latency)/1000);

            break;
        }
    }
    return ESP_OK;
}

esp_err_t toggle_blocking()
{
    xSemaphoreTake(blocking_mutex, portMAX_DELAY);
    blocking_status = !blocking_status;
    ESP_LOGI(TAG, "Changing blocking state to %d", blocking_status);
    set_led_state(BLOCKING, blocking_status);
    xSemaphoreGive(blocking_mutex);
    return ESP_OK;
}

IRAM_ATTR bool blocking_on()
{
    xSemaphoreTake(blocking_mutex, portMAX_DELAY);
    bool status = blocking_status;
    xSemaphoreGive(blocking_mutex);
    return status;
}

IRAM_ATTR uint8_t parse_query(Packet* packet, DNS_Query* query)
{
    query->header = (DNS_Header*)packet->data;
    query->qname_ptr = (char*)packet->data + sizeof(DNS_Header);
    if (strlen(query->qname_ptr) <= MAX_URL_LENGTH)
    {
        ESP_LOGV(TAG, "QName length: %d", strlen(query->qname_ptr));
        uint16_t* qtype_ptr = (uint16_t*)(query->qname_ptr + strlen(query->qname_ptr) + 1);
        query->qtype = ntohs(*qtype_ptr);

        uint16_t* qclass_ptr = (uint16_t*)(&query->qtype+sizeof(query->qtype));
        query->qclass = ntohs(*qclass_ptr);
    }
    else
    {
        ESP_LOGW(TAG, "Invalid dns query, qname too long");
        return 0;
    }
    return 1;
}

static IRAM_ATTR void dns_task(void* nvs_h)
{
    while(1)
    {
        Packet packet = {0};

        ESP_LOGD(TAG, "Waiting for packet");
        esp_err_t err = xQueueReceive(packet_queue, &packet, portMAX_DELAY);
        ESP_LOGD(TAG, "Got packet");

        if(err == pdFALSE)
        {
            ESP_LOGI(TAG, "Error receiving from queue");
        }
        else
        {
            DNS_Query query = {0};
            if(parse_query(&packet, &query))
            {
                URL url = convert_qname_to_url(query.qname_ptr);
                if(query.header->qr == QUERY)
                {
                    bool blocked = false;
                    ESP_LOGD(TAG, "Question for %s from %s", url.string,
                                    inet_ntoa(packet.src_address.sin_addr.s_addr));

                    if(memcmp(url.string, DEVICE_URL, url.length) == 0)
                    {
                        ESP_LOGW(TAG, "Redirecting Request for %s", DEVICE_URL);
                        redirect_response(query.header, &packet, query.qtype);
                    }
                    else
                    {
                        if(blocking_on() && (query.qtype == A || query.qtype == AAAA))
                        {
                            if(in_blacklist(url))
                            {
                                blocked = true;
                                ESP_LOGW(TAG, "Blocking question for %*s", url.length, url.string);
                                block_query(query.header, &packet, query.qtype);
                            }
                            else
                            {
                                ESP_LOGI(TAG, "Forwarding question for %*s", url.length, url.string);
                                forward_query(query.header, &packet);
                            }
                        }
                        else
                        {
                            ESP_LOGI(TAG, "Forwarding question for %*s", url.length, url.string);
                            forward_query(query.header, &packet);
                        }
                    }

                    log_query(url, blocked, packet.src_address.sin_addr.s_addr);
                }
                else if (query.header->qr == ANSWER)
                {
                    forward_answer(query.header, &packet, &url);
                }
            }
            int64_t end = esp_timer_get_time();
            ESP_LOGD(TAG, "Processing Time: %lld ms\n", (end-packet.recv_timestamp)/1000);
        }
    }
}

void start_dns()
{
    ESP_LOGI(TAG, "Starting DNS Task");
    blocking_mutex = xSemaphoreCreateMutex();
    packet_queue = xQueueCreate(PACKET_QUEUE_SIZE, sizeof(Packet));
    initialize_socket();

    xTaskCreatePinnedToCore(listening_task, "listening_task", 
                            8000, NULL, 9, &listening, 1);

    xTaskCreatePinnedToCore(dns_task, "dns_task", 16000, NULL, 9, &dns, 1);
}