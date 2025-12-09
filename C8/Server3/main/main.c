#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "string.h"

#include "connectwifi.h"
#include "esp_http_server.h"

esp_err_t get_handlertemp(httpd_req_t *req)
{
    const char resp[] = "Temperature is 20.3";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t get_handlerhum(httpd_req_t *req)
{
    /* Send a simple response */
    const char resp[] = "Humidity is 80%%";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t get_handler(httpd_req_t *req)
{
    const char resp[] = "Temperature is 20.3";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

void app_main(void)
{
    wifiConnect("Co", "SSID", "password", NULL, NULL);
    while (wifiStatus != 1010)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
    };

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    httpd_uri_t uri_get = {
        .uri = "/temp",
        .method = HTTP_GET,
        .handler = get_handlertemp,
        .user_ctx = NULL};

    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_register_uri_handler(server, &uri_get);
        uri_get.uri = "/hum";
        uri_get.handler = get_handlerhum;
        httpd_register_uri_handler(server, &uri_get);
    }
}