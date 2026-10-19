#ifndef Arduino_h
#include <Arduino.h>
#endif

#ifndef TwoWire_h
#include <Wire.h>
#endif

#ifndef MIDIUSB_h
#include <MIDIUSB.h>
#endif

#include "ModMIDI/Alert.h"
#include "ModMIDI.h"

// ModMIDI
ModMIDI::ModMIDI(uint8_t alertSelfPin, uint8_t alertCommonPin) {
  this->alert = Alert(alertSelfPin, alertCommonPin);
}

// Static
void ModMIDI::onAlertCallback(long currentMillis) {
  // Serial.println("Alert callback inside ModMIDI!");

  if(instance->role == ModuleRole::Master) {
    instance->setLastAlertMillis(currentMillis);

    // Read messages from slaves
    uint8_t* buffer = new uint8_t[4];

    for(uint8_t i = 0; i < 127; i++) {
      if(instance->slaveModuleAddresses[i] == 0 || !instance->alert.getCommon()) {
        break;
      }

      // Serial.printf("Requesting I2C data from 0x%02X\n", instance->slaveModuleAddresses[i]);

      do {
        Wire.requestFrom(instance->slaveModuleAddresses[i], (uint8_t) 4);
        Wire.readBytes(buffer, 4);

        if(!(buffer[0] == 0 && buffer[1] == 0 && buffer[2] == 0 && buffer[3] == MIDI_MESSAGE_TERMINATOR)) {
          // Do something with received data
          Serial.printf(
            "I2C received from 0x%02X: %02X %02X %02X %02X\n",
            instance->slaveModuleAddresses[i],
            buffer[0], buffer[1], buffer[2], buffer[3]
          );

          MIDIMessage* message = new MIDIMessage(buffer[0], buffer[1], buffer[2]);
          instance->send(message);
        }
      } while (buffer[3] == MIDI_MESSAGE_CONTINUE);
    }

    // Check if scan is requested
    if(instance->isAlertScanSignal()) {
      Serial.println("Scanning modules...");
      uint8_t count = instance->scanModules();
      Serial.printf("Found %d modules\n", count);
    }
  }
}

void ModMIDI::onSlaveI2CReceive(int count) {
  // Serial.println("Slave I2C receive");
  uint8_t* buffer = new uint8_t[4];

  do {
    Wire.readBytes(buffer, 4);

    if(!(buffer[0] == 0 && buffer[1] == 0 && buffer[2] == 0 && buffer[3] == MIDI_MESSAGE_TERMINATOR)) {
      // Do something with received data
      Serial.printf("I2C received from master: %02X %02X %02X %02X\n", buffer[0], buffer[1], buffer[2], buffer[3]);

      MIDIMessage* message = new MIDIMessage(buffer[0], buffer[1], buffer[2]);
      if(instance->onReceiveMIDIMessageCallback != nullptr) {
        instance->onReceiveMIDIMessageCallback(message);
      }
      delete message;
    }
  } while(buffer[3] == MIDI_MESSAGE_CONTINUE);
}

void ModMIDI::onSlaveI2CRequest() {
  uint8_t* buffer = new uint8_t[4];

  if(instance->availableMIDIMessages() == 0) {
    buffer[0] = 0;
    buffer[1] = 0;
    buffer[2] = 0;
    buffer[3] = MIDI_MESSAGE_TERMINATOR;

  } else {
    instance->getNextI2CData(buffer);
  }

  Wire.write(buffer, 4);
}

// Setup
void ModMIDI::setup(uint8_t slaveAddress) {
  this->address = slaveAddress;

  this->alert.setup();
  this->alert.onAlert(onAlertCallback);

  MidiUSB.available();
  USB_Begin();

  int usbRunningCount = 0;
  while(!USB_Running() && usbRunningCount < USB_CONNECTION_TRIES) {
    usbRunningCount++;
    Serial.println("Waiting for USB...");
    delay(250);
  }

  if(USB_Running()) {
    Serial.println("USB available. Assuming master role.");
    this->role = ModuleRole::Master;
  } else {
    Serial.println("USB not available. Assuming slave role.");
    this->role = ModuleRole::Slave;
  }

  if(this->role == ModuleRole::Master) {
    Wire.begin();
  } else {
    Wire.begin(this->address);
    Wire.onReceive(onSlaveI2CReceive);
    Wire.onRequest(onSlaveI2CRequest);

    // Send scan alert signal - repeat 3 times
    for(uint8_t i = 0; i < 3; i++) {
      // 3 pulses 10ms apart
      for(uint8_t i = 0; i < 3; i++) {
        this->alert.set(true);
        delay(5);
        this->alert.set(false);
        delay(5);
      }

      delay(50);
    }
  }
}

void ModMIDI::onReceiveMIDIMessage(OnReceiveMIDIMessageCallbackType callback) {
  this->onReceiveMIDIMessageCallback = callback;
}

