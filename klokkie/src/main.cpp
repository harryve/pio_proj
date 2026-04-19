#include <Arduino.h>
#include <Adafruit_BME280.h>
#include "display.h"
#include "co2sensor.h"
#include "timesync.h"
#include "network.h"
#include "log.h"

#define RADAR_SENSOR    D7
#define LED_PIN         D2
#define LED_ON          LOW
#define LED_OFF         HIGH

#define SENSOR_READ_INTERVAL    10000                               // In milli seconds
#define SENSOR_PUBLISH_INTERVAL ((5 * 60 * 1000) / SENSOR_READ_INTERVAL)    // Loop count
#define LOOP_DELAY              100                                 // milli seconds
#define STATE_PUBLISH_INTERVAL  (5 * 60 * 1000)                     // milli seconds

static Adafruit_BME280 bme;
static Display display;

static float OneDecimal(float raw)
{
    int val = (int)round(raw * 10.0);
    return (float)val / 10.0;
}

static void ReadSensors()
{
    static int publishCountDown = 0;
    static unsigned long sensorReadTime;

    if (millis() - sensorReadTime > SENSOR_READ_INTERVAL) {
        sensorReadTime = millis();
        float temperature = OneDecimal(bme.readTemperature());               // °C
        float humidity    = OneDecimal(bme.readHumidity());                  // %
        float pressure    = OneDecimal(bme.readPressure() / 100.0);          // hPa
        display.SetTemperature(temperature);
        if (--publishCountDown <= 0) {
            publishCountDown = SENSOR_PUBLISH_INTERVAL;
            int co2 = Co2SensorMeasure();
            PublishSensor(temperature, humidity, pressure, co2);
            LOG("T=%.1f H=%.1f P=%.1f co2=%d\n", temperature, humidity, pressure, co2);
        }
    }
}

static void UpdateDisplay()
{
    static int dispSec = -1;
    tm timeinfo;

    if (getLocalTime(&timeinfo)) {
        if (dispSec != timeinfo.tm_sec) {
            dispSec = timeinfo.tm_sec;
            //LOG("Ontime = %d, perc = %d\n", onTime, (onTime * 100) / ON_TIME);
            display.SetTime(timeinfo.tm_hour, timeinfo.tm_min, dispSec);
            display.Show(100, TimeIsSynced());
        }
    }
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LED_ON);

    delay(250); // wait for the OLED to power up

    Serial.begin(115200);
    LOG("\n\nStart klokkie " __DATE__ ", " __TIME__ "\n");

    Co2SensorSetup();

    display.Init();
    delay(2000); // Time to read display

    NetworkInit();

    // ---- BME280 INIT ----
    LOG("BME start\n");
    if (!bme.begin(0x76)) {
        LOG("BME280 not found!\n");
    }

    pinMode(RADAR_SENSOR, INPUT);

    TimeSyncInit();

    display.Off();
    LOG("Setup complete\n");
    digitalWrite(LED_BUILTIN, LED_OFF);
}

void loop()
{
    static bool present = false;
    static unsigned long statePublishTime;

    NetworkTick();

    if (digitalRead(RADAR_SENSOR)) {
        // Something detected by radar sensor
        if (!present) {
            present = true;
            digitalWrite(LED_BUILTIN, LED_ON);

            PublishState(present, TimeIsSynced());
            statePublishTime = millis();
        }
    }
    else {
        // No one there
        if (present) {
            present = false;
            display.Off();
            digitalWrite(LED_BUILTIN, LED_OFF);

            PublishState(present, TimeIsSynced());
            statePublishTime = millis();
        }
    }

    ReadSensors();

    if (present) {
        UpdateDisplay();
    }

    if (millis() - statePublishTime > STATE_PUBLISH_INTERVAL) {
        PublishState(present, TimeIsSynced());
        statePublishTime = millis();
    }

    delay(LOOP_DELAY);
}
