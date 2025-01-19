#pragma once

class Alarm {
  private:
    int buzzerPin;
    unsigned int startTime;
    int state;
    int count;
    int tones;

  public:
    Alarm(int pin);
    void trigger();
    bool tick(bool &invert);
};