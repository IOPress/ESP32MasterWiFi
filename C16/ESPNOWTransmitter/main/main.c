#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "string.h"

#include "esp_now.h"

static void example_espnow_send_cb(const esp_now_send_info_t *tx_info, esp_now_send_status_t status)
{
    printf("Send\n");
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
    esp_wifi_set_channel(3, WIFI_SECOND_CHAN_NONE);
    esp_wifi_start();

    int err = esp_now_init();
    err = esp_now_register_send_cb(example_espnow_send_cb);

    esp_now_peer_info_t peer = {
        .peer_addr = {0x08, 0xB6, 0x1F, 0x29, 0xBE, 0x7C},
        .encrypt = false,
    };

      esp_now_peer_info_t broadcast = {
        .peer_addr = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
        .encrypt = false,
    };

    err = esp_now_add_peer(&broadcast);

    uint8_t data[] = {1, 2, 3, 4, 5, 6};
    err = esp_now_send(broadcast.peer_addr,data,6);
    printf("error= %x\n", err);

    while (true)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    };
}