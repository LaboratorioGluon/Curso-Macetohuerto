#include "mqtt_cbs.h"

#include <esp_log.h>

void watering(uint8_t* data, uint32_t len) {
  ESP_LOGE("MQTT", "Setting watering to %s", data);
}

MqttSub mqttTopics[] = {{"/macetohuerto/riego", watering}};