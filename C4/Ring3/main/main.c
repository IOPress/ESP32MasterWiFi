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
    uint8_t *pdatah, *pdatat;
    union
    {
        mydata_t data;
        uint8_t bdata[sizeof(mydata_t)];
    } mydata;

    for (;;)
    {
        size_t nh = 0;
        size_t nt = 0;
        xRingbufferReceiveSplit(rb, (void **)&pdatah, (void **)&pdatat, &nh, &nt, 100);
        if (pdatah != NULL)
        {
            memcpy(mydata.bdata, pdatah, nh);
            vRingbufferReturnItem(rb, pdatah);
        }

        if (pdatat != NULL)
        {
            memcpy(mydata.bdata + nh, pdatat, nt);
            vRingbufferReturnItem(rb, pdatat);
        }
        printf("%ld %d\n", mydata.data.count, xRingbufferGetCurFreeSize(rb));
    }
}
void app_main(void)
{
    rb = xRingbufferCreate(100, RINGBUF_TYPE_ALLOWSPLIT);
    xTaskCreatePinnedToCore(task1, "task1", 2048, NULL, 0, NULL, 1);
    xTaskCreatePinnedToCore(task2, "task2", 4048, NULL, 0, NULL, 0);
}
