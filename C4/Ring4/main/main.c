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
        xRingbufferSend(rb, &mydata, sizeof mydata, 100);
        mydata.count++;
    }
}

void task2(void *arg)
{
    uint8_t *pdata;
    size_t n;
    for (;;)
    {
        pdata = xRingbufferReceive(rb, &n, 100);
        printf("%ld %d\n", ((mydata_t *)pdata)->count, xRingbufferGetCurFreeSize(rb));
        vRingbufferReturnItem(rb, pdata);
    }
}
void app_main(void)
{
    rb = xRingbufferCreate(100, RINGBUF_TYPE_NOSPLIT);
    xTaskCreatePinnedToCore(task1, "task1", 2048, NULL, 0, NULL, 1);
    xTaskCreatePinnedToCore(task2, "task2", 4048, NULL, 0, NULL, 0);
}
