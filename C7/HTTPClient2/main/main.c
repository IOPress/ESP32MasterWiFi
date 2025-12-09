#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "string.h"

#include "socket.h"
#include "connectWiFi.h"

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
    addr.sin_addr.s_addr = inet_addr("23.192.228.10");

    if (connect(sockfd,
                (struct sockaddr *)&addr, sizeof(addr)) < 0)
        return;
    char header[] = "GET /index.html HTTP/1.1\r\n"
                    "Host:example.com\r\n\r\n";

    int n = write(sockfd, header, strlen(header));

    fcntl(sockfd, F_SETFL, O_NONBLOCK);
    int len = 1024 * 2;
    char buffer[len];
    int m = 0;
    int tout = 1000;
    int inc = 10;
    for (int t = 0; t < tout; t += inc)
    {
        n = read(sockfd, buffer + m, len - m - 1);
        if (n >= 0)
        {
            m = m + n;
            buffer[m] = 0;
            printf("\ndata received %d\n\n", n);
        }
        else if (errno != EAGAIN)
        {
            printf("error");
            break;
        }
        vTaskDelay(inc / portTICK_PERIOD_MS);
    }

    printf("Final buffer\n\n%s\n", buffer);
}