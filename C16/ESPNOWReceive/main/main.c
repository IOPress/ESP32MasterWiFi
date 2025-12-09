#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "string.h"

#include "esp_now.h"

static void example_espnow_recv_cb(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len)
{

    for (int i = 0; i < len; i++)
    {
        printf("%2X", data[i]);
    }
    printf("\n");
    fflush(stdout);
}

void app_main(void)
{
    esp_event_loop_create_default();
    nvs_flash_init();
    esp_netif_init();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_channel(3,WIFI_SECOND_CHAN_NONE);
    esp_wifi_start();

    uint8_t mac[6];
    esp_wifi_get_mac(ESP_IF_WIFI_STA, mac);
    for (int i = 0; i < 6; i++)
    {
        printf("%2X ", mac[i]);
    }
    printf("\n");
    esp_now_init();
    esp_now_register_recv_cb(example_espnow_recv_cb);

    while (true)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
    };
}