void ModMIDI::call(long currentMillis) {
  if(this->role == ModuleRole::Slave) {
    if(!this->alert.getSelf() && this->availableMIDIMessages() > 0) {
      this->alert.set(true);
    } else if(this->alert.getSelf() && this->availableMIDIMessages() == 0) {
      this->alert.set(false);
    }
  }

  if(this->role == ModuleRole::Master) {
    midiEventPacket_t rx = MidiUSB.read();

    while(rx.header != 0) {
      Serial.printf("USB MIDI available: %02X\n", rx.header);
      MIDIMessage *message = new MIDIMessage(rx);

      incomingUSBMIDIBuffer[incomingUSBMIDIBufferIndex] = message;
      incomingUSBMIDIBufferIndex = (incomingUSBMIDIBufferIndex + 1) % USB_MIDI_MESSAGE_BUFFER_SIZE;

      if(this->onReceiveMIDIMessageCallback != nullptr) {
        this->onReceiveMIDIMessageCallback(message);
      }

      rx = MidiUSB.read();
    }

    // Create bulk I2C data containing all MIDI messages in the buffer
    if(this->availableIncomingUSBMIDIMessages() > 0) {
      int availableMessages = this->availableIncomingUSBMIDIMessages();

      uint8_t* bulkBuffer = new uint8_t[4*availableMessages];

      for(int i = 0; i < availableMessages; i++) {
        MIDIMessage message = this->getNextIncomingUSBMIDIMessage();

        uint8_t* buffer = new uint8_t[4];
        message.toBytes(buffer);

        // Copy first 3 bytes to bulk buffer
        for(int j = 0; j < 3; j++) {
          bulkBuffer[i*4 + j] = buffer[j];
        }

        // Add terminator or continue byte
        if(i == availableMessages - 1) {
          bulkBuffer[i*4 + 3] = MIDI_MESSAGE_TERMINATOR;
        } else {
          bulkBuffer[i*4 + 3] = MIDI_MESSAGE_CONTINUE;
        }

        delete buffer;
      }

      for(int i = 0; i < 127; i++) {
        if(this->slaveModuleAddresses[i] == 0) {
          break;
        }

        Serial.printf("Sending incoming USB data to I2C 0x%02X\n", this->slaveModuleAddresses[i]);

        Wire.beginTransmission(this->slaveModuleAddresses[i]);
        Wire.write(bulkBuffer, 4*availableMessages);
        Wire.endTransmission();
      }
    }

    // Pass incoming MIDI messages to slaves and execute callback in master
    while(incomingUSBMIDIBufferIndex > 0) {
      MIDIMessage *message = incomingUSBMIDIBuffer[0];
      incomingUSBMIDIBuffer[0] = nullptr;
      incomingUSBMIDIBufferIndex--;

      if(this->onReceiveMIDIMessageCallback != nullptr) {
        this->onReceiveMIDIMessageCallback(message);
      }

      // Send to all slaves
      for(uint8_t i = 0; i < 127; i++) {
        if(this->slaveModuleAddresses[i] == 0) {
          break;
        }

        // Serial.printf("Sending I2C data to 0x%02X\n", this->slaveModuleAddresses[i]);

        uint8_t* buffer = new uint8_t[4];
        int byteCount = message->toBytes(buffer);

        Wire.beginTransmission(this->slaveModuleAddresses[i]);
        Wire.write(buffer, 4);
        Wire.endTransmission();

        if(byteCount < 3) {
          Wire.beginTransmission(this->slaveModuleAddresses[i]);
          Wire.write(MIDI_MESSAGE_TERMINATOR);
          Wire.endTransmission();
        }
      }
    }

    if(this->availableOutgoingUSBMIDIMessages() > 0) {
      while(this->availableOutgoingUSBMIDIMessages() > 0) {
        MIDIMessage message = this->getNextOutgoingUSBMIDIMessage();

        if(MidiUSB.availableTx()) {
          MidiUSB.sendMIDIQuick(message.toMidiEventPacket());
        } else {
          Serial.println("USB MIDI TX not available. Skipping message.");
        }
      }
    }

    // Unstick alert
    if(currentMillis - this->getLastAlertMillis() > 250 && this->alert.getCommon()) {
      this->onAlertCallback(currentMillis);
    }
  }

  this->alert.call(currentMillis);
}

void ModMIDI::send(MIDIMessage* message) {
  if(this->role == ModuleRole::Master) {
    this->outgoingUSBMIDIBuffer[this->usbMIDIBufferIndex] = message;
    this->usbMIDIBufferIndex = (this->usbMIDIBufferIndex + 1) % USB_MIDI_MESSAGE_BUFFER_SIZE;

    if(this->usbMIDIBufferIndex >= USB_MIDI_MESSAGE_BUFFER_SIZE) {
      this->usbMIDIBufferIndex = 0;
    }
  } else {
    this->midiBuffer[this->midiBufferIndex] = message;
    this->midiBufferIndex = (this->midiBufferIndex + 1) % MIDI_MESSAGE_BUFFER_SIZE;

    if(this->midiBufferIndex >= MIDI_MESSAGE_BUFFER_SIZE) {
      this->midiBufferIndex = 0;
    }
  }
}

