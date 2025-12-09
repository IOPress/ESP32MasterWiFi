#include <stdio.h>
#include "freertos/FreeRTOS.h"

StreamBufferHandle_t sb;

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
        xStreamBufferSend(sb, (uint8_t *)&mydata, sizeof mydata, 100);
        mydata.count++;
    }
}
void task2(void *arg)
{
    mydata_t data;
    for (;;)
    {
        int n = 0;
        int m = sizeof data;
        int offset = 0;
        while (n < sizeof data)
        {
            n = xStreamBufferReceive(sb, (uint8_t*)&data + offset, m, 100);
            m = m - n;
            offset = offset + n;
        }
        printf("%ld %d\n", data.count, xStreamBufferSpacesAvailable(sb));
    }
}
void app_main(void)
{
    sb = xStreamBufferCreate(sizeof mydata * 50, sizeof mydata);
    xTaskCreatePinnedToCore(task1, "task1", 2048, NULL, 0, NULL, 1);
    xTaskCreatePinnedToCore(task2, "task2", 4048, NULL, 0, NULL, 0);
}
