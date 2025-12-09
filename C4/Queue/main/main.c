#include <stdio.h>
#include "freertos/FreeRTOS.h"
QueueHandle_t q;
int64_t count = 0;

void task1(void *arg)
{
    for (;;)
    {
        xQueueSendToBack(q, &count, 2);
        count++;
        vTaskDelay(1);
    }
}
void task2(void *arg)
{
    int64_t data;
    for (;;)
    {
        xQueueReceive(q, &data, 20);
        printf("%llX %d\n", data, uxQueueSpacesAvailable(q));
    }
}
void app_main(void)
{
    q = xQueueCreate(100, sizeof(int64_t));
    xTaskCreatePinnedToCore(task1, "task1", 2048, NULL, 0, NULL, 1);
    xTaskCreatePinnedToCore(task2, "task2", 4048, NULL, 0, NULL, 0);
}
