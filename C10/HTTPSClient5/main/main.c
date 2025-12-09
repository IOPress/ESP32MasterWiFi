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

#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/debug.h"

static void my_debug(void *ctx, int level,
                     const char *file, int line, const char *str)
{
    fprintf((FILE *)ctx, "%s", str);
    fflush((FILE *)ctx);
}

int mbedtls_net_send(void *ctx, const unsigned char *buf, size_t len)
{
    return write(*(int *)ctx, buf, len);
}
int mbedtls_net_recv(void *ctx, unsigned char *buf, size_t len)
{
    return (int)read(*(int *)ctx, buf, len);
}

void app_main(void)
{
    int err = 0;
    wifiConnect("Co", "SSID", "password", NULL, NULL);
    while (wifiStatus != 1010)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
    };
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(443);
    addr.sin_addr.s_addr = inet_addr("23.192.228.80");
    err = connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    if (err < 0)
    {
        printf("Socket error %X\n", errno);
    }

    mbedtls_ssl_config conf;
    mbedtls_ssl_config_init(&conf);
    err = mbedtls_ssl_config_defaults(&conf,
                                      MBEDTLS_SSL_IS_CLIENT,
                                      MBEDTLS_SSL_TRANSPORT_STREAM,
                                      MBEDTLS_SSL_PRESET_DEFAULT);
    mbedtls_entropy_context entropy;
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ctr_drbg_init(&ctr_drbg);
    const char *pers = "https_client";
    err = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func,
                                &entropy, (const unsigned char *)pers, strlen(pers));
    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random,
                         &ctr_drbg);
   
    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_NONE);

    mbedtls_debug_set_threshold(3);
    mbedtls_ssl_conf_dbg( &conf, my_debug, stdout );

    mbedtls_ssl_context ssl;
    mbedtls_ssl_init(&ssl);
    err = mbedtls_ssl_setup(&ssl, &conf);
    mbedtls_ssl_set_bio(&ssl, &sockfd, mbedtls_net_send, mbedtls_net_recv, NULL);



    char header[] = "GET /index.html HTTP/1.1\r\nHost:example.com\r\n\r\n";
    int n = mbedtls_ssl_write(&ssl,(u_char*) header, strlen(header));

    char buffer[2000];
    int len = sizeof(buffer) - 1;
    do
    {
        n = mbedtls_ssl_read(&ssl, (u_char*)buffer, len);

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
    close(sockfd);
    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&conf);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
    
    printf("Final buffer\n\n%s\n", buffer);
}