#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_random.h"

SemaphoreHandle_t xSemaphore0;
SemaphoreHandle_t xSemaphore1;

void task0(void *arg)
{
    uint32_t delay = (esp_random() % 500) + 1;
    vTaskDelay(delay);
    printf("task0\n");
    xSemaphoreGive(xSemaphore0);
    vTaskDelete(NULL);
}

void task1(void *arg)
{
    uint32_t delay = (esp_random() % 500) + 1;
    vTaskDelay(delay);
    printf("task1\n");
    xSemaphoreGive(xSemaphore1);
    vTaskDelete(NULL);
}

void app_main()
{
    xSemaphore0 = xSemaphoreCreateBinary();
    xSemaphoreTake(xSemaphore0, 1);
    xSemaphore1 = xSemaphoreCreateBinary();
    xSemaphoreTake(xSemaphore1, 1);

    xTaskCreatePinnedToCore(task0, "task0", 4000, NULL, 0, NULL, 1);
    xTaskCreatePinnedToCore(task1, "task1", 4000, NULL, 0, NULL, 0); 

    QueueSetHandle_t xQueueSet = xQueueCreateSet(2);
    xQueueAddToSet(xSemaphore0, xQueueSet);
    xQueueAddToSet(xSemaphore1, xQueueSet);
    QueueSetMemberHandle_t xActivatedMember = xQueueSelectFromSet(xQueueSet, portMAX_DELAY);

    if (xActivatedMember == xSemaphore0)
    {
        printf("Task0 Completed first\n");
    }
    if (xActivatedMember == xSemaphore1)
    {
        printf("Task1 Completed first\n");
    }
    printf("app main task complete\n");
}
