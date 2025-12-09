#include <stdio.h>
#include "freertos/FreeRTOS.h"


void task0(void *arg)
{
    vTaskDelay(100);
    printf("task0\n");
    xTaskNotifyGiveIndexed(*(TaskHandle_t*)arg, 0);
    vTaskDelete(NULL);
}

void app_main()
{
    TaskHandle_t main = xTaskGetCurrentTaskHandle();
    ulTaskNotifyTakeIndexed(0, true, 1);
    xTaskCreatePinnedToCore(task0, "task0", 4000, &main, 0,NULL, 1);
    ulTaskNotifyTakeIndexed(0, true, portMAX_DELAY);
    printf("task complete\n");
}
