#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "string.h"

#include "socket.h"
#include "connectWiFi.h"

void asyncServePage(void *arg)
{
    int client_fd = (int)arg;

    char buffer[2048];
    int n = read(client_fd, buffer, 2048);
    buffer[n] = 0;
    printf("%s", buffer);

    char headers[] = "HTTP/1.1 200 OK\r\n"
                     "Content-Type:text/html;"
                     "charset=UTF-8\r\n"
                     "Server:ESP32\r\n"
                     "Content-Length:";

    char html[] = "<html><head>"
                  "<title>Hello HTTP World</title>"
                  "</head><body>"
                  "<p>Hello HTTP World</p>"
                  "</body></html>\r\n";
    char data[2048] = {0};
    snprintf(data, sizeof data, "%s%d\r\n\r\n%s",
             headers, strlen(html), html);

    n = write(client_fd, data, strlen(data));
    printf("data sent \n");
    close(client_fd);
    vTaskDelete(NULL);
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
    addr.sin_port = htons(80);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    listen(sockfd, 3);
    struct sockaddr_storage client_addr;
    socklen_t addr_size = sizeof client_addr;
    while (true)
    {
        int client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size);
        xTaskCreate(asyncServePage, "asyncServePage", 1024 * 10, (void *)client_fd, 1, NULL);
    };
}
