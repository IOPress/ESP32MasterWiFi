#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "string.h"
#include "socket.h"
#include "connectWiFi.h"

#include "esp_tls.h"
#include "esp_crt_bundle.h"

void app_main(void)
{
    wifiConnect("Co", "SSID", "password", NULL, NULL);
    while (wifiStatus != 1010)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
    };

    esp_tls_t *tls = esp_tls_init();
    esp_tls_cfg_t cfg = {
        .crt_bundle_attach = esp_crt_bundle_attach,
    };
    esp_tls_conn_http_new_sync("https://example.com", &cfg, tls);



    char header[] = "GET /index.html HTTP/1.1\r\nHost:example.com\r\n\r\n";

    int m = 0;
    int n = 0;
    do
    {
        n = esp_tls_conn_write(tls, header + m, strlen(header) - m);
        if (n >= 0)
        {
            m += n;
        }
        else if (n != ESP_TLS_ERR_SSL_WANT_READ && n != ESP_TLS_ERR_SSL_WANT_WRITE)
        {
            esp_tls_conn_destroy(tls);
        }
    } while (m < strlen(header));


    char buffer[2000];
    int len = sizeof(buffer) - 1;
    do
    {

        n = esp_tls_conn_read(tls, buffer, len);

        if (n == ESP_TLS_ERR_SSL_WANT_WRITE || n == ESP_TLS_ERR_SSL_WANT_READ)
        {
            continue;
        }
        else if (n < 0)
        {
            break;
        }
        else if (n == 0)
        {
            break;
        }
        buffer[n] = 0;
        printf("%s", buffer);
    } while (true);    
}