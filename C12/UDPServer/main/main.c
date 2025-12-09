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

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in srvaddr;
    srvaddr.sin_family = AF_INET;
    srvaddr.sin_port = htons(8080);
    srvaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    int err = bind(sockfd, (struct sockaddr *)&srvaddr, sizeof(srvaddr));

    int len = 1024 * 2;
    char buffer[len];
    struct sockaddr_in clientaddr;
    int addrLen = sizeof clientaddr;
    int n = 0;
    while (true)
    {
        n = recvfrom(sockfd, buffer, len - 1, MSG_DONTWAIT, (struct sockaddr *)&clientaddr, (socklen_t *)&addrLen);
        printf("%d\n", n);
        if (n > 0)
            break;
        if(errno != EAGAIN || errno!= EWOULDBLOCK)
            break;
        vTaskDelay(200);
    }

    buffer[n] = 0;

    printf("Received message from IP: %s and port: %i\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
    printf("Msg from client: %s\n", buffer);
}