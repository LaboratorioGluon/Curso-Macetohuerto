#include <freertos/FreeRTOS.h>
#include "comms.h"
#include "pump.h"
#include "scanneri2c.h"
#include "sensors.h"
#include "system.h"

void app_main() {

  while (1) {
    scanner_scan();
    vTaskDelay(100);
  }

  system_init();

  sensors_init();

  for (;;) {

    sensors_update();

    //..

    comms_send();
    pump_actuate();

    system_sleep();
  }
}
