#ifndef MODMIDI_MIDIMESSAGE_H
#define MODMIDI_MIDIMESSAGE_H

#ifndef Arduino_h
#include <Arduino.h>
#endif

#ifndef MIDIUSB_h
#include <MIDIUSB.h>
#endif

class MIDIMessage;

class MIDIMessage {
  public:
    MIDIMessage();
    MIDIMessage(uint8_t);
    MIDIMessage(uint8_t, uint8_t);
    MIDIMessage(uint8_t, uint8_t, uint8_t);
    MIDIMessage(midiEventPacket_t);

    // Raw
    void setStatus(uint8_t);
    uint8_t getStatus();

    void setData1(uint8_t);
    uint8_t getData1();

    void setData2(uint8_t);
    uint8_t getData2();

    // Channel + Event type
    void setChannel(uint8_t);
    uint8_t getChannel();

    void setEventType(uint8_t);
    uint8_t getEventType();

    int toBytes(uint8_t*);
    midiEventPacket_t toMidiEventPacket();

    static int8_t getSpecByteLength(MIDIMessage*);

    void print();

  private:
    uint8_t status;
    bool statusSet = false;

    uint8_t data1;
    bool data1Set = false;

    uint8_t data2;
    bool data2Set = false;
};

#endif