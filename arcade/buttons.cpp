#include "buttons.h"
#include <Arduino.h>

int waitForAnyKey() {
  while ((digitalRead(leftButtonPin) == HIGH) || (digitalRead(rightButtonPin) == HIGH))
    delay(10);
  while (!((digitalRead(leftButtonPin) == HIGH) || (digitalRead(rightButtonPin) == HIGH)))
    delay(10);
  int pressedButton = (int)(digitalRead(leftButtonPin) == HIGH);
  while ((digitalRead(leftButtonPin) == HIGH) || (digitalRead(rightButtonPin) == HIGH))
    delay(10);

  return pressedButton;
}