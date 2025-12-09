#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "string.h"

#include "socket.h"
#include "connectWiFi.h"
#include <netdb.h>

void socketReadTimeout(int sockfd, char buffer[], int len, int timeoutsec)
{
    int m = 0;
    fd_set rfds;
    struct timeval tv;
    do
    {
        FD_ZERO(&rfds);
        FD_SET(sockfd, &rfds);
        tv.tv_sec = timeoutsec;
        tv.tv_usec = 0;
        int s = select(sockfd + 1, &rfds, NULL, NULL, &tv);
        if (s < 0)
        {
            printf("read error %X\n", errno);
            break;
        }
        if (s == 0)
        {
            printf("read timeout\n");
            break;
        }
        int n = read(sockfd, buffer + m, len - m - 1);
        if (n >= 0)
        {
            m = m + n;
            buffer[m] = 0;
            printf("\ndata received %d\n\n", n);
        }
        if (n < 0)
        {
            if (errno == ENOTCONN)
            {
                printf("socket closed by server\n");
                break;
            }
            printf("read error %X\n", errno);
            break;
        }
        vTaskDelay(2);

    } while (true);
}

void app_main(void)
{
    wifiConnect("Co", "SSID", "password", NULL, NULL);
    while (wifiStatus != 1010)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
    };

    esp_netif_t *netif = esp_netif_get_default_netif();
    esp_netif_dns_info_t dns;
    esp_netif_get_dns_info(netif, ESP_NETIF_DNS_MAIN, &dns);
    char bufIP[20];
    printf("DNS:%s\n", esp_ip4addr_ntoa(&(dns.ip.u_addr.ip4), bufIP, 20));

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *servinfo;

    int status = getaddrinfo("www.example.com", "80", &hints, &servinfo);

    printf("IP address found:%s\n", esp_ip4addr_ntoa((esp_ip4_addr_t *)&(servinfo->ai_addr->sa_data[2]), bufIP, 20));

    if (connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) < 0)
        return;
    freeaddrinfo(servinfo);

    char header[] = "GET /index.html HTTP/1.1\r\nHost:example.com\r\n\r\n";
    int n = write(sockfd, header, strlen(header));
    printf("data sent  %d\n", n);

    int len = 1024 * 2;
    char buffer[len];
    socketReadTimeout(sockfd, buffer, len, 2);
    printf("Final buffer\n\n%s\n", buffer);
}