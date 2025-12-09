#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/ringbuf.h"
#include <string.h>

RingbufHandle_t rb;
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
        mydata_t *data = NULL;
        xRingbufferSendAcquire(rb, (void **)&data, sizeof(mydata_t), 100);    
        memcpy(data, &mydata, sizeof(mydata_t));
        xRingbufferSendComplete(rb, data);
        while (true)
        {
        };
    }
}

void task2(void *arg)
{
    uint8_t *pdata;
    size_t n;
    pdata = xRingbufferReceive(rb, &n, 100);
    printf("%ld %ld %ld   %d\n", ((mydata_t *)pdata)->pressure, ((mydata_t *)pdata)->temperature, ((mydata_t *)pdata)->count, xRingbufferGetCurFreeSize(rb));
    vRingbufferReturnItem(rb, pdata);
    for (;;)
    {
    }
}
void app_main(void)
{
    rb = xRingbufferCreate(100, RINGBUF_TYPE_NOSPLIT);
    xTaskCreatePinnedToCore(task1, "task1", 2048, NULL, 0, NULL, 1);
    xTaskCreatePinnedToCore(task2, "task2", 4048, NULL, 0, NULL, 0);
}
