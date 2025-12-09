#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "string.h"
#include "esp_http_client.h"
#include "connectwifi.h"

esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
    static int pos = 0;
    switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:
        printf("HTTP_EVENT_ERROR\n");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        printf("HTTP_EVENT_ON_CONNECTED\n");
        break;
    case HTTP_EVENT_HEADER_SENT:
        printf("HTTP_EVENT_HEADER_SENT\n");
        break;
    case HTTP_EVENT_ON_HEADER:
        printf("HTTP_EVENT_ON_HEADER\n");
        printf("header = %s , %s\n", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        printf("HTTP_EVENT_ON_DATA, len=%d\n", evt->data_len);
        if (!esp_http_client_is_chunked_response(evt->client))
        {
            printf("%.*s", evt->data_len, (char *)evt->data);
            char *buf = (char *)(evt->user_data);
            memcpy(buf + pos, evt->data, evt->data_len);
            pos += evt->data_len;
            buf[pos] = 0;
        }
        break;
    case HTTP_EVENT_ON_FINISH:
        printf("HTTP_EVENT_ON_FINISH\n");
        pos = 0;
        break;
    case HTTP_EVENT_DISCONNECTED:
        printf("HTTP_EVENT_DISCONNECTED\n");
        break;
    default:
    }
    return ESP_OK;
}

char httpdata[2000];

void app_main(void)
{
    wifiConnect("Co", "SSID", "password", NULL, NULL);
    while (wifiStatus != 1010)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
    };

    esp_http_client_config_t httpconfig = {
        .url = "https://example.com",
        .method = HTTP_METHOD_GET,
        .event_handler = http_event_handler,
        .buffer_size = DEFAULT_HTTP_BUF_SIZE,
        .buffer_size_tx = DEFAULT_HTTP_BUF_SIZE,
        .user_data = httpdata,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
    };

    esp_http_client_handle_t httphandle = esp_http_client_init(&httpconfig);
    esp_http_client_perform(httphandle);
    printf("len data= %d\n", strlen(httpdata));
    printf("html \n %s\n ", httpdata);
}