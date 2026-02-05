
#include <driver/i2c_master.h>
#include <stdio.h>
#include "ads1115.h"

uint8_t ads1115_esp32Write(void* handle, uint8_t* data, uint32_t len) {
  esp_err_t ret = i2c_master_transmit(*(i2c_master_dev_handle_t*)handle, data, len, 100);
}

uint8_t ads1115_esp32Read(void* handle, uint8_t* data, uint32_t len) {
  esp_err_t ret = i2c_master_receive(*(i2c_master_dev_handle_t*)handle, data, len, 100);
  return ret;
}

void ads1115_esp32Create(Ads1115* ads, i2c_master_dev_handle_t* dev) {
  ads->isInitialized = 0;
  ads->hal.write     = ads1115_esp32Write;
  ads->hal.read      = ads1115_esp32Read;
  ads->hal.handle    = (void*)dev;
}