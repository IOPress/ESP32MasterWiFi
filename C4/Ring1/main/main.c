#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/ringbuf.h"

RingbufHandle_t rb;

void task1(void *arg)
{
    uint8_t count;
    for (;;)
    {
        xRingbufferSend(rb, &count, 1, 100);
        count++;
    }
}

void task2(void *arg)
{
    for (;;)
    {
        uint8_t *count;
        size_t n;
        count = xRingbufferReceiveUpTo(rb, &n, 100, 1);
        printf("%d %d\n", *count, xRingbufferGetCurFreeSize(rb));
        vRingbufferReturnItem(rb, count);
    }
}
void app_main(void)
{
    rb = xRingbufferCreate(100, RINGBUF_TYPE_BYTEBUF);
    xTaskCreatePinnedToCore(task1, "task1", 2048, NULL, 0, NULL, 1);
    xTaskCreatePinnedToCore(task2, "task2", 4048, NULL, 0, NULL, 0);
}
