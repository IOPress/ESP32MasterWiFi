#include <stdio.h>
#include "nvs_flash.h"
#include "esp_wifi.h"

#include "esp_netif.h"
int retry_num = 0;

static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        printf("WIFI CONNECTING....\n");
        break;
    case WIFI_EVENT_STA_CONNECTED:
        printf("WiFi CONNECTED\n");
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        printf("WiFi lost connection\n");
        if (retry_num < 5)
        {
            esp_wifi_connect();
            retry_num++;
            printf("Retrying to Connect...\n");
        }
        break;
    }
}

static void netif_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data){
    printf("Wifi got IP...\n\n");
}


void app_main(void)
{
    nvs_flash_init();

    esp_event_loop_create_default();
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);

    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, netif_event_handler, NULL);

    wifi_init_config_t wificonfig = WIFI_INIT_CONFIG_DEFAULT();

    esp_wifi_init(&wificonfig);
    esp_netif_init();

    wifi_config_t staconf = {
        .sta = {
            .ssid = "ssid",
            .password = "password",
            .threshold.authmode = WIFI_AUTH_WPA_PSK,
        }};
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &staconf);
    esp_netif_t *netif = esp_netif_create_default_wifi_sta();

    esp_wifi_start();
    esp_wifi_connect(); 
}