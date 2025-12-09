#include <stdio.h>
#include "nvs_flash.h"
#include "esp_wifi.h"

void app_main(void)
{
    nvs_flash_init();

    wifi_init_config_t wificonfig = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wificonfig);

    wifi_config_t staconf = {
        .sta = {
            .ssid = "ssid",
            .password = "password",
            .threshold.authmode = WIFI_AUTH_WPA_PSK,
        }};
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &staconf);
    esp_wifi_start();
    esp_wifi_connect();

    vTaskDelay(500);
    wifi_ap_record_t ap_inf;
    esp_wifi_sta_get_ap_info(&ap_inf);
    printf("Channel %d\n", ap_inf.primary);
}