#include <stdio.h>
#include "freertos/FreeRTOS.h"
uint64_t flag1 = 0;
uint64_t flag2 = 0;

void task1(void* arg) {
    for (;;) {
        flag1 = 0xFFFFFFFFFFFFFFFF;
        flag2 = 0xFFFFFFFFFFFFFFFF;
        if (flag1 != flag2) {
            printf("task 1 %llX   %llX\n", flag1, flag2);
            fflush(stdout);
            vTaskDelay(100);
        }
    }
}
void task2(void* arg) {
    for (;;) {
        flag1 = 0x0;
        flag2 = 0x0;
        if (flag1 != flag2) {
            printf("task 2 %llX   %llX\n", flag1, flag1);
            fflush(stdout);
             vTaskDelay(100);
        }
    }
}
void app_main(void)
{
    xTaskCreatePinnedToCore(task1, "task1", 4048, NULL, 0, NULL, 0);
    xTaskCreatePinnedToCore(task2, "task2", 4048, NULL, 0, NULL, 0);
}