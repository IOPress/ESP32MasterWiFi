#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "string.h"

#include "socket.h"
#include "connectWiFi.h"
#include "esp_netif_sntp.h"
#include "esp_sntp.h"
void app_main(void)
{
    wifiConnect("Co", "SSID", "password", NULL, NULL);
    while (wifiStatus != 1010)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
    };

    esp_sntp_setoperatingmode(ESP_SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");

    esp_sntp_init();
    const int retry_count = 15;
    int retry = 0;
    while (esp_netif_sntp_sync_wait(2000 / portTICK_PERIOD_MS) == ESP_ERR_TIMEOUT && ++retry < retry_count) {
        printf( "Waiting for system time to be set... (%d/%d)", retry, retry_count);
    }
    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct tm *tm = localtime(&(tv.tv_sec));
    char date[100];
    strftime(date, 100, "%a, %d %b %Y %T", tm);
    printf("date = %s\n", date);  
}