#ifndef MQTT_CBS_H__
#define MQTT_CBS_H__

#include <stdint.h>

typedef struct {
  uint8_t* topic;
  void (*callback)(uint8_t*, uint32_t);
} MqttSub;

#endif  //MQTT_CBS_H__