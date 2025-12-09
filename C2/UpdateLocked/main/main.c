#include <stdio.h>
#include "freertos/FreeRTOS.h"

#include "esp_timer.h"
int64_t start_time_us;
int64_t end_time_us;

int64_t count = 0;
bool done0 = false;
bool done1 = false;
static portMUX_TYPE my_spinlock = portMUX_INITIALIZER_UNLOCKED;
void task0(void *arg)
{
    for (int i = 0; i < 0xFFFFF; i++)
    {
        taskENTER_CRITICAL(&my_spinlock);
        count = count + 1;
        taskEXIT_CRITICAL(&my_spinlock);
    }
    done0 = true;
    for (;;)
    {
    }
}
void task1(void *arg)
{
    for (int i = 0; i < 0xFFFFF; i++)
    {
        taskENTER_CRITICAL(&my_spinlock);
        count = count + 1;
        taskEXIT_CRITICAL(&my_spinlock);
    }
    done1 = true;
    for (;;)
    {
    }
}

void task2(void *arg)
{
    start_time_us = esp_timer_get_time();
    while (!done0 || !done1)
    {
        vTaskDelay(5);
    };

    end_time_us = esp_timer_get_time();
    int64_t time_difference_us = end_time_us - start_time_us;

    printf("Time difference: %.2f seconds\n", (double)time_difference_us / 1000000.0);
     printf("%llX\n", count);
    fflush(stdout);
    for (;;)
    {
    }
}

void app_main()
{
    xTaskCreatePinnedToCore(task0, "task0", 4000, NULL, 0, NULL, 1);
    xTaskCreatePinnedToCore(task1, "task1", 4000, NULL, 0, NULL, 0);
    xTaskCreatePinnedToCore(task2, "task2", 4000, NULL, 0, NULL, 1);
}
