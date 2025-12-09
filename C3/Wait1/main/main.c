#include <stdio.h>
#include "freertos/FreeRTOS.h"

SemaphoreHandle_t xSemaphore;

void task0(void *arg)
{
    vTaskDelay(100);
    printf("task0\n");
    xSemaphoreGive(xSemaphore);
    vTaskDelete(NULL);
}

void app_main()
{
    xSemaphore = xSemaphoreCreateBinary();
    xTaskCreatePinnedToCore(task0, "task0", 4000, NULL, 0, NULL, 1);
    xSemaphoreTake(xSemaphore, portMAX_DELAY);
    printf("task complete\n");
}
