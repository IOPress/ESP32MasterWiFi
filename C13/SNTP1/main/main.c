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
    srvaddr.sin_port = htons(123);
    srvaddr.sin_addr.s_addr = inet_addr("131.111.8.28");
    //      srvaddr.sin_addr.s_addr =inet_addr("192.168.253.73");

    int len = 48;
    char buffer[len];
    memset(buffer, 0, len);
    buffer[0] = 0x23;

    sendto(sockfd, buffer, 48, 0, (struct sockaddr *)&srvaddr, sizeof srvaddr);

    int n = recvfrom(sockfd, buffer, len, 0, NULL, NULL);

    uint32_t seconds_since_1900 = (buffer[40] << 24 | buffer[41] << 16 | buffer[42] << 8 | buffer[43]);
    time_t seconds_since_1970 = seconds_since_1900 - 2208988800;

    printf("time stamp %lld\n", seconds_since_1970);

    struct timeval tv;
    tv.tv_sec = seconds_since_1970;
    settimeofday(&tv, NULL);

    gettimeofday(&tv, NULL);
    struct tm *tm = localtime(&(tv.tv_sec));
    // struct tm *tm = localtime(&(seconds_since_1970));
    char date[100];
    strftime(date, 100, "%a, %d %b %Y %T", tm);
    printf("date = %s\n", date);
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    gettimeofday(&tv, NULL);
    tm = localtime(&(tv.tv_sec));
    strftime(date, 100, "%a, %d %b %Y %T", tm);
    printf("date = %s\n", date);
}