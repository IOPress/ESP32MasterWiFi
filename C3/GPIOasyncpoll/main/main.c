#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"

void task0(void *arg)
{
    while (true)
    {
        while (gpio_get_level(4) == 0)
        {
            gpio_set_level(1, !gpio_get_level(1));
            vTaskDelay(5);
        }
        gpio_set_level(2, !gpio_get_level(2));
        while (gpio_get_level(4) == 1)
        {
            gpio_set_level(1, !gpio_get_level(1));
            vTaskDelay(5);
        }
    }
}
void task1(void *arg)
{
    while (true)
    {
    };
}
void app_main()
{

    gpio_reset_pin(1);
    gpio_set_direction(1, GPIO_MODE_INPUT_OUTPUT);
    gpio_reset_pin(2);
    gpio_set_direction(2, GPIO_MODE_INPUT_OUTPUT);
    gpio_reset_pin(4);
    gpio_set_direction(4, GPIO_MODE_INPUT);
    gpio_pullup_en(4);
    xTaskCreatePinnedToCore(task0, "task0", 4000, NULL, 2, NULL, 0);
    xTaskCreatePinnedToCore(task1, "task1", 4000, NULL, 0, NULL, 0);
    xTaskCreatePinnedToCore(task1, "task2", 4000, NULL, 0, NULL, 0);
    xTaskCreatePinnedToCore(task1, "task3", 4000, NULL, 0, NULL, 0);
    printf("task complete\n");
}
