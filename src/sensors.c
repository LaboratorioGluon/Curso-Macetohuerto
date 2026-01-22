#include "sensors.h"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include "IBme280.h"
#include "bme280.h"

struct {
  struct bme280_dev bmedev;
} sensors;

static char* TAG = "SENSORS";

void sensors_init(SensorConfig* config) {

  sensors.bmedev.read = ibme280_i2c_read;
  sensors.bmedev.write = ibme280_i2c_write;
  sensors.bmedev.delay_us = ibme280_delay;
  sensors.bmedev.intf_ptr = &(config->bmeDev);
  sensors.bmedev.intf = BME280_I2C_INTF;
  if (bme280_init(&sensors.bmedev) == 0) {
    ESP_LOGE(TAG, "BME280 Initialized!");
  } else {
    while (1) {
      ESP_LOGE(TAG, "BME280 ERROR!");
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
  }

  struct bme280_settings settings;
  bme280_get_sensor_settings(&settings, &sensors.bmedev);
  settings.osr_h = 5;
  settings.osr_t = 5;
  settings.osr_p = 5;
  bme280_set_sensor_settings(BME280_SEL_ALL_SETTINGS, &settings, &sensors.bmedev);
  bme280_set_sensor_mode(BME280_POWERMODE_NORMAL, &sensors.bmedev);
}

void sensors_update(SensorData* data) {
  struct bme280_data bmedata;
  bme280_get_sensor_data(BME280_ALL, &bmedata, &sensors.bmedev);

  data->bme.pressure = bmedata.pressure;
  data->bme.humidty = bmedata.humidity;
  data->bme.airTemp = bmedata.temperature;
}