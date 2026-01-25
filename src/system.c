#include "system.h"

#include <driver/i2c_master.h>
#include <string.h>

#define BME280_ADDR 0x76

/*********************/
/* Private Variables */

static SystemDevs globalDevs;
i2c_master_bus_handle_t busHandle;

/*********************/
/* Public Interfaces */

SystemDevs* system_init(void) {

  // Contains the basic bus configuration.
  i2c_master_bus_config_t busConfig;

  // Reset all the configuration to 0.
  // This ensures that we have no garbage in the memory.
  memset(&busConfig, 0, sizeof(i2c_master_bus_config_t));

  // Specific project configuration
  busConfig.i2c_port = I2C_NUM_0;  // We only have one.
  busConfig.scl_io_num = GPIO_NUM_9;
  busConfig.sda_io_num = GPIO_NUM_8;
  busConfig.intr_priority = 0;  // No interrupt.
  busConfig.flags.enable_internal_pullup = 1;
  busConfig.flags.allow_pd = 0;
  busConfig.glitch_ignore_cnt = 7;  // Recomended value from esp-idf.
  busConfig.clk_source = I2C_CLK_SRC_DEFAULT;

  ESP_ERROR_CHECK(i2c_new_master_bus(&busConfig, &busHandle));

  // Contains the device configuration.
  // There may be multiple devices per i2c bus.
  i2c_device_config_t devConfig;

  // Reset all the configuration to 0. (As for the master_bus_config before)
  memset(&devConfig, 0, sizeof(i2c_device_config_t));

  // Specific device configuration.
  devConfig.dev_addr_length = I2C_ADDR_BIT_LEN_7;  // Standard i2c uses 7bit addr.
  devConfig.device_address = BME280_ADDR;
  devConfig.scl_speed_hz = 100000;  // 100 KHz, it may be changed up to 400 KHz.
  devConfig.scl_wait_us = 0;
  devConfig.flags.disable_ack_check = 0;

  // Add the 'devConfig' device to the bus 'busHandle'
  ESP_ERROR_CHECK(i2c_master_bus_add_device(busHandle, &devConfig, &globalDevs.bme));

  return &globalDevs;
}

void system_sleep(void) {}