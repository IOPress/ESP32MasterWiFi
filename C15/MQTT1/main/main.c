#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "string.h"

#include "mqtt_client.h"
#include "esp_event.h"

#include "connectWiFi.h"

bool mqttOnline = false;
void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        mqttOnline = true;
        printf("connected\n");
        break;
    case MQTT_EVENT_DISCONNECTED:
        mqttOnline = false;
        printf("disconnected\n");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        printf("subscribed\n");
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        printf("unsubscribed\n");
        break;
    case MQTT_EVENT_PUBLISHED:
        printf("PUB, msg_id=%d\n", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        printf("DATA  msg_id=%d\n", event->msg_id);
        printf("    TOPIC=%.*s\n", event->topic_len, event->topic);
        printf("     DATA=%.*s\n", event->data_len, event->data);
        break;
    case MQTT_EVENT_ERROR:
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
        {
            printf("Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    default:
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

    esp_event_loop_create_default();
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.hostname = "test.mosquitto.org",
        .broker.address.port = 1883,
        .broker.address.transport = MQTT_TRANSPORT_OVER_TCP};

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);

    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);

    esp_mqtt_client_start(client);
    while (!mqttOnline)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    esp_mqtt_client_subscribe(client, "MyTopic", 1);
    while (true)
    {
        esp_mqtt_client_publish(client, "MyTopic", "Hello MQTT World", 0, 1, 0);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}