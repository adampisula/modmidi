#ifndef Arduino_h
#include <Arduino.h>
#endif

#include "ModMIDI/Alert.h"

Alert::Alert() {}

Alert::Alert(uint8_t self, uint8_t common, bool selfInverted, bool commonInverted) {
  this->selfPin = self;
  this->commonPin = common;
  this->selfInverted = selfInverted;
  this->commonInverted = commonInverted;
}

void Alert::setup() {
  pinMode(this->selfPin, OUTPUT);
  digitalWrite(this->selfPin, this->selfInverted ? HIGH : LOW);

  pinMode(this->commonPin, INPUT);
}

void Alert::call(long currentMillis) {
  bool commonState = this->getCommon();

  if(commonState != this->prevCommonState) {
    this->prevCommonState = commonState;

    if(commonState) {
      ((OnAlertCallbackType) this->onAlertCallback)(currentMillis);
    }
  }
}

void Alert::set(bool state) {
  this->selfState = state;
  digitalWrite(this->selfPin, this->selfInverted ? !state : state);
}

bool Alert::getSelf() {
  return this->selfState;
}

bool Alert::getCommon() {
  bool commonState = digitalRead(this->commonPin);
  return this->commonInverted ? !commonState : commonState;
}

void Alert::onAlert(OnAlertCallbackType callback) {
  this->onAlertCallback = callback;
}