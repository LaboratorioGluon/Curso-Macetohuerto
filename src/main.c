#include "sensors.h"
#include "system.h"

void app_main() {

  system_init();

  sensors_init();

  for (;;) {

    sensors_update();

    //..

    comms_sendData();
    pump_actuate();

    system_sleep();
  }
}
