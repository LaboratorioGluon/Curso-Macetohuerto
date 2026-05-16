#ifndef COMMS_H__
#define COMMS_H__

#include "sensors.h"

void comms_init(void);
void comms_udpInit(void);

void comms_send(SensorData* data);
void comms_udpSend(uint8_t* buf, uint32_t len);

uint8_t comms_getCommand();

#endif  //COMMS_H__