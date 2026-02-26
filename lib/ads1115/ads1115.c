#include "ads1115.h"
#include "ads1115_regs.h"

#include <errno.h>
#include <stddef.h>
#include <stdio.h>

#include <esp_log.h>

#define ADS_BITS 15

static const float fsrToVScale[] = {[ADS1115_FSR_6_144V] = 6.144f / (1 << ADS_BITS),
                                    [ADS1115_FSR_4_096V] = 4.096f / (1 << ADS_BITS),
                                    [ADS1115_FSR_2_048V] = 2.048f / (1 << ADS_BITS),
                                    [ADS1115_FSR_1_024V] = 1.024f / (1 << ADS_BITS),
                                    [ADS1115_FSR_0_512V] = 0.512f / (1 << ADS_BITS),
                                    [ADS1115_FSR_0_256V] = 0.256f / (1 << ADS_BITS)};

static uint8_t halBuffer[10] = {0};

static int32_t writeReg(Ads1115* ads, uint8_t reg, uint16_t data) {

  halBuffer[0] = reg;
  halBuffer[1] = (data >> 8) & 0xFF;
  halBuffer[2] = data & 0xFF;

  return ads->hal.write(ads->hal.handle, halBuffer, 3);
}

static int32_t readReg(Ads1115* ads, uint8_t reg, uint8_t* data) {

  halBuffer[0] = reg;
  ads->hal.write(ads->hal.handle, halBuffer, 1);

  return ads->hal.read(ads->hal.handle, data, 2);
}

int32_t ads1115_init(Ads1115* ads) {

  int32_t ret = 0;

  if (ads != NULL) {
    ret = EINVAL;
  } else {
    ads->isInitialized = 1;
  }
  return ret;
}

int32_t ads1115_config(Ads1115* ads, Ads1115Config* config) {

  int32_t ret = 0;

  if (config == NULL) {
    ret = EINVAL;
  } else {
    uint16_t regConfig = (((config->mode) << ADS1115_CFG_MODE_OFFSET) & ADS1115_CFG_MODE_MASK) |
                         (((config->dr) << ADS1115_CFG_DR_OFFSET) & ADS1115_CFG_DR_MASK) |
                         (((config->fsr) << ADS1115_CFG_PGA_OFFSET) & ADS1115_CFG_PGA_MASK) |
                         (((config->mux) << ADS1115_CFG_MUX_OFFSET) & ADS1115_CFG_MUX_MASK);
    ESP_LOGE("ADS", "Config set to: %x", regConfig);
    //write_register(ADS1115_REG_CFG, regConfig);
    ret = writeReg(ads, ADS1115_REG_CFG, regConfig);

    if (ret == 0) {
      ads->currentFsr = config->fsr;
    }
  }
  return ret;
}

int32_t ads1115_setMux(Ads1115* ads, Ads1115Mux mux) {
  int32_t ret = 0;

  if (ads == NULL) {
    ret = EINVAL;
  } else {
    uint16_t config;
    ret = readReg(ads, ADS1115_REG_CFG, (uint8_t*)&config);

    if (ret == 0) {
      config &= ~(ADS1115_CFG_MUX_MASK);
      config |= ((mux << ADS1115_CFG_MUX_OFFSET) & ADS1115_CFG_MUX_MASK);
      ret = writeReg(ads, ADS1115_REG_CFG, config);
    }
  }
  return ret;
}

int16_t ads1115_readRaw(Ads1115* ads) {
  int16_t ret;
  readReg(ads, ADS1115_REG_CONV, (uint8_t*)&ret);
  return ((ret & 0xFF) << 8) | ((ret >> 8) & 0xFF);
}

float ads1115_readVolts(Ads1115* ads) {
  return ads1115_readRaw(ads) * fsrToVScale[ads->currentFsr];
}