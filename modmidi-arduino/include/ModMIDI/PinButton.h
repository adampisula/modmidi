#ifndef MODMIDI_PINBUTTON_H
#define MODMIDI_PINBUTTON_H

#ifndef Arduino_h
#include <Arduino.h>
#endif

typedef void (*OnPressCallbackType)();
typedef void (*OnReleaseCallbackType)(long);

class PinButton {
  public:
    PinButton(uint8_t, uint8_t = 25);

    void setup();
    void call(long);

    bool getState();

    void onPress(OnPressCallbackType);
    void onRelease(OnReleaseCallbackType);

  private:
    uint8_t pin;
    uint8_t debounceDelay;

    bool prevButtonState = false;
    long lastStateMillis = 0;

    bool ledState = false;

    OnPressCallbackType onPressCallback = nullptr;
    OnReleaseCallbackType onReleaseCallback = nullptr;
};

#endif