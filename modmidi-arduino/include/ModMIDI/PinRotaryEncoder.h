#ifndef MODMIDI_ROTARYENCODER_H
#define MODMIDI_ROTARYENCODER_H

#ifndef Arduino_h
#include <Arduino.h>
#endif

typedef void (*OnRotateCallbackType)();

class PinRotaryEncoder {
  public:
    PinRotaryEncoder(uint8_t, uint8_t, bool = false);

    void setup();
    void call();

    void onRotateClockwise(OnRotateCallbackType);
    void onRotateCounterClockwise(OnRotateCallbackType);

  private:
    uint8_t pinA;
    uint8_t pinB;
    uint8_t debounceDelay;

    bool inverted = false;

    bool aState = false;
    bool bState = false;
    bool aLastState = false;
    bool bLastState = false;

    OnRotateCallbackType onRotateClockwiseCallback = nullptr;
    OnRotateCallbackType onRotateCounterClockwiseCallback = nullptr;
};

#endif