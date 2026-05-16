#include "console.h"

#include <freertos/FreeRTOS.h>
#include <stdio.h>
#include <string.h>

#include "comms.h"
#include "sensors.h"

#include "hx711.h"

// Command list

static void setCalibrationW(uint8_t* str);
static void readData(uint8_t* str);
static void showHelp(uint8_t* str);
static void tare(uint8_t* str);
static void soilRead(uint8_t* str);

#define MAX_CMD_LEN  15
#define MAX_HELP_LEN 150

typedef struct {
    uint8_t cmd[MAX_CMD_LEN];
    uint8_t help[MAX_HELP_LEN];
    void (*f)(uint8_t*);
} Command;

Command commands[] = {
    {.cmd = "help", .f = showHelp, .help = "Show this help"},
    // Common debug
    {.cmd = "read", .f = readData, .help = "Read sensor data"},

    // Weight commands (we.)
    {.cmd  = "we.tare",
     .f    = tare,
     .help = "Tare the weight. Use when there is nothing on the scale"},

    {.cmd  = "we.setCalib",
     .f    = setCalibrationW,
     .help = "Calibrate Weight sensor: calibrateW <gain> <offset>"},

    // Soil commands (so.)
    {.cmd  = "so.read",
     .f    = soilRead,
     .help = "Read Moisture sensor. soil.read <num_reading:default 10>"},
};

static void soilRead(uint8_t* str)
{

    uint8_t buf[200] = "";
    uint8_t* token;
    token            = strtok(str, " ");
    uint32_t samples = 0;
    // Check if a token is present
    if (token != NULL)
    {
        samples = atoi(token);
    }
    else
    {
        samples = 10;
    }
    SensorHandlers* sensors = sensors_getSensors(sensors);
    int32_t adcHumidity     = 0;
    sprintf(buf, "  Measuring the soil humidity %d times.\n", samples);
    comms_udpSend(buf, strlen(buf));

    ads1115_setMux(&sensors->ads, ADS1115_MUX_AIN1_GND);
    vTaskDelay(pdMS_TO_TICKS(30));
    // Recalibrate when the project is finished and installed.
    for (uint32_t i = 0; i < samples; i++)
    {
        adcHumidity += ads1115_readRaw(&sensors->ads);
        vTaskDelay(pdMS_TO_TICKS(20));
    }
    adcHumidity = adcHumidity / samples;
    sprintf(buf, "  Read RAW: %d\n  Read Scaled: %.3f\n", adcHumidity,
            ads1115_rawToVolts(&sensors->ads, adcHumidity));
    comms_udpSend(buf, strlen(buf));
}

static void tare(uint8_t* str)
{
    SensorHandlers* sensors = sensors_getSensors(sensors);

    uint32_t rawValue = 0;

    hx711_readValueRaw(&(sensors->hx711), &rawValue);

    uint8_t buf[200] = "";
    sprintf(buf, "Weight tared to %lu ", rawValue);
    hx711_setTare(&sensors->hx711, rawValue);
    comms_udpSend(buf, strlen(buf));
}

static void showHelp(uint8_t* str)
{
    uint8_t buf[200] = "";
    comms_udpSend("=== MacetoHuerto help ===\n", 27);
    for (uint32_t i = 0; i < sizeof(commands) / sizeof(Command); i++)
    {
        comms_udpSend(" - ", 4);
        sprintf(buf, "%-15s: ", commands[i].cmd);
        comms_udpSend(buf, strlen(buf));
        sprintf(buf, "%s\n", commands[i].help);
        comms_udpSend(buf, strlen(buf));
    }
}

static void setCalibrationW(uint8_t* str)
{
    uint8_t* token;
    token = strtok(str, " ");

    if (token != NULL)
    {
        printf("Data: %.2f\n", atof(token));
    }
}

static void readData(uint8_t* str)
{
    printf("Running Readdata\n");
    SensorData data;
    sensors_update(&data);

    printf("Supply:\n - vSolar: %.3f\n - vBatt: %.3f\n", data.vSolar, data.vBatt);
    printf("Air:\n - Temp: %.3f\n - Humidity: %.3f\n - Pressure: %.3f\n", data.bme.airTemp,
           data.bme.humidty, data.bme.pressure);
    printf("Otros:\n - LDR: %.2f\n - Soil Humidity: %.2f\n - Weight: %.2f\n", data.adcLdr,
           data.adcHumidity, data.grams);

    uint8_t buf[100] = "";
    sprintf(buf, "Supply:\n - vSolar: %.3f\n - vBatt: %.3f\n", data.vSolar, data.vBatt);
    comms_udpSend(buf, strlen(buf));
    sprintf(buf, "Air:\n - Temp: %.3f\n - Humidity: %.3f\n - Pressure: %.3f\n", data.bme.airTemp,
            data.bme.humidty, data.bme.pressure);
    comms_udpSend(buf, strlen(buf));
    sprintf(buf, "Otros:\n - LDR: %.2f\n - Soil Humidity: %.2f\n - Weight: %.2f\n", data.adcLdr,
            data.adcHumidity, data.grams);
    comms_udpSend(buf, strlen(buf));
}

void console_parse(uint8_t* str)
{
    uint8_t* token;
    token = strtok(str, " ");

    if (token != NULL)
    {
        // Calculate the pointer of the rest of the parameters to be send to the command.
        uint8_t* params = token + strlen((char*)token) + 1;

        // Find the required command.
        for (uint8_t i = 0; i < sizeof(commands) / sizeof(Command); i++)
        {

            if (strcmp(token, commands[i].cmd) == 0)
            {
                printf("Running command: '%s'\n", commands[i].cmd);
                commands[i].f(params);
            }
        }
    }
}