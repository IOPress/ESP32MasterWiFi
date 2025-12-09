#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "string.h"

#include "esp_mesh.h"

void mesh_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
   
}

void app_main(void)
{
    esp_event_loop_create_default();
    nvs_flash_init();
    esp_netif_init();
    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&config);
    esp_wifi_start();

    esp_mesh_init();
    // esp_event_handler_register(MESH_EVENT, ESP_EVENT_ANY_ID, &mesh_event_handler, NULL);

    mesh_cfg_t cfg = MESH_INIT_CONFIG_DEFAULT();
    uint8_t MESH_ID[6] = {0x77, 0x77, 0x77, 0x77, 0x77, 0x77};
    memcpy((uint8_t *)&cfg.mesh_id, MESH_ID, 6);

    char ssid[] = "SSID";
    char password[] = "password";
    char APpassword[] = "APpassword";
    cfg.router.ssid_len = strlen(ssid);
    memcpy((uint8_t *)&cfg.router.ssid, ssid, cfg.router.ssid_len);
    memcpy((uint8_t *)&cfg.router.password, password, strlen(password));
    memcpy((uint8_t *)&cfg.mesh_ap.password, APpassword, strlen(APpassword));
    cfg.mesh_ap.max_connection = 5;
    esp_mesh_set_config(&cfg);
    esp_mesh_start();

    mesh_addr_t from;
    mesh_data_t datarx;
    uint8_t rxbuf[100] = {
        0,
    };
    datarx.data = rxbuf;
    datarx.size = 100;
    int flag;

    mesh_data_t data;
    data.data = (u_int8_t *)"Hello mesh world";
    data.size = strlen((char *)data.data);
    data.proto = MESH_PROTO_BIN;
    data.tos = MESH_TOS_P2P;
    while (true)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        if (esp_mesh_is_root())
        {
            esp_mesh_recv(&from, &datarx, 0, &flag, NULL, 0);
            printf("%d\n", flag);
            if (flag != 0)
                printf("%s\n", datarx.data);
        }
        else
        {
            esp_mesh_send(NULL, &data, MESH_DATA_P2P, NULL, 0);
        }
    };
}