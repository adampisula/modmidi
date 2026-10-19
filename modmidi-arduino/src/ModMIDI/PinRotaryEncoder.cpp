#ifndef Arduino_h
#include <Arduino.h>
#endif

#include "ModMIDI/PinRotaryEncoder.h"

PinRotaryEncoder::PinRotaryEncoder(uint8_t pinA, uint8_t pinB, bool inverted) {
  this->pinA = pinA;
  this->pinB = pinB;
  this->inverted = inverted;
}

void PinRotaryEncoder::setup() {
  pinMode(this->pinA, INPUT_PULLUP);
  pinMode(this->pinB, INPUT_PULLUP);
}

void PinRotaryEncoder::call() {
  bool aState = digitalRead(this->pinA);
  bool bState = digitalRead(this->pinB);

  if(aState != this->aLastState) {
    if(aState == bState) {
      if(this->inverted) {
        if(this->onRotateClockwiseCallback) this->onRotateClockwiseCallback();
      } else {
        if(this->onRotateCounterClockwiseCallback) this->onRotateCounterClockwiseCallback();
      }
    } else {
      if(this->inverted) {
        if(this->onRotateCounterClockwiseCallback) this->onRotateCounterClockwiseCallback();
      } else {
        if(this->onRotateClockwiseCallback) this->onRotateClockwiseCallback();
      }
    }
  }

  this->aLastState = aState;
}

void PinRotaryEncoder::onRotateClockwise(OnRotateCallbackType callback) {
  this->onRotateClockwiseCallback = callback;
}

void PinRotaryEncoder::onRotateCounterClockwise(OnRotateCallbackType callback) {
  this->onRotateCounterClockwiseCallback = callback;
}