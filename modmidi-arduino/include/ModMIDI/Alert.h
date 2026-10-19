#ifndef MODMIDI_ALERT_H
#define MODMIDI_ALERT_H

#ifndef Arduino_h
#include <Arduino.h>
#endif

typedef void (*OnAlertCallbackType)(long);
// typedef void* OnAlertCallbackType;

class Alert {
  public:
    Alert();
    Alert(uint8_t, uint8_t, bool = false, bool = true);

    void setup();
    void call(long);

    void set(bool);
    bool getSelf();
    bool getCommon();

    void onAlert(OnAlertCallbackType);

  private:
    uint8_t selfPin;
    uint8_t commonPin;

    bool selfInverted;
    bool commonInverted;

    bool selfState = false;
    bool prevCommonState = false;

    OnAlertCallbackType onAlertCallback = nullptr;
};

#endif