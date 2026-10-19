#ifndef Arduino_h
#include <Arduino.h>
#endif

#ifndef MIDIUSB_h
#include <MIDIUSB.h>
#endif

#include "ModMIDI/MIDIMessage.h"

MIDIMessage::MIDIMessage() {}

MIDIMessage::MIDIMessage(uint8_t status) {
  this->status = status;
  this->statusSet = true;
}

MIDIMessage::MIDIMessage(uint8_t status, uint8_t data1) {
  this->status = status;
  this->statusSet = true;
  this->data1 = data1;
  this->data1Set = true;
}

MIDIMessage::MIDIMessage(uint8_t status, uint8_t data1, uint8_t data2) {
  this->status = status;
  this->statusSet = true;
  this->data1 = data1;
  this->data1Set = true;
  this->data2 = data2;
  this->data2Set = true;
}

MIDIMessage::MIDIMessage(midiEventPacket_t packet) {
  this->status = packet.byte1;
  this->statusSet = true;
  this->data1 = packet.byte2;
  this->data1Set = true;
  this->data2 = packet.byte3;
  this->data2Set = true;
}

// Raw
void MIDIMessage::setStatus(uint8_t status) {
  this->status = status;
  this->statusSet = true;
}
uint8_t MIDIMessage::getStatus() {
  if(!this->statusSet) return 0x00;
  return this->status;
}

void MIDIMessage::setData1(uint8_t data1) {
  this->data1 = data1;
  this->data1Set = true;
}
uint8_t MIDIMessage::getData1() {
  if(!this->data1Set) return 0x00;
  return this->data1;
}

void MIDIMessage::setData2(uint8_t data2) {
  this->data2 = data2;
  this->data2Set = true;
}
uint8_t MIDIMessage::getData2() {
  if(!this->data2Set) return 0x00;
  return this->data2;
}

// Channel + Event type
void MIDIMessage::setChannel(uint8_t channel) {
  if(channel > 15) {
    channel = 0;
  }

  if(!this->statusSet) this->status = 0x00;

  this->status = (this->status & 0xF0) | channel;
  this->statusSet = true;
}
uint8_t MIDIMessage::getChannel() {
  if(!this->statusSet) return 0x00;
  return this->status & 0x0F;
}

void MIDIMessage::setEventType(uint8_t eventType) {
  if(eventType > 15) {
    eventType = 0;
  }

  if(!this->statusSet) this->status = 0x00;

  this->status = (this->status & 0x0F) | (eventType << 4);
  this->statusSet = true;
}
uint8_t MIDIMessage::getEventType() {
  if(!this->statusSet) return 0x00;
  return this->status >> 4;
}

int MIDIMessage::toBytes(uint8_t* buffer) {
  int byteCount = 0;

  buffer[0] = this->status;
  byteCount++;

  if(this->data1Set) {
    buffer[1] = this->data1;
    byteCount++;
  }

  if(this->data2Set) {
    buffer[2] = this->data2;
    byteCount++;
  }

  return byteCount;
}

midiEventPacket_t MIDIMessage::toMidiEventPacket() {
  midiEventPacket_t packet = {
    this->getEventType(),
    this->getStatus(),
    this->getData1(),
    this->getData2()
  };

  return packet;
}

int8_t MIDIMessage::getSpecByteLength(MIDIMessage* message) {
  // Channel-dependent messages
  uint8_t channelThreeByteBitmask[] = {
    0b1000, // Note Off
    0b1001, // Note On
    0b1010, // Polyphonic Key Pressure (Aftertouch)
    0b1011, // Control Change
    0b1110, // Pitch Bend
  };

  uint8_t channelTwoByteBitmask[] = {
    0b1100, // Program Change
    0b1101, // Channel Pressure (Aftertouch)
  };

  // Check channel-dependent
  uint8_t shiftedStatus = message->status >> 4;

  for(unsigned char bitmask : channelThreeByteBitmask) {
    if(shiftedStatus == bitmask) return 3;
  }

  for(unsigned char bitmask : channelTwoByteBitmask) {
    if(shiftedStatus == bitmask) return 2;
  }

  // Channel-independent messages
  uint8_t nonChannelThreeByteBitmask[] = {
    0b11110010, // Song Position Pointer
  };

  uint8_t nonChannelTwoByteBitmask[] = {
    0b11110001, // MIDI Time Code Quarter Frame
    0b11110011, // Song Select
  };

  uint8_t nonChannelOneByteBitmask[] = {
    0b11110100, // Undefined
    0b11110101, // Undefined
    0b11110110, // Tune Request
    0b11110111, // End of System Exclusive
    0b11111000, // Timing Clock
    0b11111001, // Undefined
    0b11111010, // Start
    0b11111011, // Continue
    0b11111100, // Stop
    0b11111101, // Undefined
    0b11111110, // Active Sensing
    0b11111111, // System Reset
  };

  uint8_t nonChannelUnknownByteBitmask[] = {
    0b11110000, // System Exclusive
  };

  // Check channel-independent
  uint8_t status = message->status;
  
  for(unsigned char bitmask : nonChannelThreeByteBitmask) {
    if(status == bitmask) return 3;
  }

  for(unsigned char bitmask : nonChannelTwoByteBitmask) {
    if(status == bitmask) return 2;
  }

  for(unsigned char bitmask : nonChannelOneByteBitmask) {
    if(status == bitmask) return 1;
  }

  for(unsigned char bitmask : nonChannelUnknownByteBitmask) {
    if(status == bitmask) return 0;
  }

  return -1;
}

void MIDIMessage::print() {
  Serial.printf(
    "Status: %02X, Data1: %02X, Data2: %02X\n",
    this->status,
    this->data1,
    this->data2
  );
}