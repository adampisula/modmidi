# ModMIDI

ModMIDI is a modular MIDI hardware and software project. This repository brings the base board, pad shield, extension board, and shared Arduino library together in one monorepo.

> **Current state:** the KiCad source projects and PlatformIO configuration are present, but there is no board firmware or generated hardware PDF yet. Firmware and PDFs will be added/generated later; the current projects are not yet ready for a firmware build or hardware release.

## Repository map

| Path | Contents |
|---|---|
| [`boards/MM00/`](boards/MM00/README.md) | Base/controller board; KiCad source, PlatformIO configuration, and board instructions |
| [`boards/MM01/`](boards/MM01/README.md) | 4×4 pad shield; KiCad source, local footprint/model assets, PlatformIO configuration, and board instructions |
| [`boards/EX08/`](boards/EX08/README.md) | Passive 8-port extension board; KiCad source and board instructions, no firmware project |
| [`modmidi-arduino/`](modmidi-arduino/README.md) | Shared Arduino library used by the board firmware |

The `MIDIUSB` fork is maintained as a separate repository and is not nested in this monorepo. PlatformIO obtains it through the library dependency declaration; users do not need to clone the sibling repository separately.

## Prerequisites

- **KiCad 9.x** to open all hardware projects. MM00 uses KiCad 9-format files; MM01 and EX08 use KiCad 8-format files.
- **PlatformIO Core** for future firmware builds. Install PlatformIO yourself; this repository does not provide CI builds.
- Network access on the first PlatformIO dependency resolution to retrieve external libraries.

MM01's project includes its custom TL1105 footprint and the referenced STEP model using project-relative paths. A standard KiCad installation is still required for standard libraries and models. The included E-Switch asset did not come with license/provenance information; confirm its redistribution terms before publishing the repository publicly.

## Open the hardware

Open each KiCad project file:

- [MM00 base board](boards/MM00/hardware/MM00.kicad_pro)
- [MM01 4×4 pad shield](boards/MM01/hardware/MM01.kicad_pro)
- [EX08 extension board](boards/EX08/hardware/EX08.kicad_pro)

The editable KiCad projects are the hardware source of truth. PDFs, Gerbers, and BOMs are generated manually and are not produced by a script in this repository.

### Board PDFs

No PDFs are included yet. Adam will generate and add them manually under each board's `hardware/output/` directory. These links show the planned locations and will become available once the files are generated:

- MM00: [`boards/MM00/hardware/output/MM00-schematic.pdf`](boards/MM00/hardware/output/MM00-schematic.pdf) — pending
- MM01: [`boards/MM01/hardware/output/MM01-schematic.pdf`](boards/MM01/hardware/output/MM01-schematic.pdf) — pending
- EX08: [`boards/EX08/hardware/output/EX08-schematic.pdf`](boards/EX08/hardware/output/EX08-schematic.pdf) — pending

Gerbers and BOMs may also be added manually under the same `hardware/output/` directories. Intended output files are not ignored by the repository rules.

## Firmware

MM00 is the base board with the STM32F103C8 controller. MM01 is a shield driven by the MM00 controller; EX08 is passive and has no firmware. The per-board PlatformIO configurations select the STM32 Arduino target and link the local `modmidi-arduino/` library. USB support flags are included for USBLibrarySTM32/MIDIUSB.

For example, the MM00 configuration is in `boards/MM00/software/platformio.ini`:

```sh
cd boards/MM00/software
pio run
```

MM01 has a corresponding configuration under `boards/MM01/software/`. **These commands cannot build firmware yet:** neither software directory contains source files. Adam plans to add firmware later. No upload protocol is configured; confirm the actual programmer/bootloader and MM00's 16 MHz HSE clock/USB setup before attempting to flash. EX08 has no `software/` directory.

The board projects reference the library locally with `symlink://../../../modmidi-arduino`. Do not install the repository root as a PlatformIO library URL; the library's `library.json` is inside the `modmidi-arduino/` subdirectory. PlatformIO fetches the declared USB dependencies, including the `adampisula/MIDIUSB` branch `1.0.5` and `levi--g/USBLibrarySTM32` `~1.0.1`.

See the [`modmidi-arduino` README](modmidi-arduino/README.md) for API details and current implementation notes.

## Licensing

First-party ModMIDI code, documentation, and hardware are provided under the MIT License; see [`LICENSE`](LICENSE). Third-party dependencies and assets retain their own licenses and are not relicensed by the root MIT license. In particular, MIDIUSB is LGPL-2.1 and USBLibrarySTM32 is LGPL-3.0-or-later. Redistribution terms for the bundled E-Switch footprint/model still need confirmation before a public push.
