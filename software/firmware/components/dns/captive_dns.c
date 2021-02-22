#include "captive_dns.h"
#include "dns.h"
#include "url.h"

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "lwip/ip_addr.h"

#define LOG_LOCAL_LEVEL ESP_LOG_WARN
#include "esp_log.h"
static const char *TAG = "DNS";

// DNS Answer (192.168.4.1) for captive dns
const char ip4_captive_response[] = "\xc0\x0c\x00\x01\x00\x01\x00\x00\x00\x02\x00\x04\xC0\xA8\x04\x01";

static TaskHandle_t captive_dns;


static void captive_dns_task(void* parameters)
{
    ESP_LOGD(TAG, "Starting captive DNS");

    // Setup recv_sock
    socklen_t addrlen = sizeof(struct sockaddr_in);
    struct sockaddr_in my_addr;
	my_addr.sin_family = PF_INET;
    my_addr.sin_port = htons(DNS_PORT);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int sock;
	if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		ESP_LOGW(TAG, "Failed To Create Socket");
		esp_restart(); // ¯\_(ツ)_/¯ 
	}

    if(bind(sock, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0)
    {
    	ESP_LOGW(TAG, "Failed To Bind Socket");
    	esp_restart(); // ¯\_(ツ)_/¯ 
    }
    ESP_LOGD(TAG, "Socket %d created and bound on port %d", sock, DNS_PORT);    

    ESP_LOGI(TAG, "Captive DNS Started");
    while(1)
    {
        //uint8_t buf[MAX_PACKET_SIZE];
        Packet packet = {0};
        struct sockaddr_in src_address;
        
        int recvlen = recvfrom(sock, packet.data, MAX_PACKET_SIZE, 0, 
                              (struct sockaddr *)&src_address, &addrlen);

        packet.length = recvlen;
        packet.recv_timestamp = esp_timer_get_time();

        if(recvlen > 0)
        {
            //DNS_Header *header = (DNS_Header*)buf;
            DNS_Query query = {0};
            if(parse_query(&packet, &query))
            {
                if(query.header->qr == 0) // question
                {
                    // char* qname_ptr = (char*)buf+sizeof(DNS_Header);
                    URL url = convert_qname_to_url(query.qname_ptr);
                    // uint16_t* qtype_ptr = (uint16_t*)(qname_ptr+strlen(qname_ptr)+1);
                    // uint16_t qtype = ntohs(*qtype_ptr);


                    size_t packet_len = 0;
                    char answer_packet[recvlen+AAAA_RECORD_ANSWER_SIZE];

                    ESP_LOGD(TAG, "Redirecting Question for %s from %s", url.string,
                                inet_ntoa(src_address.sin_addr.s_addr));

                    query.header->qr = 1; // change packet to answer
                    if(query.qtype == A)
                    {
                        query.header->aa = 1; // respect my authoritah
                        query.header->ans_count = htons(1);
                        
                        memcpy(answer_packet, query.header, recvlen);
                        memcpy(answer_packet+recvlen, ip4_captive_response, A_RECORD_ANSWER_SIZE);
                        packet_len = recvlen+A_RECORD_ANSWER_SIZE;
                    }
                    if(query.qtype == AAAA)
                    {
                        query.header->rd = 1; // recursion desired
                        query.header->ra = 1; // recursion available
                        query.header->rcode = 3; // No such name

                        memcpy(answer_packet, query.header, recvlen);
                        packet_len = recvlen;
                    }
                    
                    int sendlen = sendto(sock, answer_packet, packet_len, 0, 
                                        (struct sockaddr *)&src_address, addrlen);
                    if(sendlen < 0)
                    {
                        ESP_LOGI(TAG, "Error sending DNS response");
                    }
                }
            }
        } 
        else
        {
            ESP_LOGW(TAG, "Error Receiving Packet");
        }
    }
}

esp_err_t start_captive_dns()
{
    xTaskCreatePinnedToCore(captive_dns_task, "captive_dns_task", 8000, NULL, 5, 
                            &captive_dns, tskNO_AFFINITY);
    
    return ESP_OK;
}