#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"

void task1(void *arg)
{
   for (;;)
   {
      gpio_set_level(2, 1);
   }
}
void task2(void *arg)
{
   for (;;)
   {
      gpio_set_level(2, 0);
   }
}

void app_main(void)
{
   gpio_reset_pin(2);
   gpio_set_direction(2, GPIO_MODE_OUTPUT);

   xTaskCreatePinnedToCore(task1, "task1", 2048, NULL, 1, NULL, 1);
   xTaskCreatePinnedToCore(task2, "task2", 2048, NULL, 1, NULL, 1);
}