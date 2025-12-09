#include <stdio.h>
#include "esp_wifi.h"
#include "esp_netif.h"
#include "string.h"
#include "nvs_flash.h"

#include "connectwifi.h"

void app_main(void)
{
 /*    esp_netif_ip_info_t ip_info;

    esp_netif_str_to_ip4("192.168.253.204", &ip_info.ip);
    esp_netif_str_to_ip4("192.168.253.1", &ip_info.gw);
    esp_netif_str_to_ip4("255.255.255.0", &ip_info.netmask);
    wifiConnect("Co", SSID", "password", "MyESP", &ip_info); */

    wifiConnect("Co", "SSID", "password", "MyESP", NULL);
    while (wifiStatus != 1010)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
    };
    // use WiFi connection
}