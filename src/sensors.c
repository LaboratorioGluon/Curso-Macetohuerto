#include "sensors.h"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include "IBme280.h"
#include "bme280.h"

/*********************/
/* Private Variables */

static struct {
  struct bme280_dev bmedev;
} sensors;

static char* TAG = "SENSORS";

/*********************/
/* Public Interfaces */

void sensors_init(SensorConfig* config) {

  // Prepare 'bmedev' device.
  sensors.bmedev.read = ibme280_i2c_read;
  sensors.bmedev.write = ibme280_i2c_write;
  sensors.bmedev.delay_us = ibme280_delay;

  // The interface implemented for the BME280 (IBme280.h/c)
  // requires the handle, so we pass it using this variable.
  sensors.bmedev.intf_ptr = &(config->bmeDev);

  // The Bme280 support I2C or SPI, select I2C.
  sensors.bmedev.intf = BME280_I2C_INTF;

  if (bme280_init(&sensors.bmedev) == 0) {
    ESP_LOGE(TAG, "BME280 Initialized!");
  } else {
    while (1) {
      ESP_LOGE(TAG, "BME280 ERROR!");
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
  }

  // Configure the BME280 settings.
  // Required to start the sensor measurements.
  struct bme280_settings settings;
  bme280_get_sensor_settings(&settings, &sensors.bmedev);

  // The OSR (Oversampling Rate) shall be more than 0.
  // 0 -> Sensor off
  // Othervalue -> Check datasheet.
  settings.osr_h = BME280_OVERSAMPLING_16X;
  settings.osr_t = BME280_OVERSAMPLING_16X;
  settings.osr_p = BME280_OVERSAMPLING_16X;

  // Configure the sensors.
  bme280_set_sensor_settings(BME280_SEL_ALL_SETTINGS, &settings, &sensors.bmedev);

  // Start the measurements.
  bme280_set_sensor_mode(BME280_POWERMODE_NORMAL, &sensors.bmedev);
}

void sensors_update(SensorData* data) {
  struct bme280_data bmedata;
  bme280_get_sensor_data(BME280_ALL, &bmedata, &sensors.bmedev);

  data->bme.pressure = bmedata.pressure;
  data->bme.humidty = bmedata.humidity;
  data->bme.airTemp = bmedata.temperature;
}