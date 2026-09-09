# Processor, memory map, and custom-peripheral architecture

## Purpose

This chapter consolidates the evidence-supported processor-visible architecture of the GM P4 1227165 ECM used by the supplied BUA/9340 material. It is an architectural map, not a fabricated datasheet. Exact emitted firmware behavior remains authoritative; schematic connectivity establishes hardware endpoints; undocumented behavior inside U9, U11, U12, analog devices, optional ROM, and other external hardware remains an explicit boundary.

The project fidelity vocabulary applies throughout:

- **F0** — listing-backed firmware-exact state, ordering, branch, table, or raw write.
- **F1** — hardware-connected by schematic, connector, MEMCAL, or direct physical evidence.
- **F2** — processor-visible HAL signal supplied or observed by the PC harness.
- **F3** — simulation assumption such as transfer curve, polarity, timing, vehicle, or plant behavior.
- **F4** — external or unknown custom-device, analog, power, optional-ROM, or other hardware boundary.

## Processor-visible address-space organization

The supplied program material establishes a program window at `$8000-$FFFF`. The MEMCAL/PROM hardware exposes `A0-A14`, `D0-D7`, `/ROMCS`, and `/ROMOE`, consistent with a 32 KiB processor-visible PROM window. The historical BUA image itself is 16 KiB; this distinction is preserved rather than silently converting image size into a claim about the complete hardware decode.

The architecture visible to firmware can be divided into the following evidence-supported regions and interfaces:

| Region/interface | Established role | Evidence boundary |
| --- | --- | --- |
| low RAM / firmware variables | scheduler, sensor, control, diagnostic and communication state | F0 where listing-backed |
| processor/internal I/O and MPU-facing registers | interrupt, serial, A/D request/result, output and factory-test interaction | F0 raw access; electrical consequences F1-F4 |
| `$3FC0-$3FFF` | U9 custom-peripheral processor window | F0 for classified addresses; unclassified remainder stays unknown |
| `$4004` and other established raw I/O locations | output staging/device-facing writes where listing demonstrates them | F0 write semantics; downstream hardware F4 unless closed |
| `$5800` optional external window | optional HUD-ROM boundary reached by supplied firmware | F4; no invented implementation |
| `$6000` vector target | external/unsupplied code target | F4; target is direct F0 vector evidence, implementation absent |
| `$8000-$FFFF` | external program/PROM-visible window | F0 for emitted image/listing content; physical decode F1/F4 |
| `$FFF0-$FFFE` | eight emitted vector words | F0 table contents and targets; processor-specific consequences bounded |

This table is intentionally functional. It does not assert that every unmentioned address is unused or reserved.

## Program and vector architecture

The authoritative assembled listing is the source of truth for emitted instructions, addresses, tables, and vectors. The corrected source, S-record and generated binary have previously been reconciled against it. The live image includes calibration beginning at `$C000`, executable code through the upper program area, and eight emitted vector words at `$FFF0-$FFFE`.

Established vector targets include `$6000`, `$C9F4`, `$F27B`, and repeated `$C800` entries. `$F27B` is an immediate `RTI`. The target address is direct firmware evidence; assigning a processor-specific interrupt name or inventing the behavior behind unsupplied `$6000` code is not justified without separate evidence.

Startup at `$C800` and the exceptional/factory paths are therefore part of the processor architecture, while the physical reset/COP/power consequences remain outside the PROM-exact layer.

## U9: bus-visible custom timing peripheral

U9 (`16045148`) is the clearest processor-visible custom device. Schematic evidence establishes a bus-connected device with external ignition/injection/reference/feedback pins, while the listing establishes accesses in `$3FC0-$3FFF`.

The currently classified processor-visible addresses are:

- reference/timing state: `$3FC0`, `$3FC8`, `$3FEC`, `$3FFA`;
- knock-event state: `$3FCA`;
- injection timing/command: `$3FCE`, `$3FD0`;
- spark/dwell/control: `$3FDC`, `$3FE4`, `$3FE6`, `$3FE8`, `$3FF6`, `$3FFC`.

The external U9 pins directly established by schematic include `IGN`, `INJS`, `INJA`, `INJREF`, `IGNREF`, `ESTLOOP`, and `KNOCK`. Firmware-visible register semantics and those pin connections are independently strong evidence. The internal mapping between a particular U9 pin and a particular register is not automatically direct evidence.

`U9_REGISTER_WINDOW_MAP.md` contains the classified semantics and `U9_REGISTER_WINDOW_CLOSURE.md` records the closure rule: every other `$3FC0-$3FFF` location remains unclassified unless executable or independent hardware evidence establishes it. Unclassified does not mean unused, reserved, mirrored, or inaccessible.

## U11: MEMCAL-configured custom-device boundary

U11 is not represented by a processor-visible register block comparable to U9. Its role is established from schematic endpoints and MEMCAL connectivity. Evidence-supported external functions include MEMCAL-configured `OSC` and `MAP` relationships and reference/injection-limiting interfaces documented in `U11_U12_FUNCTIONAL_BOUNDARIES.md`.

High-value MEMCAL anchors include:

