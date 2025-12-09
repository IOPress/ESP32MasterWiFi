#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"

void task1(void* arg) {
    for (;;) {
        gpio_set_level(2, 1);
    }
}
void task2(void* arg) {
    for (;;) {
        gpio_set_level(2, 0);
    }
}

void app_main(void)
{
    gpio_reset_pin(2);
    gpio_set_direction(2, GPIO_MODE_OUTPUT);

    TaskHandle_t th1;
    xTaskCreatePinnedToCore(task1, "task1", 2048, NULL, 0, &th1, 1);
    TaskHandle_t th2;
    xTaskCreatePinnedToCore(task2, "task2", 2048, NULL, 0, &th2, 1);
}