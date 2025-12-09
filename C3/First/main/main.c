#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_random.h"

SemaphoreHandle_t xSemaphore;
void task0(void *arg)
{
    uint32_t delay = (esp_random() % 500) + 1;
    vTaskDelay(delay);
    printf("task0\n");
    xSemaphoreGive(xSemaphore);
    vTaskDelete(NULL);
}

void task1(void *arg)
{
    uint32_t delay = (esp_random() % 500) + 1;
    vTaskDelay(delay);
    printf("task1\n");
    xSemaphoreGive(xSemaphore);
    vTaskDelete(NULL);
}

void app_main()
{
    xSemaphore=xSemaphoreCreateBinary();
    xTaskCreatePinnedToCore(task0, "task0", 4000, NULL, 0, NULL, 1);
    xTaskCreatePinnedToCore(task1, "task1", 4000, NULL, 0, NULL, 0);
    xSemaphoreTake(xSemaphore, portMAX_DELAY);
    printf("task complete\n");
}
