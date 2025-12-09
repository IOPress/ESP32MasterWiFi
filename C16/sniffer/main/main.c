#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "string.h"

static void promiscuous_rx_cb(void *buf, wifi_promiscuous_pkt_type_t type)
{
    switch (type)
    {
    case WIFI_PKT_MGMT:
        printf("Received packet: management\n");
        break;
    case WIFI_PKT_CTRL:
        printf("Received packet: control\n");
        break;
    case WIFI_PKT_DATA:
        printf("Received packet: data\n");
        break;
    case WIFI_PKT_MISC:
        printf("Received packet: misc\n");
        break;
    }
}
char header[6000] = {"1"};
void app_main(void)
{

    nvs_flash_init();
    esp_event_loop_create_default();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_wifi_set_mode(WIFI_MODE_NULL);

    esp_wifi_set_promiscuous_rx_cb(promiscuous_rx_cb);

    wifi_promiscuous_filter_t mask;
    mask.filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT;
    esp_wifi_set_promiscuous_filter(&mask);
    esp_wifi_set_channel(10, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(true);
}