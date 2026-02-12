#include <freertos/FreeRTOS.h>
#include "comms.h"
#include "pump.h"
#include "scanneri2c.h"
#include "sensors.h"
#include "system.h"

void app_main() {

  vTaskDelay(pdMS_TO_TICKS(3000));

  SystemDevs* sysDevs = system_init();

  SensorConfig sensorsconfig;
  sensorsconfig.bmeDev = sysDevs->bme;
  sensorsconfig.adsDev = sysDevs->ads;
  sensors_init(&sensorsconfig);

  SensorData data;
  for (;;) {

    printf("Pre sensor_update\n");
    sensors_update(&data);

    //printf("Read data: %.2f, %.2f, %.2f", data.bme.pressure, data.bme.humidty, data.bme.airTemp);
    printf("ADC: %.4f\n", data.adcValueV);
    //..

    comms_send();
    pump_actuate();

    system_sleep();

    vTaskDelay(pdMS_TO_TICKS(200));
  }
}
