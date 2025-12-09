#include <stdio.h>
#include "freertos/FreeRTOS.h"
StreamBufferHandle_t sb;
uint8_t count = 0;

void task1(void *arg)
{
    for (;;)
    {
        xStreamBufferSend(sb, &count, 1, 100);
        count++;
    }
}
void task2(void *arg)
{
    uint8_t data;
    for (;;)
    {
        xStreamBufferReceive(sb, &data, 1, 100);
        printf("%d %d\n", data, xStreamBufferSpacesAvailable(sb));
    }
}
void app_main(void)
{
    sb = xStreamBufferCreate(100, 50);
    xTaskCreatePinnedToCore(task1, "task1", 2048, NULL, 0, NULL, 1);
    xTaskCreatePinnedToCore(task2, "task2", 4048, NULL, 0, NULL, 0);
}
