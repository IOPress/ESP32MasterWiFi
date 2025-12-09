#include <stdio.h>
#include "nvs_flash.h"
#include "esp_wifi.h"

void app_main(void)
{
    
    nvs_flash_init();
    esp_event_loop_create_default();
    wifi_init_config_t wificonfig = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wificonfig);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();
    esp_wifi_scan_start(NULL, true);
    uint16_t n;
    esp_wifi_scan_get_ap_num(&n);
    wifi_ap_record_t rec;
    for (int i = 0; i < n; i++)
    {
        esp_wifi_scan_get_ap_record(&rec);
        printf("%s\n", rec.ssid);
    }
}