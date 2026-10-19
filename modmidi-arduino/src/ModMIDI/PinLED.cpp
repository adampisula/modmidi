#ifndef Arduino_h
#include <Arduino.h>
#endif

#include "ModMIDI/PinLED.h"

PinLED::PinLED(uint8_t pin, bool inverted) {
  this->pin = pin;
  this->inverted = inverted;
}

void PinLED::setup() {
  pinMode(this->pin, OUTPUT);
  digitalWrite(this->pin, this->inverted ? HIGH : LOW);
}

void PinLED::set(bool state) {
  this->ledState = state;
  digitalWrite(this->pin, this->inverted ? !state : state);
}

bool PinLED::toggle() {
  this->ledState = !this->ledState;
  this->set(ledState);

  return this->ledState;
}

bool PinLED::getState() {
  return this->ledState;
}