class Alarm {
  private:
    int buzzerPin;
    unsigned int startTime;
    int state;
    int count;

  public:
    Alarm(int pin);
    void trigger();
    void tick();
};