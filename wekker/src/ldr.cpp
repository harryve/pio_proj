#include <Arduino.h>
#include "hwdefs.h"
#include "ldr.h"

void LdrInit()
{
  pinMode(LDR_PIN, INPUT);
}

#define AVG_COUNT 10
static int avg(int newVal)
{
  static int values[AVG_COUNT] = {0};
  static int pos = 0;

  values[pos] = newVal;
  if (++pos >= AVG_COUNT) {
    pos = 0;
  };

  int retVal = 0;
  for (int i = 0; i < AVG_COUNT; i++) {
    retVal += values[i];
  }
  return retVal / AVG_COUNT;
}

bool LdrRead(int *pVal, unsigned long sampleTime)
{
  static unsigned long prevMillis = 0;

  unsigned long currentMillis = millis();
  if (currentMillis - prevMillis > sampleTime) {
    prevMillis = currentMillis;

    uint16_t val = analogRead(LDR_PIN);
    *pVal = avg((int)val);
    return true;
  }
  return false;
}
