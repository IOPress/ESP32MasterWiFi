#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "string.h"
#include "esp_tls.h"
#include "esp_crt_bundle.h"
#include <mbedtls/base64.h>

#include "socket.h"
#include "connectWiFi.h"

static int write_ssl_and_get_response(esp_tls_t *tls, unsigned char *buf, size_t len)
{
    int ret;
    const size_t DATA_SIZE = 128;
    unsigned char data[DATA_SIZE];
    while (len && (ret = esp_tls_conn_write(tls, buf, len)) <= 0)
    {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
        {
            printf("mbedtls_ssl_write failed with error -0x%x", -ret);
            return ret;
        }
    }
    while (true)
    {
        ret = esp_tls_conn_read(tls, data, DATA_SIZE - 1);
        if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE)
            continue;
        if (ret <= 0)
        {
            printf("mbedtls_ssl_read failed with error -0x%x", errno);
            break;
        }
        data[ret] = 0;
        printf("%s", data);
        break;
    }
    return ret;
}

static int write_and_get_response(int sockfd, unsigned char *buf, size_t len)
{
    int ret;
    const size_t DATA_SIZE = 128;
    unsigned char data[DATA_SIZE];

    if (len && (ret = send(sockfd, buf, len, 0)) <= 0)
    {
        printf("mbedtls_write failed with error -0x%x", -ret);
        return ret;
    }

    ret = recv(sockfd, data, DATA_SIZE - 1, 0);
    if (ret <= 0)
    {
        printf("mbedtls_read failed with error -0x%x\n", errno);
        return ret;
    }
    data[ret] = 0;
    printf("%s", data);
    return ret;
}

void app_main(void)
{
    wifiConnect("Co", "SSID", "password", NULL, NULL);
    while (wifiStatus != 1010)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
    };

    esp_tls_cfg_t cfg = {
        .addr_family = ESP_TLS_AF_INET,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    char mailserver[] = "smtp.gmail.com";
    int sockfd;
    struct esp_tls_last_error error;
    esp_tls_error_handle_t herror = &error;
    int ret = esp_tls_plain_tcp_connect(mailserver, strlen(mailserver), 587, &cfg, herror, &sockfd);

    size_t BUF_SIZE = 512;
    char buf[BUF_SIZE];

    ret = write_and_get_response(sockfd, (unsigned char *)buf, 0);
    printf("\n\n");

    int len = snprintf((char *)buf, BUF_SIZE, "EHLO %s\r\n", "ESP32");
    ret = write_and_get_response(sockfd, (unsigned char *)buf, len);
    printf("\n\n");

    len = snprintf((char *)buf, BUF_SIZE, "STARTTLS\r\n");
    ret = write_and_get_response(sockfd, (unsigned char *)buf, len);

    ret = write_and_get_response(sockfd, (unsigned char *)buf, 0);
    printf("\n\n");

    esp_tls_t *tls = esp_tls_init();
    esp_tls_set_conn_state(tls, ESP_TLS_CONNECTING);
    esp_tls_set_conn_sockfd(tls, sockfd);
    ret = esp_tls_conn_new_sync(mailserver, strlen(mailserver), 587, &cfg, tls);

    len = snprintf((char *)buf, 512, "AUTH LOGIN\r\n");
    ret = write_ssl_and_get_response(tls, (unsigned char *)buf, len);
    printf("\n\n");
    char senderemail[] = "senderemail";
    unsigned char base64_buffer[128];
    size_t base64_len;
    ret = mbedtls_base64_encode((unsigned char *)base64_buffer, sizeof(base64_buffer),
                                &base64_len, (unsigned char *)senderemail, strlen(senderemail));
    len = snprintf((char *)buf, BUF_SIZE, "%s\r\n", base64_buffer);
    ret = write_ssl_and_get_response(tls, (unsigned char *)buf, len);
    printf("\n\n");

    char senderpassword[] = "senderpassword";
    ret = mbedtls_base64_encode((unsigned char *)base64_buffer, sizeof(base64_buffer),
                                &base64_len, (unsigned char *)senderpassword, strlen(senderpassword));
    len = snprintf((char *)buf, BUF_SIZE, "%s\r\n", base64_buffer);
    ret = write_ssl_and_get_response(tls, (unsigned char *)buf, len);
    printf("\n\n");

    len = snprintf((char *)buf, BUF_SIZE, "MAIL FROM:<%s>\r\n", senderemail);
    ret = write_ssl_and_get_response(tls, (unsigned char *)buf, len);

    char recipientemail[] = "recipientemail";
    len = snprintf((char *)buf, BUF_SIZE, "RCPT TO:<%s>\r\n", recipientemail);
    ret = write_ssl_and_get_response(tls, (unsigned char *)buf, len);

    len = snprintf((char *)buf, BUF_SIZE, "DATA\r\n");
    ret = write_ssl_and_get_response(tls, (unsigned char *)buf, len);

    len = snprintf((char *)buf, BUF_SIZE,
                   "From: %s\r\nSubject: mbed TLS Test mail\r\n"
                   "To: %s\r\n"
                   "This is a simple test mail from the SMTP client example.\r\n"
                   "\r\n"
                   "\r\n.\r\n",
                   "ESP32 SMTP Client", recipientemail);
    ret = esp_tls_conn_write(tls, (unsigned char *)buf, len);
}