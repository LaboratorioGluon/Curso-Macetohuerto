#include <freertos/FreeRTOS.h>
#include <socket.h>
#include "comms.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "hx711.h"
#include "pump.h"
#include "scanneri2c.h"
#include "sensors.h"
#include "system.h"

#define CALIBRATION (1U)

void app_main() {

  vTaskDelay(pdMS_TO_TICKS(3000));

  SystemDevs* sysDevs = system_init();

  SensorConfig sensorsconfig;
  sensorsconfig.bmeDev    = sysDevs->bme;
  sensorsconfig.adsDev    = sysDevs->ads;
  sensorsconfig.hx711Data = sysDevs->hx711Data;
  sensorsconfig.hx711Sck  = sysDevs->hx711Sck;

  sensors_init(&sensorsconfig);
  pump_init(sysDevs->pumpGpio);
  comms_init();

#if CALIBRATION
  sensors_calibrate();
#endif

  SensorData data;
  int64_t start = esp_timer_get_time();
  for (;;) {

    printf("Pre sensor_update\n");
    sensors_update(&data);

    //printf("Read data: %.2f, %.2f, %.2f", data.bme.pressure, data.bme.humidty, data.bme.airTemp);
    printf("ADC: %2.4f | %2.4f | %.2f\n", data.adcLdr, data.adcHumidity, data.grams);
    //..

    if (esp_timer_get_time() - start > 10000000) {
      comms_send(&data);
    }

    // Activate the pump if humidity is low.
    /*if (data.adcHumidity > 2.0f) {
      ESP_LOGI("PUMP", "Empezando a regar!");
      pump_actuate();
      ESP_LOGI("PUMP", "Terminando de regar!");
    }*/

    system_sleep();

    vTaskDelay(pdMS_TO_TICKS(200));
  }
}
