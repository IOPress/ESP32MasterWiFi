#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "string.h"

#include "connectWiFi.h"
#include "esp_tls.h"
#include "socket.h"

u_char key[] = "-----BEGIN PRIVATE KEY-----\n"
               "MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQgNgzd8KrRZldyzSSA\n"
               "aeoQz6fseiJV4aflT2JzJDgFBZ+hRANCAARO0C3CI4jpsdWL/05GBCPivy9SlMGl\n"
               "F8RqlGATf9Agz8nxmWGbWlyJ+/hV1AUjuOUvOeCUxaBoRc2zvqC0pdme\n"
               "-----END PRIVATE KEY-----\n";
u_char cert[] = "-----BEGIN CERTIFICATE-----\n"
                "MIIB3zCCAYWgAwIBAgIUfuveYhwTmXHCRI0buiTT/NGg/VUwCgYIKoZIzj0EAwIw\n"
                "RTELMAkGA1UEBhMCVVMxEzARBgNVBAgMClNvbWUtU3RhdGUxITAfBgNVBAoMGElu\n"
                "dGVybmV0IFdpZGdpdHMgUHR5IEx0ZDAeFw0yNTEwMjUxMDM2NTFaFw0yNjEwMjUx\n"
                "MDM2NTFaMEUxCzAJBgNVBAYTAlVTMRMwEQYDVQQIDApTb21lLVN0YXRlMSEwHwYD\n"
                "VQQKDBhJbnRlcm5ldCBXaWRnaXRzIFB0eSBMdGQwWTATBgcqhkjOPQIBBggqhkjO\n"
                "PQMBBwNCAARO0C3CI4jpsdWL/05GBCPivy9SlMGlF8RqlGATf9Agz8nxmWGbWlyJ\n"
                "+/hV1AUjuOUvOeCUxaBoRc2zvqC0pdmeo1MwUTAdBgNVHQ4EFgQUpWAfLSO3TxuE\n"
                "wZQZwg8bKAT8IKMwHwYDVR0jBBgwFoAUpWAfLSO3TxuEwZQZwg8bKAT8IKMwDwYD\n"
                "VR0TAQH/BAUwAwEB/zAKBggqhkjOPQQDAgNIADBFAiBruNYUmtPg0eWQGxFiA7U5\n"
                "aVWQjidRBf67JQLQXa2dOAIhAPcreSEwtF7Pch12IWMJeGyCf+kKqRO82SbwWpyh\n"
                "MdbX\n"
                "-----END CERTIFICATE-----\n";

void app_main(void)
{
    wifiConnect("Co", "SSID", "password", NULL, NULL);
    while (wifiStatus != 1010)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
    };

    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(443);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    listen(sockfd, 3);

    esp_tls_cfg_server_t cfg = {
        .servercert_buf = cert,
        .servercert_bytes = sizeof(cert),
        .serverkey_buf = key,
        .serverkey_bytes = sizeof(key),
    };

    int len = 1024 * 2;
    char buffer[len];
    struct sockaddr_storage client_addr;
    socklen_t addr_size = sizeof client_addr;
    while (true)
    {
        int client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size);

        esp_tls_t *tls = esp_tls_init();
        esp_err_t ret = esp_tls_server_session_create(&cfg, client_fd, tls);
        if (ret < 0)
        {
            esp_tls_conn_destroy(tls);
            continue;
        }
        printf("error session created %X\n", -ret);
        
        int n = esp_tls_conn_read(tls, buffer, len - 1);
        buffer[n] = 0;
        printf("\ndata received %d\n\n", n);
        printf("Final buffer\n\n%s\n", buffer);

        char headers[] = "HTTP/1.1 200 OK\r\n"
                         "Content-Type:text/html;"
                         "charset=UTF-8\r\n"
                         "Server:ESP32\r\n"
                         "Content-Length:";
        char html[] = "<html><head><title>Hello HTTP World</title></head>"
                      "<body><p>Hello HTTP World</p></body>"
                      "</html>\r\n";
        char data[2048] = {0};
        snprintf(data, sizeof data, "%s%d\r\n\r\n%s", headers, strlen(html), html);

        n = esp_tls_conn_write(tls, data, strlen(data));

        printf("data sent \n");
        esp_tls_conn_destroy(tls);
    }
    while (true)
    {
        vTaskDelay(1000);
    };
}