- `CAL42 -> U11 pin 18 OSC`;
- `CAL61 + conditioned MAP -> U11 pin 28`;
- additional CAL connections to U11 documented in the MEMCAL motherboard map;
- reference/injection-related external connections established on the ignition/injection schematic.

No direct processor read of CAL42/`OSC` has been identified, and the evidence does not establish that U11's oscillator is the U9 or processor timebase. U11 transfer functions, thresholds, phase, polarity and internal logic therefore remain F4.

## U12: reference/control and injector-driver boundary

U12 likewise has no demonstrated processor-visible register block. Its externally visible behavior divides usefully into two evidence-supported sections without claiming an internal implementation.

The reference/control side accepts conditioned distributor `REF`, MEMCAL `CYL`, EST/bypass-related signals and knock-related inputs. It produces `INJREF`, `IGNREF`, `ESTLOOP`, `KNOCK`, and other established external signals that reach U9/U11 or surrounding circuitry.

The injector-driver side receives U9 `INJS`/`INJA`, participates in current-sense/limit circuitry, and drives `INJOUT` toward Q1. The processor commands injection through U9; the transformation from those commands to U12/Q1 electrical waveforms remains a custom-device boundary.

Important separations are preserved:

- `CAL56 -> U12 CYL` is distinct from distributor `REF`;
- CAL56's relationship to PROM `LC009`/`LC225` is a strong end-to-end inference, not a direct encoding;
- CAL32/U12/U9 `KNOCK` is separate from the CAL29/U10-AN9 analog ESC path;
- U12 reference outputs reaching U9 do not prove a one-to-one mapping to U9 status or period registers.

## U10 and processor-visible analog acquisition

U10 forms the established analog acquisition boundary. The motherboard maps visible analog inputs AN0-AN10 to MAP2, VOLT, O2, MAP, CTS, TPS, PUMPVOLT, DIAG, MAT, ESC and VMAF. Firmware uses a common A/D routine at `$F1BE-$F1DF` and normal selectors `$10`, `$20`, `$40`, `$50`, `$60`, `$70`, `$80`, and `$A0`; factory test additionally scans the broader selector set through `$B0`.

This establishes processor-visible selection and storage behavior without requiring an invented analog transfer model. Raw samples are F2 inputs in the simulator; sensor curves, voltages and physical transfer functions belong in F3 unless independently measured.

## MEMCAL as part of the processor architecture

The MEMCAL is both program storage and hardware configuration. Its 66-contact J4 carrier contains the external PROM and the reconstructed 16055375/16055376 resistor networks. The physical carrier numbering and J4/CAL mapping are established separately in `MEMCAL_ARCHITECTURE.md` and must not be inferred from software addresses alone.

The two resistor networks configure external custom-device inputs rather than acting as ordinary processor-readable calibration bytes. This is why CAL42/OSC, CAL56/CYL, CAL61/MAP and CAL59/VIGN must remain distinct from PROM calibration locations even when firmware behavior provides a useful reverse-check.

For as-built reconstruction, direct photographs and physical inspection control legible component values and jumper population. KiCad and LTspice remain supporting topology/design evidence and do not override photographed hardware.

## Architectural data flow

At the highest supported level, normal operation can be viewed as:

`physical inputs -> analog/discrete conditioning -> processor-visible samples/status -> listing-backed control code -> raw processor/U9 commands -> custom/output hardware -> vehicle loads`

The ignition/injection timing path adds the U12/U9 custom-device loop:

`distributor REF -> U12 -> INJREF/IGNREF -> U9 timing/status -> firmware calculations -> U9 spark/injection commands -> U9 IGN/INJS/INJA -> U12/output hardware`

The MEMCAL adds configuration alongside this flow rather than merely supplying program bytes:

`MEMCAL resistor networks -> CAL nodes -> U11/U12 configuration inputs`

These paths explain why processor-exact emulation and electrical plant simulation should remain separate layers.

## Simulation and HAL consequence

The C port should preserve the same responsibility split already defined by Step 127:

1. F0 code preserves listing-backed state, arithmetic, ordering and raw register semantics.
2. F2 interfaces provide processor-visible samples, events and output observations.
3. F3 code may optionally model engineering units, sensors, vehicle dynamics and electrical timing.
4. F4 custom-device behavior is not synthesized merely to make the simulator convenient.

For U9, a future model may implement only the processor-visible semantics that the listing proves before adding any waveform model. For U11/U12, external pin/state interfaces may be exposed without inventing undocumented transfer functions.

## What this chapter closes

The repository now has an integrated architectural description that connects the program window, vectors, U9 register window, U10 analog boundary, U11/U12 custom-device boundaries, MEMCAL hardware configuration, and simulation fidelity model.

This closes the architectural organization question; it does **not** close:

- undocumented processor/internal peripheral details not established by the listing or schematics;
- the internal U9 register/pin implementation;
- U11/U12 transfer functions, thresholds, polarity, phase or current control;
- optional `$5800` ROM behavior;
- external `$6000` code;
- physical reset, power, transceiver or output-driver consequences; or
- addresses not reached/classified by the supplied firmware evidence.

Those remain explicit boundaries for later measurement or independent documentation.
