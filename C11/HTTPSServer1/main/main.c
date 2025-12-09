#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "string.h"

#include "connectWiFi.h"

#include "esp_https_server.h"

esp_err_t get_handlertemp(httpd_req_t *req)
{
    const char resp[] = "Temperature is 20.3";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}
esp_err_t get_handlerhum(httpd_req_t *req)
{
    const char resp[] = "Humidity is 80%%";
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

    u_char key[] = "-----BEGIN PRIVATE KEY-----\n"
                   "MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQgCb3W8d9cGuHL+s+b\n"
                   "ZIqjNac1GYI8Y48x9zqEQj+1eiihRANCAATEBr895ERVG8DHAvc8DRovkz0xYNKd\n"
                   "4WJKB23x4OBXXOAvdVRzKrmXYYMsZWZwehkTpTwEd3rHrDZaR54SzbZo\n"
                   "-----END PRIVATE KEY-----\n";
    u_char cert[] = "-----BEGIN CERTIFICATE-----\n"
                    "MIIB3jCCAYWgAwIBAgIUFw6Y69PWFes18KM4d0k3yNTa3X0wCgYIKoZIzj0EAwIw\n"
                    "RTELMAkGA1UEBhMCVVMxEzARBgNVBAgMClNvbWUtU3RhdGUxITAfBgNVBAoMGElu\n"
                    "dGVybmV0IFdpZGdpdHMgUHR5IEx0ZDAeFw0yNTEwMjYwNjQ4MjBaFw0yNjEwMjYw\n"
                    "NjQ4MjBaMEUxCzAJBgNVBAYTAlVTMRMwEQYDVQQIDApTb21lLVN0YXRlMSEwHwYD\n"
                    "VQQKDBhJbnRlcm5ldCBXaWRnaXRzIFB0eSBMdGQwWTATBgcqhkjOPQIBBggqhkjO\n"
                    "PQMBBwNCAATEBr895ERVG8DHAvc8DRovkz0xYNKd4WJKB23x4OBXXOAvdVRzKrmX\n"
                    "YYMsZWZwehkTpTwEd3rHrDZaR54SzbZoo1MwUTAdBgNVHQ4EFgQUonOHUDw/aPWK\n"
                    "mD/Gw8DTfEtiXdIwHwYDVR0jBBgwFoAUonOHUDw/aPWKmD/Gw8DTfEtiXdIwDwYD\n"
                    "VR0TAQH/BAUwAwEB/zAKBggqhkjOPQQDAgNHADBEAiBhcyxGHH9zbzYkMD6hkZHG\n"
                    "Qr+tES5jcYYPr0Xccw4iGAIgHL859AAriuORNw/yhjkczngIziwBZ3/aWOPUVkaH\n"
                    "k/o=\n"
                    "-----END CERTIFICATE-----\n";
    httpd_ssl_config_t config = HTTPD_SSL_CONFIG_DEFAULT();
    config.servercert = cert;
    config.servercert_len = sizeof(cert);
    config.prvtkey_pem = key;
    config.prvtkey_len = sizeof(key);

    httpd_handle_t server = NULL;
    httpd_uri_t uri_get = {
        .uri = "/temp",
        .method = HTTP_GET,
        .handler = get_handlertemp,
        .user_ctx = NULL};

    if (httpd_ssl_start(&server, &config) == ESP_OK)
    {
        httpd_register_uri_handler(server, &uri_get);
        uri_get.uri = "/hum";
        uri_get.handler = get_handlerhum;
        httpd_register_uri_handler(server, &uri_get);
    }
    while (true)
    {
        vTaskDelay(1000);
    };
}
