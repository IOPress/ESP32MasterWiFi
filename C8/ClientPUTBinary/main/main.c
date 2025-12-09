#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "string.h"

#include "socket.h"
#include "connectWiFi.h"
#include <netdb.h>

typedef struct
{
    char *req;
    char *port;
    char *url;
    char *path;
    char *buffer;
    int len;
    char *senddata;
    int lensenddata;
    bool done;
} Page;

Page initPage(char req[], char port[], char url[], char path[], char buffer[], int len, char *senddata, int lensenddata)
{
    Page page;
    page.req = req;
    page.url = url;
    page.path = path;
    page.port = port;
    page.buffer = buffer;
    page.len = len;
    page.senddata = senddata;
    page.lensenddata = lensenddata;
    page.done = false;
    return page;
}

void socketReadTimeout(int sockfd, char buffer[], int len, int timeoutsec)
{
    int m = 0;
    fd_set rfds;
    struct timeval tv;
    do
    {
        FD_ZERO(&rfds);
        FD_SET(sockfd, &rfds);
        tv.tv_sec = timeoutsec;
        tv.tv_usec = 0;
        int s = select(sockfd + 1, &rfds, NULL, NULL, &tv);
        if (s < 0)
        {
            printf("select error %X\n", errno);
            break;
        }
        if (s == 0)
        {
            printf("read timeout\n");
            break;
        }
        int n = read(sockfd, buffer + m, len - m - 1);
        if (n >= 0)
        {
            m = m + n;
            buffer[m] = 0;
            printf("data received %d\n", n);
        }
        if (n < 0)
        {
            if (errno == ENOTCONN)
            {
                printf("socket closed by server\n");
                break;
            }
            printf("read error %X\n", errno);
            break;
        }
        vTaskDelay(2);
    } while (true);
}

void asyncgetPage(void *arg)
{
    Page *page = (Page *)arg;

    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    struct addrinfo *servinfo;
    int status = getaddrinfo(page->url, page->port, &hints, &servinfo);
    if (status < 0)
        printf("dns fail\n");
    char bufIP[20];
    printf("IP address found:%s\n", esp_ip4addr_ntoa((esp_ip4_addr_t *)&(servinfo->ai_addr->sa_data[2]), bufIP, 20));

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) < 0)
    {
        printf("no connection %X\n", errno);
        printf("close socket %d\n", sockfd);
        close(sockfd);
        vTaskDelete(NULL);
    }
    freeaddrinfo(servinfo);

    char headerTemplate[] = "%s %s HTTP/1.1\r\n"
                            "HOST:%s:%s\r\n"
                            "Connection: close\r\n"
                            "Content-length: %d\r\n\r\n%s";
    int len = snprintf(NULL, 0, headerTemplate, page->req, page->path, page->url, page->port, page->lensenddata, page->senddata);
    char *requestData = malloc(len + 1);
    snprintf(requestData, len + 1, headerTemplate, page->req, page->path, page->url, page->port, page->lensenddata, page->senddata);
    int n = write(sockfd, requestData, len + 1);
    if (n < 0)
    {
        printf("no write %X\n", errno);
        printf("close socket %d\n", sockfd);
        close(sockfd);
        vTaskDelete(NULL);
    }
    else
        printf("data sent socket %d %d\n", sockfd, n);

    socketReadTimeout(sockfd, page->buffer, page->len, 10);
    printf("close socket %d\n", sockfd);
    close(sockfd);
    page->done = true;
    vTaskDelete(NULL);
}

void doPage(Page *page)
{
    TaskHandle_t asyncgetpage;
    xTaskCreate(asyncgetPage, "asyncgetPage", 1024 * 10, page, 0, &asyncgetpage);
    return;
}

void app_main(void)
{
    wifiConnect("Co", "SSID", "password", NULL, NULL);
    while (wifiStatus != 1010)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
    };

    char buffer1[4000];
    char url[] = "192.168.253.75";
    char path[] = "/";
    char senddata[] = {0xFF, 0x55, 0xF0, 0x0F, 0xFF};
    Page pagetemp1 = initPage("PUT", "80", url, path, buffer1, 4000, senddata, 5);
    doPage(&pagetemp1);

    while (true)
    {
        if (pagetemp1.done)
        {
            printf("\nrequest \n %s\n", buffer1);
            pagetemp1.done = false;
            break;
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    };
    vTaskDelete(NULL);
}
