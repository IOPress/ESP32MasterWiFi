#include <stdio.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "dhcpserver/dhcpserver.h"
#include "dhcpserver/dhcpserver_options.h"

#include "esp_http_server.h"
int retry_num = 0;

static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id)
    {
    case WIFI_EVENT_AP_STACONNECTED:
        printf("STA CONNECTING....\n");
        break;
    case WIFI_EVENT_AP_STADISCONNECTED:
        printf("STA DISCONNECTED\n");
        break;
    }
}

esp_err_t get_handler(httpd_req_t *req)
{
    printf("GET %s\n", req->uri);
    if (strcmp(req->uri, "/") == 0)
    {
        const char resp[] = "<!DOCTYPE html>"
                            "<html><head>""<meta http-equiv="
                            "content-type"" content=""text/html"" />"
                            "<title>Setup</title></head><body>"
                            "<form name=""Form"" action=""/"" method=""POST"
                            " enctype=""text/plain"">"
                            "<p>SSID of the AP</p>""<input name=""SSID"" style="
                            "width: 6cm; height: 1cm""></input>""<p>Password </p>"
                            "<input name=""Password"" style="
                            "width: 6cm; height: 1cm""></input>"
                            "</br></br>""<button>Ok</button>"
                            "</form></body></html>";
        httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    }
    else
    {
        httpd_resp_send(req, "", HTTPD_RESP_USE_STRLEN);
    }
    return ESP_OK;
}

int getField(char buffer[], char name[], char value[])
{
    char *myPtr1 = strstr(buffer, name);
    if (myPtr1 == NULL)
        return -1;
    char *myPtr2 = strstr(myPtr1, "\r\n");
    if (myPtr2 == NULL)
        return -2;
    int c = myPtr2 - myPtr1;
    strncpy(value, myPtr1, c);
    value[c] = 0;
    return 0;
}

esp_err_t post_handler(httpd_req_t *req)
{
    char buffer[50];
    int n = req->content_len;
    int i = 0;
    while (n != i)
    {
        int m = httpd_req_recv(req, buffer + i, n - i);
        i = i + m;
    }
    buffer[i] = 0;
    char ssid[20];
    char password[20];
    getField(buffer, "SSID=", ssid);

    getField(buffer, "Password=", password);
    printf("ssid\n %s\n", ssid);
    printf("password\n %s\n", password);

    char html[] = "<html><head><title>Thanks</title></head>"
                  "<body><p>The new WiFi connection will now be made</p>"
                  "</body></html>\r\n";
    httpd_resp_send_chunk(req, html, strlen(html));
    return ESP_OK;
}

void app_main(void)
{
    nvs_flash_init();
    esp_event_loop_create_default();
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);

    esp_netif_init();
    esp_netif_t *esp_netif = esp_netif_create_default_wifi_ap();

    wifi_init_config_t wificonfig = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wificonfig);

    wifi_config_t APconf = {
        .ap = {
            .ssid = "ESP32",
            .channel = 5,
            .authmode = WIFI_AUTH_OPEN,
            .max_connection = 2}};
    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(WIFI_IF_AP, &APconf);
    esp_wifi_start();

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    httpd_uri_t uri_get = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = get_handler,
        .user_ctx = NULL};

    httpd_uri_t uri_post = {
        .uri = "/",
        .method = HTTP_POST,
        .handler = post_handler,
        .user_ctx = NULL};
    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_post);
    }
    while (true)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    };
}