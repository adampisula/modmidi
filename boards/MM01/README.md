# MM01 — ModMIDI 4×4 Pad Shield

MM01 is a 4×4 button and LED shield for the ModMIDI system. It does **not** contain its own MCU; its signals connect through MM00, whose STM32F103C8 runs any future MM01 firmware.

**Revision:** 1, dated 2025-02-09 in the schematic title block.

## Hardware overview

- Sixteen TL1105 push buttons are scanned through two 74HC165 parallel-in/serial-out shift registers.
- 74HC14 Schmitt-trigger logic conditions/debounces the button signals.
- WS2813 addressable RGB LEDs form the LED array.
- An AO3400A MOSFET provides the LED-data level-shifting stage between the controller logic and LED signal path.
- The shield uses +5 V from the base-board headers for the LED circuitry; it has no standalone MCU or firmware target.

## Connectors and controller signals

| Reference | Connector | Signals / notes |
|---|---|---|
| J1, J2 | Two 1×13, 2.54 mm sockets | Shield connections to the matching MM00 base-board headers. Check the schematic for pin numbering and orientation. |
| MM00 GPIO | Controller signals on the shield interface | The schematic maps the key signals as shown below. |

| MM00 MCU pin | MM01 signal | Function |
|---|---|---|
| PA2 | `LED_IN_MCU` | LED data input to the shield's level-shifting/LED path |
| PA3 | `NBUTTON_LOAD` | Loads the button shift-register state |
| PA5 | `SERIAL_CLOCK` | Shift-register serial clock |
| PA6 | `SERIAL_OUT` | Button shift-register serial data back to the MCU |

The shield takes its +5 V supply through the base-board connection. Use the schematic for the complete connector pin order, power pins, and electrical details before connecting a board.

## KiCad project, libraries, and PDF

Open [`hardware/MM01.kicad_pro`](hardware/MM01.kicad_pro) in **KiCad 9.x**. MM01 uses KiCad 8-format files; KiCad 9.x is the common version recommended for all ModMIDI boards.

The project includes a local footprint table at [`hardware/fp-lib-table`](hardware/fp-lib-table), the used `SW-Eswitch:TL1105` footprint under `hardware/footprints/SW-Eswitch.pretty/`, and the referenced STEP model under `hardware/3dmodels/SW-Eswitch.3dshapes/`. The PCB stores the placed 2D footprint geometry and uses project-relative `${KIPRJMOD}` paths for the bundled asset. A standard KiCad installation is still required for the standard libraries and models. The bundled E-Switch asset had no accompanying license/provenance document in the source library; its redistribution terms must be confirmed before public publication.

- Schematic PDF: [`hardware/output/MM01-schematic.pdf`](hardware/output/MM01-schematic.pdf) — **not generated yet**; Adam will generate and add it manually.

## Firmware build and flashing

The PlatformIO project is [`software/platformio.ini`](software/platformio.ini). It targets the MM00 STM32F103C8 controller with Arduino, links the local `modmidi-arduino` library, and enables the USBLibrarySTM32 flags.

```sh
cd boards/MM01/software
pio run
```

There is currently **no firmware source** in `software/`, so this project cannot produce firmware yet. Adam plans to add the firmware later. When firmware is added, verify the pin mapping against the schematic and build/flash the **MM00 controller**; MM01 itself is a shield and cannot be flashed. The upload protocol remains unset until the actual programmer/bootloader is confirmed.

## BOM and manufacturing outputs

No BOM, Gerbers, or generated PDFs are currently included. Adam will generate hardware outputs manually under `hardware/output/`; intended PDF and flat BOM files are not ignored by the repository rules. Add manufacturing notes and links here when those files are available.
