#include "pump.h"

#include <freertos/FreeRTOS.h>

static gpio_num_t g_pumpGpio;

void pump_init(gpio_num_t pumpgpio) {
  g_pumpGpio = pumpgpio;
}

void pump_actuate() {
  gpio_set_level(g_pumpGpio, 1);
  vTaskDelay(pdMS_TO_TICKS(3000));
  gpio_set_level(g_pumpGpio, 0);
}