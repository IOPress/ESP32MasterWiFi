#include <stdio.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "dhcpserver/dhcpserver.h"
#include "dhcpserver/dhcpserver_options.h"
#include "lwip/lwip_napt.h"
int wifiStatus = 1000;
int retry_num = 0;
static void wifi_event_handler(void *event_handler_arg,
                               esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        printf("Got STA START\n");
        break;
    case WIFI_EVENT_STA_CONNECTED:
        printf("Got STA Connect\n");
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        if (retry_num < 5)
        {
            esp_wifi_connect();
            retry_num++;
        }
        break;
    case IP_EVENT_STA_GOT_IP:
        printf("Got IP\n");
        wifiStatus = 1010;
        break;
    case WIFI_EVENT_AP_STACONNECTED:
        printf("STA CONNECTING....\n");
        break;
    case WIFI_EVENT_AP_STADISCONNECTED:
        printf("STA DISCONNECTED\n");
        break;
    }
}

void app_main(void)
{

    nvs_flash_init();
    esp_netif_init();

    esp_event_loop_create_default();
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                               wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                               wifi_event_handler, NULL);

    wifi_init_config_t wificonfig = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wificonfig);

    esp_wifi_set_mode(WIFI_MODE_APSTA);
    esp_netif_t *esp_netif_ap = esp_netif_create_default_wifi_ap();
    esp_netif_t *esp_netif_sta = esp_netif_create_default_wifi_sta();

    wifi_config_t staconf = {
        .sta = {
            .ssid = "SSID",
            .password = "password",
            .threshold.authmode = WIFI_AUTH_WPA_PSK,
        }};
    esp_wifi_set_config(WIFI_IF_STA, &staconf);

    wifi_config_t APconf = {
        .ap = {
            .ssid = "ESP32",
            .authmode = WIFI_AUTH_OPEN,
            .max_connection = 2}};
    esp_wifi_set_config(WIFI_IF_AP, &APconf);

    esp_wifi_start();
    esp_wifi_connect();
    while (wifiStatus != 1010)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    esp_netif_dns_info_t dns;
    esp_netif_get_dns_info(esp_netif_sta, ESP_NETIF_DNS_MAIN, &dns);
    uint8_t dhcps_offer_option = 0x02;
    esp_netif_dhcps_stop(esp_netif_ap);
    esp_netif_dhcps_option(esp_netif_ap, ESP_NETIF_OP_SET, ESP_NETIF_DOMAIN_NAME_SERVER, &dhcps_offer_option, sizeof(dhcps_offer_option));
    esp_netif_set_dns_info(esp_netif_ap, ESP_NETIF_DNS_MAIN, &dns);
    esp_netif_dhcps_start(esp_netif_ap);

    esp_netif_set_default_netif(esp_netif_sta);
    int err = esp_netif_napt_enable(esp_netif_ap);
    
}