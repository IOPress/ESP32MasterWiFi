#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "string.h"

#include "connectWiFi.h"

#include "socket.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/debug.h"


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

int mbedtls_net_send(void *ctx, const unsigned char *buf,
                     size_t len)
{
    int n = write(*(int *)ctx, buf, len);
    if (n < 0)
        printf("write error  %d/n", n);
    return n;
}

int mbedtls_net_recv(void *ctx, unsigned char *buf, size_t len)
{
    int n = (int)read(*(int *)ctx, buf, len);
    if (n < 0)
        printf("read error %d/n", n);
    return n;
}

void setupSSLconfig(mbedtls_ssl_config *conf,
                    mbedtls_entropy_context *entropy,
                    mbedtls_ctr_drbg_context *ctr_drbg)
{
    int err = 0;
    mbedtls_ssl_config_init(conf);
    err = mbedtls_ssl_config_defaults(conf, MBEDTLS_SSL_IS_SERVER,
                                      MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
    if (err < 0)
        printf("%d", err);

    mbedtls_entropy_init(entropy);
    mbedtls_ctr_drbg_init(ctr_drbg);
    const char *pers = "https_server";
    err = mbedtls_ctr_drbg_seed(ctr_drbg, mbedtls_entropy_func,
                                entropy, (const unsigned char *)pers, strlen(pers));
    mbedtls_ssl_conf_rng(conf, mbedtls_ctr_drbg_random, ctr_drbg);
}

void setupCert(mbedtls_x509_crt *srvcert, mbedtls_pk_context *srvkey)
{

    mbedtls_x509_crt_init(srvcert);
    int err = mbedtls_x509_crt_parse(srvcert, cert, sizeof(cert));
    if (err < 0)
        printf("%d", err);

    mbedtls_pk_init(srvkey);
    err = mbedtls_pk_parse_key(srvkey, key, sizeof(key), NULL, 0, NULL, 0);
    if (err < 0)
        printf("%d", err);
}

int doHandshake(mbedtls_ssl_context *ssl)
{
    int err;
    while ((err = mbedtls_ssl_handshake(ssl)) != 0)
    {
        if (err != MBEDTLS_ERR_SSL_WANT_READ &&
            err != MBEDTLS_ERR_SSL_WANT_WRITE)
            break;
    }
    return err;
}
int readData(mbedtls_ssl_context *ssl, char buffer[], int len)
{
    int n;
    do
    {
        n = mbedtls_ssl_read(ssl, (u_char *)buffer, len - 1);
    } while (n == MBEDTLS_ERR_SSL_WANT_READ || n == MBEDTLS_ERR_SSL_WANT_WRITE);
    if (n > 0)
        buffer[n] = 0;
    return n;
}

int writeData(mbedtls_ssl_context *ssl, char buffer[], int len)
{
    int n = 0;
    while (true)
    {
        n = mbedtls_ssl_write(ssl, (u_char *)buffer, len);
        if (n > 0)
            return n;
        if (n != MBEDTLS_ERR_SSL_WANT_WRITE || n != MBEDTLS_ERR_SSL_WANT_READ)
        {
            printf(" failed\n  ! mbedtls_ssl_write returned %d\n\n", n);
            return n;
        }
    }
}

void getPage(char data[], int len)
{

    char headers[200] = "HTTP/1.1 200 OK\r\n"
                        "Content-Type:text/html;"
                        "charset=UTF-8\r\n"
                        "Server:ESP32\r\n";
    char html[] = "<html><head><title>Hello HTTP World</title></head>"
                  "<body><p>Hello HTTP World</p></body>"
                  "</html>\r\n";
    char ContLen[100] = {0};
    snprintf(ContLen, sizeof ContLen, "Content-Length:%d \r\n\r\n", strlen(html));
    strcat(headers, ContLen);
    snprintf(data, len, "%s%s%c", headers, html, '\0');
}

void notifyClose(mbedtls_ssl_context *ssl)
{
    int err;
    while ((err = mbedtls_ssl_close_notify(ssl)) < 0)
    {
        if (err != MBEDTLS_ERR_SSL_WANT_READ &&
            err != MBEDTLS_ERR_SSL_WANT_WRITE)
            break;
    }
}

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

    mbedtls_ssl_config conf;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    setupSSLconfig(&conf, &entropy, &ctr_drbg);

    mbedtls_x509_crt srvcert;
    mbedtls_pk_context srvkey;
    setupCert(&srvcert, &srvkey);

    int err = mbedtls_ssl_conf_own_cert(&conf, &srvcert, &srvkey);

    int len = 1024 * 2;
    char buffer[len];
    struct sockaddr_storage client_addr;
    socklen_t addr_size = sizeof client_addr;
    while (true)
    {
        int client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size);

        mbedtls_ssl_context ssl;
        mbedtls_ssl_init(&ssl);
        err = mbedtls_ssl_setup(&ssl, &conf);
        if (err < 0)
            printf("%d", err);

        mbedtls_ssl_set_bio(&ssl, &client_fd, mbedtls_net_send, mbedtls_net_recv, NULL);
        if (doHandshake(&ssl) < 0)
        {
            mbedtls_ssl_session_reset(&ssl);
            close(client_fd);
            continue;
        };

        if (readData(&ssl, buffer, sizeof(buffer)) < 0)
        {
            mbedtls_ssl_session_reset(&ssl);
            close(client_fd);
            continue;
        };
        printf("Final buffer\n\n%s\n", buffer);

        char data[2048] = {0};
        getPage(data, sizeof(data));
        if (writeData(&ssl, data, strlen(data)) < 0)
        {
            mbedtls_ssl_session_reset(&ssl);
            close(client_fd);
            continue;
        };
        printf("page sent\n");
        mbedtls_ssl_session_reset(&ssl);
        close(client_fd);
    }

    while (true)
    {
        vTaskDelay(1000);
    };
}
