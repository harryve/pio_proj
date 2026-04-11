#include "SoftwareSerial.h"
#include "Arduino.h"
#include "MHZCO2.h"
#include "co2sensor.h"


static SoftwareSerial ss(D6, D5);   // Tx, rx

static MHZ19B sensor;

void Co2SensorSetup()
{
    sensor.begin(&ss);
    ss.begin(9600);
}

int Co2SensorMeasure()
{
    int r = sensor.measure();
    if (r != MHZCO2_OK) {
        Serial.printf("CO2 sensor error %d\n", r);
        return 0;
    }
    return sensor.getCO2();

    //Serial.println(MHZ19B.getMinCO2());
    //Serial.println(MHZ19B.getTemperature());
    //Serial.println(MHZ19B.getAccuracy());
}