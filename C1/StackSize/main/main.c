#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void task0(void *arg)
{
   for (;;)
   {
      printf("Hello Task World\n");
      vTaskDelay(100);
   }
}

void app_main(void)
{
   TaskHandle_t th0;
   xTaskCreatePinnedToCore(task0, "task0", 2048, NULL, 0, &th0, PRO_CPU_NUM);

   vTaskDelay(1000 / portTICK_PERIOD_MS);
   TaskStatus_t TaskStatus;
   vTaskGetInfo(th0, &TaskStatus, true, true);
   printf("%ld\n", TaskStatus.usStackHighWaterMark);
   for (;;)
   {
      printf("app main\n");
      vTaskDelay(100);
   }
}