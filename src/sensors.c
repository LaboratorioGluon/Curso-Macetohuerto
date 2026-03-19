#include "sensors.h"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include "IBme280.h"
#include "ads1115.h"
#include "bme280.h"
#include "hal/ads1115_hals.h"
#include "hx711.h"

/*********************/
/* Private Variables */

static struct {
  struct bme280_dev bmedev;
  Ads1115 ads;
  Hx711 hx711;
} sensors;

static char* TAG = "SENSORS";

#define HUMIDITY_GAIN   -63.37
#define HUMIDITY_OFFSET 164.76

#define HX711_NUM_SAMPLES 10

#define CALIBRATION_HX711_SAMPLES 100U
#define CALIBRATION_HX711_OFFSET  (276709)
#define HX711_GAIN                (952.35955e-6)
#define HX711_OFFSET              (-37.4651e-3)

/*********************/
/* Public Interfaces */

void sensors_init(SensorConfig* config) {

  // Prepare 'bmedev' device.
  sensors.bmedev.read     = ibme280_i2c_read;
  sensors.bmedev.write    = ibme280_i2c_write;
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

  ads1115_esp32Create(&sensors.ads, &config->adsDev);

  Ads1115Config adsConf = {
      .mode = ADS1115_MODE_CONTINUOUS,
      .fsr  = ADS1115_FSR_4_096V,
      .dr   = ADS1115_DR_128SPS,
      .mux  = ADS1115_MUX_AIN0_GND,
  };

  ads1115_config(&sensors.ads, &adsConf);

  sensors.hx711.ch       = HX711_CHA_128;
  sensors.hx711.gpioSck  = config->hx711Sck;
  sensors.hx711.gpioData = config->hx711Data;
  hx711_init(&sensors.hx711);
  hx711_setTare(&sensors.hx711, CALIBRATION_HX711_OFFSET);
  hx711_setScaleOffset(&sensors.hx711, HX711_GAIN, HX711_OFFSET);
}

void sensors_update(SensorData* data) {
  struct bme280_data bmedata;
  bme280_get_sensor_data(BME280_ALL, &bmedata, &sensors.bmedev);

  data->bme.pressure = bmedata.pressure;
  data->bme.humidty  = bmedata.humidity;
  data->bme.airTemp  = bmedata.temperature;

  //data->adcValueMv = ads1115_readRaw(&sensors.ads) * 125.0f / 1000.0f;
  ads1115_setMux(&sensors.ads, ADS1115_MUX_AIN0_GND);
  vTaskDelay(pdMS_TO_TICKS(30));
  data->adcLdr = ads1115_readVolts(&sensors.ads);

  ads1115_setMux(&sensors.ads, ADS1115_MUX_AIN1_GND);
  vTaskDelay(pdMS_TO_TICKS(30));
  //data->adcHumidity = HUMIDITY_GAIN * ads1115_readVolts(&sensors.ads) + HUMIDITY_OFFSET;

  // Recalibrate when the project is finished and installed.
  data->adcHumidity = ads1115_readVolts(&sensors.ads);

  float grams    = 0.0f;
  float sumGrams = 0.0f;
  for (uint32_t i = 0; i < HX711_NUM_SAMPLES; i++) {
    hx711_readValueScaled(&sensors.hx711, &grams);
    sumGrams += grams;
  }
  data->grams = sumGrams / HX711_NUM_SAMPLES;
}

void sensors_calibrate() {
  int32_t value     = 0;
  int32_t sumValues = 0;
  for (uint32_t i = 0; i < CALIBRATION_HX711_SAMPLES; i++) {
    hx711_readValueRaw(&sensors.hx711, &value);
    sumValues += value;
  }
  ESP_LOGE("CALIBRATION", "HX711 mean value: %d", sumValues / CALIBRATION_HX711_SAMPLES);
}