#ifndef HX_711_H__
#define HX_711_H__

#include <driver/gpio.h>

typedef enum {
  HX711_CHA_128 = 0,
  HX711_CHB_32,
  HX711_CHB_64,
} Hx711Channel;

typedef struct {
  gpio_num_t gpioData;
  gpio_num_t gpioSck;
  Hx711Channel ch;
  int32_t tare;
  float offset;
  float scale;
} Hx711;

/**
 * @brief Initialize the hx711 device.
 *  Currently it does nothing.
 * @param dev Pointer to the device.
 * @return 0
 */
uint8_t hx711_init(Hx711* dev);

/**
 * @brief Read the Raw value from the HX711 without any modification
 * @param dev Pointer to the device
 * @param value Where the value will be stored.
 * @return 0
 */
uint8_t hx711_readValueRaw(Hx711* dev, uint32_t* value);

/**
 * @brief Read the value from the Hx711 and apply tare and scaling.
 * @param dev Pointer to the hx711 device.
 * @param grams Measured weight in grams.
 * @return 0
 */
uint8_t hx711_readValueScaled(Hx711* dev, float* grams);

/**
 * @brief Configure tare value, in @tare insert the raw value
 * read using the @hx711_readValueRaw.
 * @param dev Pointer to the hx711 device.
 * @param tare Raw value when the scale has no weight on it.
 * @return 0
 */
uint8_t hx711_setTare(Hx711* dev, int32_t tare);

/**
 * @brief Set the scale and offset value for the linear interpolation
 * The result is w[g] = hx711 * scale + offset.
 * @param dev Pointer to the hx711 device.
 * @param scale Scale value.
 * @param offset Offset value from the interpolation.
 * @return 
 */
uint8_t hx711_setScaleOffset(Hx711* dev, float scale, float offset);

#endif  //HX_711_H__