uint8_t ModMIDI::scanModules() {
  for(uint8_t i = 0; i < 127; i++) {
    this->slaveModuleAddresses[i] = 0;
  }

  uint8_t count = 0;

  for(uint8_t i = 0; i < 127; i++) {
    Wire.beginTransmission(i);
    if(Wire.endTransmission() == 0) {
      this->slaveModuleAddresses[count] = i;
      count++;
    }
  }

  return count;
}

uint8_t* ModMIDI::getSlaveModuleAddresses() {
  return this->slaveModuleAddresses;
}
  
void ModMIDI::getNextI2CData(uint8_t* buffer) {
  MIDIMessage* message = this->midiBuffer[0];
  if(message == nullptr) {
    return;
  }

  // MIDI message buffer
  uint8_t* messageBuffer = new uint8_t[3];
  int byteCount = message->toBytes(messageBuffer);

  // Set buffer bytes 0 up to 2
  uint8_t i = 0;
  for(i = 0; i < byteCount; i++) {
    buffer[i] = messageBuffer[i];
  }

  for(uint8_t j = i; j < 3; j++) {
    buffer[j] = 0;
  }

  if(this->midiBuffer[1] == nullptr) {
    buffer[3] = MIDI_MESSAGE_TERMINATOR;
  } else {
    buffer[3] = MIDI_MESSAGE_CONTINUE;
  }

  delete message;
  this->midiBuffer[0] = nullptr;
  this->midiBufferIndex--;

  // Shift buffer
  for(uint8_t i = 1; i < MIDI_MESSAGE_BUFFER_SIZE; i++) {
    this->midiBuffer[i - 1] = this->midiBuffer[i];
  }
  this->midiBuffer[MIDI_MESSAGE_BUFFER_SIZE - 1] = nullptr;
}

int ModMIDI::availableMIDIMessages() {
  return midiBufferIndex;
}

MIDIMessage ModMIDI::getNextOutgoingUSBMIDIMessage() {
  MIDIMessage* messagePtr = this->outgoingUSBMIDIBuffer[0];
  MIDIMessage message = *messagePtr;

  delete messagePtr;
  
  this->outgoingUSBMIDIBuffer[0] = nullptr;
  this->usbMIDIBufferIndex--;

  // Shift buffer
  for(uint8_t i = 1; i < USB_MIDI_MESSAGE_BUFFER_SIZE; i++) {
    this->outgoingUSBMIDIBuffer[i - 1] = this->outgoingUSBMIDIBuffer[i];
  }
  this->outgoingUSBMIDIBuffer[USB_MIDI_MESSAGE_BUFFER_SIZE - 1] = nullptr;

  return message;
}

int ModMIDI::availableOutgoingUSBMIDIMessages() {
  return usbMIDIBufferIndex;
}

MIDIMessage ModMIDI::getNextIncomingUSBMIDIMessage() {
  MIDIMessage* messagePtr = this->incomingUSBMIDIBuffer[0];
  MIDIMessage message = *messagePtr;

  delete messagePtr;
  
  this->incomingUSBMIDIBuffer[0] = nullptr;
  this->incomingUSBMIDIBufferIndex--;

  // Shift buffer
  for(uint8_t i = 1; i < USB_MIDI_MESSAGE_BUFFER_SIZE; i++) {
    this->incomingUSBMIDIBuffer[i - 1] = this->incomingUSBMIDIBuffer[i];
  }
  this->incomingUSBMIDIBuffer[USB_MIDI_MESSAGE_BUFFER_SIZE - 1] = nullptr;

  return message;
}

int ModMIDI::availableIncomingUSBMIDIMessages() {
  return incomingUSBMIDIBufferIndex;
}

long ModMIDI::getLastAlertMillis() {
  return this->pastAlertMillis[0];
}

void ModMIDI::setLastAlertMillis(long millis) {
  this->pastAlertMillis[2] = this->pastAlertMillis[1];
  this->pastAlertMillis[1] = this->pastAlertMillis[0];
  this->pastAlertMillis[0] = millis;
}

bool ModMIDI::isAlertScanSignal() {
  long diff01 = this->pastAlertMillis[0] - this->pastAlertMillis[1];
  long diff12 = this->pastAlertMillis[1] - this->pastAlertMillis[2];

  if(
    diff01 > 9 && diff01 < 11 &&
    diff12 > 9 && diff12 < 11
  ) {
    return true;
  }

  return false;
}