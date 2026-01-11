#include <Arduino.h>
// #include <Wire.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SH110X.h>
#include <Adafruit_BME280.h>
#include "display.h"
#include "timesync.h"
#include "network.h"
#include "log.h"

#define PIR_SENSOR  D7
#define LED_PIN     D2
#define LED_ON      LOW
#define LED_OFF     HIGH


static Adafruit_BME280 bme;
static Display *pDisplay;

static float OneDecimal(float raw)
{
    int val = (int)round(raw * 10.0);
    return (float)val / 10.0;
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LED_ON);

    delay(250); // wait for the OLED to power up

    Serial.begin(115200);
    LOG("\n\nStart klokkie " __DATE__ ", " __TIME__ "\n");
    pDisplay = new Display();

    delay(2000); // Time to read display

    NetworkInit();

    // ---- BME280 INIT ----
    LOG("BME start\n");
    if (!bme.begin(0x76)) {
        LOG("BME280 not found!\n");
    }

    pinMode(PIR_SENSOR, INPUT);

    TimeSyncInit();

    pDisplay->Off();
    LOG("Setup complete\n");
    digitalWrite(LED_BUILTIN, LED_OFF);
}

#define SENSOR_READ_INTERVAL    10000                               // In milli seconds
#define SENSOR_PUBLISH_INTERVAL ((5 * 60 * 1000) / SENSOR_READ_INTERVAL)    // Loop count
#define LOOP_DELAY              100                                 // milli seconds
#define ON_TIME                 ((3 * 60 * 1000) / LOOP_DELAY)      // Loop count: (Minutes * seconds[ms]] / LOOP_DELAY
#define STATE_PUBLISH_INTERVAL  (5 * 60 * 1000)                     // milli seconds

void loop()
{
    static int onTime = 0;
    static int dispSec;
    static unsigned long sensorReadTime;
    static unsigned long statePublishTime;
    static int publishCountDown = 0;

    tm timeinfo;

    NetworkTick();

    if (digitalRead(PIR_SENSOR)) {
        if (onTime == 0) {
            statePublishTime = millis();
            PublishState(true, TimeIsSynced());
            //digitalWrite(LED_BUILTIN, LED_ON);
        }
        onTime = ON_TIME;
    }

    if (millis() - sensorReadTime > SENSOR_READ_INTERVAL) {
        sensorReadTime = millis();
        float temperature = OneDecimal(bme.readTemperature());               // °C
        float humidity    = OneDecimal(bme.readHumidity());                  // %
        float pressure    = OneDecimal(bme.readPressure() / 100.0);          // hPa
        pDisplay->SetTemperature(temperature);
        if (--publishCountDown <= 0) {
            publishCountDown = SENSOR_PUBLISH_INTERVAL;
            PublishSensor(temperature, humidity, pressure);
            LOG("T=%.1f H=%.1f P=%.1f\n", temperature, humidity, pressure);
        }
    }

    if (onTime > 0) {
        if (getLocalTime(&timeinfo)) {
            if (dispSec != timeinfo.tm_sec) {
                dispSec = timeinfo.tm_sec;
                //LOG("Ontime = %d, perc = %d\n", onTime, (onTime * 100) / ON_TIME);
                pDisplay->SetTime(timeinfo.tm_hour, timeinfo.tm_min, dispSec);
                pDisplay->Show((onTime * 100) / ON_TIME, TimeIsSynced());
            }
        }
        onTime--;
        if (onTime == 0) {
            statePublishTime = millis();
            PublishState(false, TimeIsSynced());
            pDisplay->Off();
            digitalWrite(LED_BUILTIN, LED_OFF);
            LOG("Off\n");
        }
    }
    else {
        if (millis() - statePublishTime > STATE_PUBLISH_INTERVAL) {
            PublishState(false, TimeIsSynced());
            statePublishTime = millis();
        }
    }

    delay(LOOP_DELAY);
}
