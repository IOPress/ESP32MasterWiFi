#include <stdio.h>
#include "nvs_flash.h"
#include "esp_wifi.h"



static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    uint16_t n;
    esp_wifi_scan_get_ap_num(&n);
    wifi_ap_record_t rec;
    for (int i = 0; i < n; i++)
    {
        esp_wifi_scan_get_ap_record(&rec);
        printf("%s\n", rec.ssid);
    }
}

void app_main(void)
{

    esp_event_loop_create_default();
    esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_SCAN_DONE, wifi_event_handler, NULL);

    nvs_flash_init();
    esp_event_loop_create_default();
    wifi_init_config_t wificonfig = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wificonfig);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();
    esp_wifi_scan_start(NULL, false);
}