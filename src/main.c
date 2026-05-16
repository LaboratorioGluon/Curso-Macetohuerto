#include <freertos/FreeRTOS.h>
#include <socket.h>
#include "comms.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "hx711.h"
#include "pump.h"
#include "scanneri2c.h"
#include "sensors.h"
#include "system.h"

#define CALIBRATION (0U)

typedef enum { STATE_NORMAL = 0, STATE_DEBUG } State;

State currentState = STATE_DEBUG;

void app_main()
{

    vTaskDelay(pdMS_TO_TICKS(3000));

    SystemDevs* sysDevs = system_init();

    SensorConfig sensorsconfig;
    sensorsconfig.bmeDev    = sysDevs->bme;
    sensorsconfig.adsDev    = sysDevs->ads;
    sensorsconfig.hx711Data = sysDevs->hx711Data;
    sensorsconfig.hx711Sck  = sysDevs->hx711Sck;

    sensors_init(&sensorsconfig);
    pump_init(sysDevs->pumpGpio);
    comms_init();

    if (currentState == STATE_DEBUG)
    {
        comms_udpInit();
    }

#if CALIBRATION
    sensors_calibrate();
#endif

    SensorData data;
    int64_t start = esp_timer_get_time();
    for (;;)
    {

        sensors_update(&data);

        if (currentState == STATE_DEBUG)
        {
            uint8_t buf[100] = "";
            /*printf("===DEBUG===\n");
      printf("Supply:\n - vSolar: %.3f\n - vBatt: %.3f\n", data.vSolar, data.vBatt);
      printf("Air:\n - Temp: %.3f\n - Humidity: %.3f\n - Pressure: %.3f\n", data.bme.airTemp,
             data.bme.humidty, data.bme.pressure);
      printf("Otros:\n - LDR: %.2f\n - Soil Humidity: %.2f\n - Weight: %.2f\n", data.adcLdr,
             data.adcHumidity, data.grams);*/

            /*sprintf(buf, "Supply:\n - vSolar: %.3f\n - vBatt: %.3f\n", data.vSolar, data.vBatt);
      comms_udpSend(buf, strlen(buf));
      sprintf(buf, "Air:\n - Temp: %.3f\n - Humidity: %.3f\n - Pressure: %.3f\n", data.bme.airTemp,
              data.bme.humidty, data.bme.pressure);
      comms_udpSend(buf, strlen(buf));
      sprintf(buf, "Otros:\n - LDR: %.2f\n - Soil Humidity: %.2f\n - Weight: %.2f\n", data.adcLdr,
              data.adcHumidity, data.grams);
      comms_udpSend(buf, strlen(buf));*/

            comms_getCommand();
            vTaskDelay(pdMS_TO_TICKS(200));
        }
        else
        {
            comms_send(&data);
            vTaskDelay(pdMS_TO_TICKS(1000));

            system_sleep(10);
        }

        // Activate the pump if humidity is low.
        /*if (data.adcHumidity > 2.0f) {
      ESP_LOGI("PUMP", "Empezando a regar!");
      pump_actuate();
      ESP_LOGI("PUMP", "Terminando de regar!");
    }*/

        // Deep sleep we never return.
    }
}
