#include <stdio.h>
#include "freertos/FreeRTOS.h"

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
   // xTaskCreate(task0, "task0", 2048,  NULL, 0, NULL);
   xTaskCreatePinnedToCore(task0, "task0", 2048, NULL, 0, NULL, PRO_CPU_NUM);
   for (;;)
   {
      printf("app main\n");
      vTaskDelay(100);
   }
}