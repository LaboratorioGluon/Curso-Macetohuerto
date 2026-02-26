#ifndef SENSORS_C__
#define SENSORS_C__

#include <driver/i2c_master.h>

/**
 * @brief Configuration required to initialize the sensors.
 */
typedef struct {
  i2c_master_dev_handle_t bmeDev;
  i2c_master_dev_handle_t adsDev;
} SensorConfig;

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

#endif  //SENSORS_C__