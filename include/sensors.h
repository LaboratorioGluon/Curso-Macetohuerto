#ifndef SENSORS_C__
#define SENSORS_C__

#include <driver/i2c_master.h>

#include "ads1115.h"
#include "bme280.h"
#include "hx711.h"

/**
 * @brief Configuration required to initialize the sensors.
 */
typedef struct {
    i2c_master_dev_handle_t bmeDev;
    i2c_master_dev_handle_t adsDev;
    gpio_num_t hx711Sck;
    gpio_num_t hx711Data;
} SensorConfig;

typedef struct {
    struct {
        float tare;
        float gain;
        float offset;
    } weight;
    struct {
        uint16_t HumMin;
        uint16_t HumMax;
    } soil;
} SensorCalibration;

typedef struct {
    struct bme280_dev bmedev;
    Ads1115 ads;
    Hx711 hx711;
} SensorHandlers;

/**
 * @brief Stores the read data from the sensors.
 */
typedef struct {
    struct {
        float pressure;
        float humidty;
        float airTemp;
    } bme;
    float adcLdr;
    float adcHumidity;
    float grams;
    float vSolar;
    float vBatt;
} SensorData;

/**
 * @brief Initialize the board sensors using the configuration.
 * @param config Required configuration by the sensor module.
 */
void sensors_init(SensorConfig* config);

/**
 * @brief Read the sensors and update the 'data' parameter.
 * @param data Pointer where to store the data.
 */
void sensors_update(SensorData* data);

void sensors_calibrate();

SensorHandlers* sensors_getSensors();

#endif  //SENSORS_C__