# modmidi-arduino

`modmidi-arduino` is the Arduino-framework library for the ModMIDI modular MIDI system. It contains the module-level I²C/USB-MIDI orchestration API, MIDI message conversion, an alert-line helper, and small pin input/output helpers.

This directory is a library within the `adampisula/modmidi` monorepo, not a standalone PlatformIO project. Board projects in `boards/MM00/software/` and `boards/MM01/software/` reference it with a local `symlink://` dependency. The board projects currently contain configuration only; firmware source will be added later.

## Requirements and dependencies

- PlatformIO with the `ststm32` platform and Arduino framework.
- STM32F103C8 target hardware for the current ModMIDI projects.
- `levi--g/USBLibrarySTM32` `~1.0.1`, which supplies the STM32-compatible Arduino USB API used by MIDIUSB.
- The `adampisula/MIDIUSB` fork at branch `1.0.5`.
- The Arduino `Wire` API for I²C module communication.

The board PlatformIO configurations enable `USBCON` and `HAL_PCD_MODULE_ENABLED`, as required by USBLibrarySTM32 for MIDIUSB on STM32. The MIDIUSB fork's `library.properties` still lists `avr,sam,samd` architectures; the board projects use PlatformIO's `lib_compat_mode = soft`, which checks framework compatibility rather than filtering by that architecture field. The fork contains STM32-specific source changes, but the PlatformIO dependency chain has not yet been built in this environment.

The monorepo's first-party library is MIT-licensed. Dependencies keep their own licenses: USBLibrarySTM32 is LGPL-3.0-or-later and MIDIUSB is LGPL-2.1. See each dependency for its complete license terms.

## Using the library in this monorepo

Open a board software project and build from that directory after firmware source has been added:

```sh
cd boards/MM00/software
pio run
```

The `platformio.ini` in that project links `../../../modmidi-arduino` and resolves the dependencies above. Do not install the monorepo root as a PlatformIO library URL: `library.json` is inside this subdirectory, not at the repository root. PlatformIO downloads the external dependencies on the first dependency resolution.

Include individual helpers as needed:

```cpp
#include <ModMIDI/PinButton.h>
#include <ModMIDI/PinLED.h>
#include <ModMIDI/MIDIMessage.h>
```

Or include the main module API with:

```cpp
#include <ModMIDI.h>
```

## API overview

### `ModMIDI`

`ModMIDI(alertSelfPin, alertCommonPin)` coordinates a module using USB-MIDI, I²C, and the alert lines. Call `setup(slaveAddress)` once, then call `call(millis())` regularly from `loop()`. The setup logic attempts to start USB and selects master/slave behavior based on whether USB is running. Register a receive handler with `onReceiveMIDIMessage(callback)`, enqueue outgoing messages with `send(MIDIMessage*)`, and use `scanModules()` / `getSlaveModuleAddresses()` for I²C module discovery. The `alert` member exposes the module's `Alert` helper.

**Implementation caveat:** `ModMIDI.cpp` uses the static `ModMIDI::instance` pointer for callbacks, but this repository does not define or initialize that pointer. The main orchestration API needs follow-up before relying on it in firmware. The lower-level helpers below can be used independently.

### `MIDIMessage`

`MIDIMessage` represents a MIDI status byte and up to two data bytes. It can be constructed from raw bytes or a `midiEventPacket_t`; getters/setters expose the status, channel, event type, and data bytes. `toBytes()` converts to raw MIDI bytes, `toMidiEventPacket()` creates a USB-MIDI packet, `getSpecByteLength()` reports the MIDI message length, and `print()` writes the fields to `Serial`.

```cpp
MIDIMessage noteOn(0x90, 60, 100);  // channel 1, note 60, velocity 100
uint8_t raw[3];
int byteCount = noteOn.toBytes(raw);
midiEventPacket_t usbPacket = noteOn.toMidiEventPacket();
```

The USB packet can be passed to the MIDIUSB fork after USB has been initialized.

### `Alert`

`Alert(selfPin, commonPin, selfInverted = false, commonInverted = true)` configures the module's self-alert output and common-alert input. Call `setup()`, use `set(bool)` to drive the self line, and poll `call(currentMillis)` to detect a rising common-alert signal. `getSelf()` and `getCommon()` return the logical states. `onAlert(callback)` registers a callback receiving the current millisecond count; register it before polling because the current implementation invokes it on an edge without checking for a null callback.

### `PinButton`

`PinButton(pin, debounceDelay = 25)` configures an active-high input using `INPUT_PULLDOWN`. Call `setup()` once and poll `call(millis())` regularly. `getState()` returns the last accepted state. `onPress(callback)` registers a no-argument callback; `onRelease(callback)` registers a callback receiving the elapsed milliseconds between the accepted press and release transitions.

### `PinLED`

`PinLED(pin, inverted = false)` configures an output. Use `setup()`, `set(bool)`, `toggle()` (which returns the new logical state), and `getState()`. Set `inverted` for an active-low LED.

### `PinRotaryEncoder`

`PinRotaryEncoder(pinA, pinB, inverted = false)` configures two inputs with `INPUT_PULLUP`. Call `setup()` once and `call()` regularly to poll the encoder. Register no-argument callbacks with `onRotateClockwise()` and `onRotateCounterClockwise()`.

## Minimal pin-helper example

This example toggles an external LED when an active-high button connected to `PA0` is pressed. Connect the LED to `PA1` through a suitable resistor, or replace these pins with pins appropriate for your hardware.

```cpp
#include <Arduino.h>
#include <ModMIDI/PinButton.h>
#include <ModMIDI/PinLED.h>

PinButton button(PA0);
PinLED led(PA1);

void toggleLed() {
  led.toggle();
}

void setup() {
  button.setup();
  led.setup();
  button.onPress(toggleLed);
}

void loop() {
  button.call(millis());
}
```

The example documents the helper API; it is not a checked-in firmware project and has not been compiled in this environment.
