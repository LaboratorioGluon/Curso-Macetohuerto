#ifndef IBME280_H__
#define IBME280_H__

#include "bme280.h"

BME280_INTF_RET_TYPE ibme280_i2c_read(uint8_t reg_addr, uint8_t* reg_data, uint32_t len,
                                      void* intf_ptr);

BME280_INTF_RET_TYPE ibme280_i2c_write(uint8_t reg_addr, const uint8_t* reg_data, uint32_t len,
                                       void* intf_ptr);

void ibme280_delay(uint32_t period, void* intf_ptr);

#endif  //IBME280_H__