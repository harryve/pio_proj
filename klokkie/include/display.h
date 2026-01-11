#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

class Display {
public:
    Display();

    void Off();
    void Show(int dispTimePercentage, bool synced);
    void SetTime(int hour, int minute, int second);
    void SetTemperature(float temperature);

private:
    Adafruit_SH1106G *pDriver;

    int hour;
    int minute;
    int second;
    float temperature;
};
