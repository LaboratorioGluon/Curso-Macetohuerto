#ifndef SYSTEM_H__
#define SYSTEM_H__

#include <driver/i2c_master.h>

/**
 * @brief Stores the handles required by the 
 * rest of the software
 */
typedef struct {
  i2c_master_dev_handle_t bme;
  i2c_master_dev_handle_t ads;
} SystemDevs;

/**
 * @brief Initialize chip internal peripherals.
 * @return Pointer to required handles. Used by the sensor manager to
 *         connect the sensor to the peripheral.
 */
SystemDevs* system_init(void);

/**
 * @brief TODO
 * @param  TODO
 */
void system_sleep(void);

#endif  //SYSTEM_H__