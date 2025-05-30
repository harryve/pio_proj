#include <Arduino.h>
#include "sensor.h"

#include <OneWire.h>
#include <DallasTemperature.h>

#define SENSOR_PIN D5

OneWire oneWire(SENSOR_PIN);
DallasTemperature sensors(&oneWire);
//static bool status;

void SensorInit(void)
{
    sensors.begin();
}

static float OneDecimal(float raw)
{
    int val = (int)round(raw * 10.0);
    return (float)val / 10.0;
}

float SensorTemperature()
{
    sensors.requestTemperatures();
    return OneDecimal(sensors.getTempCByIndex(0));
}
