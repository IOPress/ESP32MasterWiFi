#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
SemaphoreHandle_t xSemaphore;

void ISR4(void *arg)
{
    gpio_intr_disable(4);
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    gpio_intr_enable(4);
}

void task0(void *arg)
{
    while (true)
    {
        xSemaphoreTake(xSemaphore, portMAX_DELAY);
        gpio_set_level(2, !gpio_get_level(2));
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
    vSemaphoreCreateBinary(xSemaphore);
    xSemaphoreTake(xSemaphore, 1);
    gpio_install_isr_service(ESP_INTR_FLAG_LOWMED | ESP_INTR_FLAG_EDGE);

    gpio_reset_pin(2);
    gpio_set_direction(2, GPIO_MODE_INPUT_OUTPUT);

    gpio_reset_pin(4);
    gpio_set_direction(4, GPIO_MODE_INPUT);
    gpio_pullup_en(4);
    gpio_intr_disable(4);
    gpio_set_intr_type(4, GPIO_INTR_POSEDGE);
    gpio_isr_handler_add(4, ISR4, NULL);

    xTaskCreatePinnedToCore(task0, "task0", 4000, NULL, 2, NULL, 0);
    xTaskCreatePinnedToCore(task1, "task1", 4000, NULL, 0, NULL, 0);
    xTaskCreatePinnedToCore(task1, "task2", 4000, NULL, 0, NULL, 0);
    xTaskCreatePinnedToCore(task1, "task3", 4000, NULL, 0, NULL, 0);
    gpio_intr_enable(4);
    printf("task complete\n");
}
