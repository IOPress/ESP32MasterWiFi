#include <stdio.h>
#include "freertos/FreeRTOS.h"

MessageBufferHandle_t mb;

typedef struct
{
    int32_t temperature;
    int32_t pressure;
    int32_t count;
} mydata_t;

mydata_t mydata = {20, 75, 0};

void task1(void *arg)
{
    for (;;)
    {
        xMessageBufferSend(mb, &mydata, sizeof mydata, 100);
        mydata.count++;
    }
}
void task2(void *arg)
{
    mydata_t data;
    for (;;)
    {
        int n = xMessageBufferReceive(mb, &data, sizeof data, 100);
        printf("%ld %d\n", data.count, xMessageBufferSpacesAvailable(mb));
    }
}
void app_main(void)
{
    mb = xMessageBufferCreate(500);
    xTaskCreatePinnedToCore(task1, "task1", 2048, NULL, 0, NULL, 1);
    xTaskCreatePinnedToCore(task2, "task2", 4048, NULL, 0, NULL, 0);
}
