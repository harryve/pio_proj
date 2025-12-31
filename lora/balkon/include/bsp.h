#pragma once
namespace bsp {
    void Led(int percent);
    void SensorSupply(bool enable);
    void DeepSleep(int seconds);
    void VbatMeasurementEnable(bool enable);
    float MeasureVbat();
    void LowPowerDelayMs(int ms);
}