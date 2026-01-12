#pragma once
#include <Arduino.h>

class Display {
public:
    Display();

    void Init();
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
