
#include <Arduino.h>

#include "Util.h"

void setupPin(const int pin, const int mode, const int initialValue){
  pinMode(pin, mode);
  digitalWrite(pin, initialValue);
}

