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
    char *url;
    char *path;
    char *buffer;
    int len;
    bool done;
} Page;

Page initPage(char url[], char path[], char buffer[], int len)
{
    Page page;
    page.url = url;
    page.path = path;
    page.buffer = buffer;
    page.len = len;
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
            printf("read error %X on socket %d\n", errno, sockfd);
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
            printf("\ndata received %d\n\n", n);
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
    int status = getaddrinfo(page->url, "80", &hints, &servinfo);
    if (status < 0)
        printf("dns fail\n");

    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

    if (connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) < 0)
    {
        printf("no connection %X\n", errno);
        printf("close socket %d\n", sockfd);
        close(sockfd);
        vTaskDelete(NULL);
    }
    freeaddrinfo(servinfo);

    char header[100];
    sprintf(header, "GET %s HTTP/1.1\r\nHost:%s\r\n\r\n ", page->path, page->url);
    int n = write(sockfd, header, strlen(header));
    if (n < 0)
    {
        printf("no write %X\n", errno);
        printf("close socket %d\n", sockfd);
        close(sockfd);
        vTaskDelete(NULL);
    }
    else
        printf("data sent socket %d %d\n", sockfd, n);

    socketReadTimeout(sockfd, page->buffer, page->len, 4);
    printf("close socket %d\n", sockfd);
    close(sockfd);
    page->done = true;
    vTaskDelete(NULL);
}

void getPage(Page *page)
{
    TaskHandle_t asyncgetpage;
    xTaskCreate(asyncgetPage, "asyncgetPage", 1024 * 4, page, 0, &asyncgetpage);
    return;
}

void app_main(void)
{
    wifiConnect("Co", "SSID", "password", NULL, NULL);
    while (wifiStatus != 1010)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
    };

    char buffer1[2000];
    char url[] = "example.com";
    char path[] = "/index.html";
    Page pagetemp1 = initPage(url, path, buffer1, 2000);
    getPage(&pagetemp1);

    char buffer2[2000];
    Page pagetemp2 = initPage("example.com", "/", buffer2, 2000);
    getPage(&pagetemp2);
    while (true)
    {
        if (pagetemp1.done)
        {
            printf("\n*****************first request \n %s\n", buffer1);
            pagetemp1.done = false;
        }

        if (pagetemp2.done)
        {
            printf("\n*****************second request \n %s\n", buffer2);
            pagetemp2.done = false;
        }
        vTaskDelay(100);
    };
}
