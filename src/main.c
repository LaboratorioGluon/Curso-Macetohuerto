#include <freertos/FreeRTOS.h>
#include "comms.h"
#include "esp_log.h"
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
  pump_init(sysDevs->pumpGpio);

  SensorData data;
  for (;;) {

    printf("Pre sensor_update\n");
    sensors_update(&data);

    //printf("Read data: %.2f, %.2f, %.2f", data.bme.pressure, data.bme.humidty, data.bme.airTemp);
    printf("ADC: %2.4f | %2.4f\n", data.adcLdr, data.adcHumidity);
    //..

    comms_send();

    // Activate the pump if humidity is low.
    if (data.adcHumidity > 2.0f) {
      ESP_LOGI("PUMP", "Empezando a regar!");
      pump_actuate();
      ESP_LOGI("PUMP", "Terminando de regar!");
    }

    system_sleep();

    vTaskDelay(pdMS_TO_TICKS(200));
  }
}
