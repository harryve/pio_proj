#include <Arduino.h>
#include "hwdefs.h"
#include "ldr.h"

void LdrInit()
{
  pinMode(LDR_PIN, INPUT);
}

bool LdrRead(int *pVal)
{
  static unsigned long prevMillis = 0;

  unsigned long currentMillis = millis();
  if (currentMillis - prevMillis > 60000) {
    prevMillis = currentMillis;

    uint16_t val = analogRead(LDR_PIN);
    *pVal = (int)val;
    return true;
  }
  return false;
}
