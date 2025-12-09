#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "string.h"

#include "socket.h"
#include "connectWiFi.h"

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
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr.s_addr = inet_addr("23.192.228.80");

    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        return;
    char header[] = "GET /index.html HTTP/1.1\r\n"
                    "Host:example.com\r\n\r\n";

    int n = write(sockfd, header, strlen(header));
    printf("data sent  %d\n", n);

    int len = 1024 * 2;
    char buffer[len];
    socketReadTimeout(sockfd, buffer, len, 2);
    printf("Final buffer\n\n%s\n", buffer);
}