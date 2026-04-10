#ifndef COMMS_H__
#define COMMS_H__

#include "sensors.h"

void comms_init(void);

void comms_send(SensorData* data);

#endif  //COMMS_H__