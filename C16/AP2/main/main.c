#include <stdio.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "dhcpserver/dhcpserver.h"
#include "dhcpserver/dhcpserver_options.h"
int retry_num = 0;

static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id)
    {
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
    esp_event_loop_create_default();
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);

    esp_netif_init();
    esp_netif_t *esp_netif = esp_netif_create_default_wifi_ap();

    esp_netif_ip_info_t ip_info;
    esp_netif_str_to_ip4("192.168.253.2", &ip_info.ip);
    esp_netif_str_to_ip4("192.168.253.1", &ip_info.gw);
    esp_netif_str_to_ip4("255.255.255.0", &ip_info.netmask);

    dhcps_lease_t dhcpPool = {
        .enable = true};
    IP4_ADDR(&dhcpPool.start_ip, 192, 168, 253, 240);
    IP4_ADDR(&dhcpPool.end_ip, 192, 168, 253, 243);
    esp_netif_dhcps_stop(esp_netif);
    esp_netif_set_ip_info(esp_netif, &ip_info);

    int err = esp_netif_dhcps_option(esp_netif, ESP_NETIF_OP_SET, ESP_NETIF_REQUESTED_IP_ADDRESS, &dhcpPool, sizeof(dhcpPool));
    printf("dchp error %X\n", err);
    esp_netif_dhcps_start(esp_netif);
 
    wifi_init_config_t wificonfig = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wificonfig);

    wifi_config_t APconf = {
        .ap = {
            .ssid = "ESP32",
            .password = "mypassword",
            .channel = 5,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .max_connection = 2}};
    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(WIFI_IF_AP, &APconf);
    esp_wifi_start();
}