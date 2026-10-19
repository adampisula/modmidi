#ifndef MODMIDI_H
#define MODMIDI_H

#ifndef Arduino_h
#include <Arduino.h>
#endif

#ifndef MODMIDI_ALERT_H
#include "ModMIDI/Alert.h"
#endif

#ifndef MODMIDI_PINLED_H
#include "ModMIDI/PinLED.h"
#endif

#ifndef MODMIDI_MIDIMESSAGE_H
#include "ModMIDI/MIDIMessage.h"
#endif

#define MIDI_MESSAGE_CONTINUE 0x0F
#define MIDI_MESSAGE_TERMINATOR 0xF0

#define MIDI_MESSAGE_BUFFER_SIZE 32
#define USB_MIDI_MESSAGE_BUFFER_SIZE 32 

#define USB_CONNECTION_TRIES 8 // Every 250ms

enum class ModuleRole;
class ModMIDI;

enum class ModuleRole {
  Master,
  Slave,
};

typedef void (*OnReceiveMIDIMessageCallbackType)(MIDIMessage*);

class ModMIDI {
  public:
    static ModMIDI* instance;

    ModMIDI(uint8_t, uint8_t);

    Alert alert;

    // void setup(ModuleRole);
    void setup(uint8_t);
    void onReceiveMIDIMessage(OnReceiveMIDIMessageCallbackType);

    void call(long);

    void send(MIDIMessage*);

    uint8_t scanModules();
    uint8_t* getSlaveModuleAddresses();
    // void setSlaveModuleAddresses(uint8_t*, uint8_t);

  private:
    ModuleRole role;
    uint8_t address;

    OnReceiveMIDIMessageCallbackType onReceiveMIDIMessageCallback = nullptr;

    uint8_t slaveModuleAddresses[127] = {0};

    MIDIMessage *midiBuffer[MIDI_MESSAGE_BUFFER_SIZE] = {nullptr};
    uint8_t midiBufferIndex = 0;

    MIDIMessage *outgoingUSBMIDIBuffer[USB_MIDI_MESSAGE_BUFFER_SIZE] = {nullptr};
    uint8_t usbMIDIBufferIndex = 0;
    
    MIDIMessage *incomingUSBMIDIBuffer[USB_MIDI_MESSAGE_BUFFER_SIZE] = {nullptr};
    uint8_t incomingUSBMIDIBufferIndex = 0;

    long pastAlertMillis[3] = {0};
    
    static void onAlertCallback(long);
    // static void onMasterI2CReceive(int);
    static void onSlaveI2CReceive(int);
    static void onSlaveI2CRequest();

    void getNextI2CData(uint8_t*);
    int availableMIDIMessages();

    MIDIMessage getNextOutgoingUSBMIDIMessage();
    int availableOutgoingUSBMIDIMessages();

    MIDIMessage getNextIncomingUSBMIDIMessage();
    int availableIncomingUSBMIDIMessages();

    void setLastAlertMillis(long);
    long getLastAlertMillis();
    bool isAlertScanSignal();
};

#endif