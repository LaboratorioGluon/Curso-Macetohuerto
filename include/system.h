#ifndef SYSTEM_H__
#define SYSTEM_H__

#include <driver/i2c_master.h>

typedef struct {
  i2c_master_dev_handle_t bme;
} SystemDevs;

SystemDevs* system_init(void);

void system_sleep(void);

#endif  //SYSTEM_H__