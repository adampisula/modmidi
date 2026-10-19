#ifndef Arduino_h
#include <Arduino.h>
#endif

#include "ModMIDI/PinButton.h"

PinButton::PinButton(uint8_t pin, uint8_t debounceDelay) {
  this->pin = pin;
  this->debounceDelay = debounceDelay;
}

void PinButton::setup() {
  pinMode(this->pin, INPUT_PULLDOWN);
}

void PinButton::call(long currentMillis) {
  bool buttonState = digitalRead(this->pin);

  if(
    this->prevButtonState != buttonState &&
    currentMillis - this->lastStateMillis > this->debounceDelay
  ) {
    if(buttonState) {
      if(this->onPressCallback) this->onPressCallback();
    } else {
      if(this->onReleaseCallback) this->onReleaseCallback(currentMillis - this->lastStateMillis);
    }

    lastStateMillis = currentMillis;
    prevButtonState = buttonState;
  }
}

bool PinButton::getState() {
  return this->prevButtonState;
}

void PinButton::onPress(OnPressCallbackType callback) {
  this->onPressCallback = callback;
}

void PinButton::onRelease(OnReleaseCallbackType callback) {
  this->onReleaseCallback = callback;
}
