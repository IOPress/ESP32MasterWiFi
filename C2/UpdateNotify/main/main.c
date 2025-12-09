#include <stdio.h>
#include "freertos/FreeRTOS.h"

#include "esp_timer.h"
#include "driver/gpio.h"
int64_t start_time_us;
int64_t end_time_us;

int64_t count = 0;
bool done0 = false;
bool done1 = false;

TaskHandle_t th0, th1;
void task0(void *arg)
{
    for (int i = 0; i < 0xFFFFF; i++)
    {
        ulTaskNotifyTakeIndexed(0, true, 1000);
        count = count + 1;
        gpio_set_level(1, 1);
        xTaskNotifyGiveIndexed(th1, 0);
    }

    done0 = true;
    for (;;)
    {
          vTaskDelay(1000);
    }
}
void task1(void *arg)
{
    for (int i = 0; i < 0xFFFFF; i++)
    {
        ulTaskNotifyTakeIndexed(0, true, 1000);
        count = count + 1;
        gpio_set_level(1, 0);
        xTaskNotifyGiveIndexed(th0, 0);
    }

    done1 = true;
    for (;;)
    {
        vTaskDelay(1000);
    }
}

void task2(void *arg)
{
    start_time_us = esp_timer_get_time();
    while (!done0 || !done1)
    {
        vTaskDelay(1);
    };
    printf("%llX\n", count);
    end_time_us = esp_timer_get_time();
    int64_t time_difference_us = end_time_us - start_time_us;

    printf("Time difference: %.2f seconds\n", (double)time_difference_us / 1000000.0);
    fflush(stdout);
    for (;;)
    {
       vTaskDelay(1000); 
    }
}

void app_main()
{
    gpio_reset_pin(1);
    gpio_set_direction(1, GPIO_MODE_INPUT_OUTPUT);
    gpio_reset_pin(2);
    gpio_set_direction(2, GPIO_MODE_INPUT_OUTPUT);

    xTaskCreatePinnedToCore(task0, "task0", 4000, NULL, 1, &th0, 1);
    xTaskCreatePinnedToCore(task1, "task1", 4000, NULL, 1, &th1, 0);
    xTaskCreatePinnedToCore(task2, "task2", 4000, NULL, 0, NULL, 1);
    xTaskNotifyGiveIndexed(th0, 0);
}
