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
    //    srvaddr.sin_addr.s_addr = inet_addr("192.168.253.73");
    srvaddr.sin_addr.s_addr = INADDR_BROADCAST;
    char broadcast = '1';
    setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast);
    int len = 1024 * 2;
    char buffer[1024 * 2] = "Hello UDP World";
    sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&srvaddr, sizeof srvaddr);
}