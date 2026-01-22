#include "IBme280.h"

#include <driver/i2c_master.h>
#include <string.h>

#include <rom/ets_sys.h>

#define SEND_BUF_SIZE 10
#define SEND_TIMEOUT_MS 100

uint8_t sendBuf[SEND_BUF_SIZE] = {0};

BME280_INTF_RET_TYPE ibme280_i2c_read(uint8_t reg_addr, uint8_t* reg_data, uint32_t len,
                                      void* intf_ptr) {

  i2c_master_dev_handle_t i2cDev = *(i2c_master_dev_handle_t*)intf_ptr;
  sendBuf[0] = reg_addr;
  return i2c_master_transmit_receive(i2cDev, sendBuf, 1, reg_data, len, SEND_TIMEOUT_MS);
}

BME280_INTF_RET_TYPE ibme280_i2c_write(uint8_t reg_addr, const uint8_t* reg_data, uint32_t len,
                                       void* intf_ptr) {

  i2c_master_dev_handle_t i2cDev = *(i2c_master_dev_handle_t*)intf_ptr;
  sendBuf[0] = reg_addr;
  memcpy(&sendBuf[1], reg_data, len);
  return i2c_master_transmit(i2cDev, sendBuf, len + 1, SEND_TIMEOUT_MS);
}

void ibme280_delay(uint32_t period, void* intf_ptr) {
  ets_delay_us(period);
}