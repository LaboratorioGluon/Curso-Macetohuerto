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

    SensorData data;
    int64_t start = esp_timer_get_time();
    for (;;)
    {
        // On debug, just handle commands
        if (currentState == STATE_DEBUG)
        {
            comms_getCommand();
            vTaskDelay(pdMS_TO_TICKS(200));
        }
        else
        {

            // Update sensor data.
            sensors_update(&data);

            comms_send(&data);
            vTaskDelay(pdMS_TO_TICKS(1000));

            // Deep sleep we never return.
            system_sleep(10);
        }
    }
}
