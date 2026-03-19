
#include "hx711.h"
#include <rom/ets_sys.h>

#define HX711_FIRST_SCK_WAI_US (2U)
#define HX711_SCK_TIME_US      (10U)
#define HX711_NUM_PULSES       (25U)
#define HX711_MSB_BIT_POS      (23U)

uint8_t hx711_init(Hx711* dev) {
  return 0;
}

uint8_t hx711_readValueRaw(Hx711* dev, uint32_t* value) {

  uint8_t numPulses = HX711_NUM_PULSES + (uint8_t)dev->ch;
  int32_t readData  = 0;
  uint8_t bitValue  = 0;

  while (gpio_get_level(dev->gpioData) == 1U)
    ;

  for (uint8_t i = 0U; i < numPulses; i++) {
    // Set SCK High
    gpio_set_level(dev->gpioSck, 1);
    // Wait T3
    ets_delay_us(HX711_SCK_TIME_US);
    // Set SCK Low
    gpio_set_level(dev->gpioSck, 0);
    // Read DATA
    if (i < 24) {
      bitValue = gpio_get_level(dev->gpioData);
      readData |= (bitValue << (HX711_MSB_BIT_POS - i));
    }
    // Wait T4
    ets_delay_us(HX711_SCK_TIME_US);
  }

  if (readData & 0x800000) {
    readData |= 0xFF000000;
  }

  *value = readData;
  return 0;
}

uint8_t hx711_readValueScaled(Hx711* dev, float* grams) {
  int32_t scaleValue = 0;
  hx711_readValueRaw(dev, &scaleValue);

  *grams = (scaleValue - dev->tare) * dev->scale + dev->offset;
}

uint8_t hx711_setTare(Hx711* dev, int32_t tare) {
  dev->tare = tare;
}

uint8_t hx711_setScaleOffset(Hx711* dev, float scale, float offset) {
  dev->scale  = scale;
  dev->offset = offset;
}