#include <stdio.h>
#include "nvs_flash.h"
#include "esp_wifi.h"


void app_main(void)
{
    nvs_flash_init();
    esp_event_loop_create_default();


    esp_netif_init();
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t wificonfig = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wificonfig);

    wifi_config_t APconf = {
        .ap = {
            .ssid = "ESP32",
            .password = "mypassword",
            .channel=5,
            .authmode=WIFI_AUTH_WPA2_PSK,
            .max_connection=2
        }};
   
    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(WIFI_IF_AP, &APconf);
    esp_wifi_start();

}