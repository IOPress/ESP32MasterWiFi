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
    union
    {
        mydata_t data;
        uint8_t bdata[sizeof(mydata_t)];
    } mydata;

    for (;;)
    {
        size_t n = 0;
        int m = sizeof mydata;
        int offset = 0;
        while (m > 0)
        {
            pdata = xRingbufferReceiveUpTo(rb, &n, 100, m);
            memcpy(mydata.bdata + offset, pdata, n);
            m = m - n;
            offset = offset + n;
            vRingbufferReturnItem(rb, pdata);
        }
        printf("%ld %d\n", mydata.data.count, xRingbufferGetCurFreeSize(rb));
    }
}
void app_main(void)
{
    rb = xRingbufferCreate(100, RINGBUF_TYPE_BYTEBUF);
    xTaskCreatePinnedToCore(task1, "task1", 2048, NULL, 0, NULL, 1);
    xTaskCreatePinnedToCore(task2, "task2", 4048, NULL, 0, NULL, 0);
}
