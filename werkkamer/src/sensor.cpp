#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include "sensor.h"

Adafruit_BME280 bme;

#define BME_I2C_ADDRESS     0x76

static bool status;

void SensorInit(void)
{
    status = bme.begin(BME_I2C_ADDRESS);
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
    }
}

static float OneDecimal(float raw)
{
    int val = (int)round(raw * 10.0);
    return (float)val / 10.0;
}

float SensorTemperature()
{
    return OneDecimal(bme.readTemperature());
}

float SensorPressure()
{
    return OneDecimal(bme.readPressure() / 100.0F);
}

float SensorHumidity()
{
    return OneDecimal(bme.readHumidity());
}
