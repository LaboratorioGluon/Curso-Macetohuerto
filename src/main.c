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

TaskHandle_t hTaskSensors;
TaskHandle_t hTaskComms;
QueueHandle_t hqSensorToComms;
SemaphoreHandle_t hSempahore;

typedef struct {
  uint8_t counter;
  int8_t decounter;
} SensorToCommsData;

void taskSensor(void* arg) {
  SensorToCommsData data;
  data.counter   = 0;
  data.decounter = 100;
  printf("[SENSORS] Initializing sensors\n");
  xSemaphoreTake(hSempahore, portMAX_DELAY);
  while (1) {
    printf("[SENSORS] Reading sensors\n");
    data.counter++;
    data.decounter--;
    xQueueSend(hqSensorToComms, &data, 10);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void taskComms(void* arg) {
  printf("[COMMS] Initializing comms\n");
  SensorToCommsData commData;
  while (1) {
    xQueueReceive(hqSensorToComms, &commData, portMAX_DELAY);
    printf("[COMMS] Sending data...\n");
  }
}

void app_main() {

  hqSensorToComms = xQueueCreate(1, sizeof(SensorToCommsData));
  hSempahore      = xSemaphoreCreateBinary();

  xTaskCreate(taskSensor, "TaskSensors", 4096, NULL, 1, &hTaskSensors);
  xTaskCreate(taskComms, "TaskComms", 4096, NULL, 1, &hTaskComms);
  return;
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
      start = esp_timer_get_time();
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
