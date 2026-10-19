# EX08 — ModMIDI Extension Board (8 ports)

EX08 is a passive wiring/extension board for connecting ModMIDI modules. It contains no MCU, so it has no PlatformIO project, firmware, or flashing procedure.

**Revision:** 1, dated 2025-02-15 in the schematic title block.

## Connectors and signals

The schematic shows four 5-pin input/output connectors and eight 5-pin pogo connectors. Each carries the shared ModMIDI module signals:

| References | Role | Signals |
|---|---|---|
| J_IN1, J_IN2 | Input connectors | `5V`, GND, `I2C_SDA`, `I2C_SCL`, `NALERT` |
| J_OUT1, J_OUT2 | Output connectors | `5V`, GND, `I2C_SDA`, `I2C_SCL`, `NALERT` |
| J_POGO_M1–M4, J_POGO_F1–F4 | Male/female pogo connections | `5V`, GND, `I2C_SDA`, `I2C_SCL`, `NALERT` |

This is a passive interconnect board; observe the schematic's connector pin numbering, orientation, and power distribution when wiring it. Do not treat the signal list as a pin-order table.

## KiCad project and PDF

Open [`hardware/EX08.kicad_pro`](hardware/EX08.kicad_pro) in **KiCad 9.x**. EX08 uses KiCad 8-format files; KiCad 9.x is the common version recommended for all ModMIDI boards.

- Schematic PDF: [`hardware/output/EX08-schematic.pdf`](hardware/output/EX08-schematic.pdf) — **not generated yet**; Adam will generate and add this file manually.
- The KiCad project is the editable source; the PDF will be a reading copy.

## BOM and manufacturing outputs

No BOM, Gerbers, or generated PDFs are currently included. Adam will generate hardware outputs manually under `hardware/output/`; intended PDF and flat BOM files are not ignored by the repository rules. Add manufacturing notes and links here when those files are available.
