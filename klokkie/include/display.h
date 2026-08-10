#pragma once
#include <Arduino.h>

class Display {
public:
    Display();

    void Init(const char *pDate, const char *pTime);
    void Off();
    void Show(int dispTimePercentage, bool synced);
    void SetTime(int hour, int minute, int second);
    void SetTemperature(float temperature);

private:
    int hour;
    int minute;
    int second;
    float temperature;
};
