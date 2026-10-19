#ifndef MODMIDI_PINLED_H
#define MODMIDI_PINLED_H

#ifndef Arduino_h
#include <Arduino.h>
#endif

class PinLED {
  public:
    PinLED(uint8_t, bool = false);

    void setup();

    void set(bool);
    bool toggle();
    bool getState();

  private:
    uint8_t pin;
    bool inverted;

    bool ledState = false;
};

#endif