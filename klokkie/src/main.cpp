#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_BME280.h>
#include "timesync.h"
#include "network.h"
#include "log.h"

#define I2C_ADDRESS 0x3c  // SH1106 I2C address

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET -1     // QT-PY / XIAO

#define PIR_SENSOR   D7

static Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
static Adafruit_BME280 bme;

static void DisplayTime(int hour, int min, int sec, bool synced, float temperature, int dispTimePercentage)
{
    display.clearDisplay();
    display.setTextSize(4);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 0);
    display.printf("%2d:%02d", hour, min);

    display.drawFastHLine(0, 37, (SCREEN_WIDTH * dispTimePercentage) / 100, SH110X_WHITE);

    display.setTextSize(2);
    display.setCursor(0, 43);
    if (synced) {
        display.printf("%02d", sec);
    }
    else {
        display.print("??");
    }
    display.printf("  %.1f", temperature);
    display.display();
}

static float OneDecimal(float raw)
{
    int val = (int)round(raw * 10.0);
    return (float)val / 10.0;
}

void setup()
{
    delay(250); // wait for the OLED to power up

    Serial.begin(115200);
    LOG("\n\nStart klokkie " __DATE__ ", " __TIME__ "\n");

    display.begin(I2C_ADDRESS, true); // Address 0x3C default
    display.setContrast (0); // dim display
    display.clearDisplay();
    display.setTextSize(3);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 0);
    display.print("Klokkie");
    display.setTextSize(1);
    display.setCursor(0, 40);
    display.print(__DATE__);
    display.setCursor(0, 50);
    display.print(__TIME__);
    display.display();

    delay(2000); // Time to read display

    NetworkInit();

    // ---- BME280 INIT ----
    LOG("BME start %ld\n", millis());
    if (!bme.begin(0x76)) {
        LOG("BME280 not found!\n");
    }

    pinMode(PIR_SENSOR, INPUT);

    TimeSyncInit();

    display.clearDisplay();
    display.display();
    LOG("Setup complete\n");
}

#define SENSOR_READ_INTERVAL    10000   // In milli seconds
#define SENSOR_PUBLISH_INTERVAL ((5 * 60 * 1000) / SENSOR_READ_INTERVAL)
#define LOOP_DELAY              100
#define ON_TIME                 ((3 * 60 * 1000) / LOOP_DELAY)     // Minutes * seconds in millis / LOOP_DELAY
#define STATE_PUBLISH_INTERVAL  (5 * 60 * 1000)

void loop()
{
    static int onTime = 0;
    static int dispSec;
    static unsigned long sensorReadTime;
    static unsigned long statePublishTime;
    static int publishCountDown = 0;

    static float temperature, humidity, pressure;
    tm timeinfo;

    NetworkTick();

    if (digitalRead(PIR_SENSOR)) {
        if (onTime == 0) {
            statePublishTime = millis();
            PublishState(true, TimeIsSynced());
        }
        onTime = ON_TIME;
    }

    if (millis() - sensorReadTime > SENSOR_READ_INTERVAL) {
        sensorReadTime = millis();
        temperature = OneDecimal(bme.readTemperature());               // °C
        humidity    = OneDecimal(bme.readHumidity());                  // %
        pressure    = OneDecimal(bme.readPressure() / 100.0);          // hPa
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
                DisplayTime(timeinfo.tm_hour, timeinfo.tm_min, dispSec, TimeIsSynced(), temperature, (onTime * 100) / ON_TIME);
            }
        }
        onTime--;
        if (onTime == 0) {
            statePublishTime = millis();
            PublishState(false, TimeIsSynced());
            display.clearDisplay();
            display.display();
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
