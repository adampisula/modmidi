# MM00 — ModMIDI Base

MM00 is the base/controller board for the ModMIDI modular system. It connects to the USB host, communicates with attached modules over I²C and an alert line, and breaks out MCU signals for compatible shields such as MM01.

**Revision:** 1.0, dated 2025-02-09 in the schematic title block.

## Hardware overview

- **MCU:** STM32F103C8, per the project specification. The schematic uses the `STM32F103C_8-B_Tx` family symbol.
- **Clocking:** Y1 is marked `16M 9p`; Y2 is marked `32K 12.5p`. The 16 MHz HSE is important when selecting/configuring the PlatformIO Arduino core; do not assume a Blue Pill's 8 MHz crystal.
- **USB:** USB D+/D−, USBLC6-4SC6 ESD protection, and an MCU-controlled D+ pull-up. USB-MIDI support is provided through the firmware dependencies.
- **Power:** a +5 V rail, LM66100 ideal-diode protection, AMS1117-3.3 regulation, and a separate 3.3 V analog rail (`3.3VA`). Check the schematic before applying power or connecting an external supply.
- **Module interface:** I²C SDA/SCL and the shared active-low alert net `NALERT`.
- **Indicators/debug:** status and error LED signals, UART TX/RX with an `EN_DEBUG` jumper, SWDIO/SWCLK programming signals, and a `BOOT_SELECT` jumper.

## Connectors and signal groups

| Reference | Connector | Signals / notes |
|---|---|---|
| J7 | 5-pin JST XH, in the POGO section | +5 V, GND, `I2C_SDA`, `I2C_SCL`, and `NALERT`. Use the schematic for the numbered pin order. |
| J8, J9 | Two 1×14, 2.54 mm headers | MCU GPIO and power breakout for compatible shields. The schematic labels include PA0–PA8, PB4, PB5, and PB8–PB15; use the schematic/PDF for the exact pin-to-header mapping and orientation. |
| USB connector | USB data and power interface | USB D+/D− with ESD protection and the controlled D+ pull-up. Confirm connector orientation and supply limits in the PCB/schematic before use. |
| Debug/programming headers and jumpers | UART, SWD, boot selection | Signals include UART TX/RX, SWDIO/SWCLK, `EN_DEBUG`, and `BOOT_SELECT`; follow the schematic for exact connector pin numbering. |

The table summarizes signal groups, not a substitute for the pin numbering and orientation shown in the KiCad design. Review the schematic before wiring a module or applying power.

## KiCad project and PDF

Open [`hardware/MM00.kicad_pro`](hardware/MM00.kicad_pro) in **KiCad 9.x**. MM00 uses the KiCad 9 file format; older KiCad versions may not open it correctly. The schematic symbols are embedded in the project.

- Schematic PDF: [`hardware/output/MM00-schematic.pdf`](hardware/output/MM00-schematic.pdf) — **not generated yet**; Adam will generate and add this file manually.
- The PDF, if present, is a reading copy; use the KiCad project as the editable source.

## Firmware build and flashing

The PlatformIO project is [`software/platformio.ini`](software/platformio.ini). It targets `genericSTM32F103C8`, uses Arduino, links the local `modmidi-arduino` library, and includes the USBLibrarySTM32 build flags required for USB-MIDI.

```sh
cd boards/MM00/software
pio run
```

There is currently **no firmware source** in `software/`, so `pio run` cannot produce firmware yet. Adam plans to add the firmware later. Once source is added, confirm the Arduino core's clock/USB setup for MM00's 16 MHz HSE and select the actual upload method before flashing. No upload protocol is currently configured; do not assume ST-Link or the UART bootloader without checking the hardware/programmer.

## BOM and manufacturing outputs

No BOM, Gerbers, or generated PDFs are currently included. Adam will generate hardware outputs manually under `hardware/output/`; intended PDF and flat BOM files are not ignored by the repository rules. Add manufacturing notes and links here when those files are available.
