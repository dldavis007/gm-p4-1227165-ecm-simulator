# U10 A/D and sensor-acquisition map

## Scope

This document maps the U10 (`16034988`) analog-to-digital converter channels
shown on the 1227165 processor and input schematics to the channel selectors
used by the supplied 9340 firmware. It separates:

- schematic channel identity;
- normal-operation firmware acquisition;
- factory-test-only observation;
- raw RAM storage and later processed state; and
- hardware transfer functions that remain HAL assumptions.

Signal names on the schematic are direct hardware evidence. Channel selectors,
RAM writes, and branches in `bua-hac.lst` are direct firmware evidence.
Engineering-unit conversions are not inferred from resistor values alone.

## Complete schematic channel map

U10 is selected by `ADCCS` and shares the SPI nets `MOSI`, `MISO`, `SCK`, and
clock-enable `E`. The schematic exposes these analog inputs:

| Selector | U10 input/pin | Schematic net | Normal 9340 use | Factory scan |
| ---: | --- | --- | --- | --- |
| `$00` | AN0, pin 1 | `MAP2` | No explicit normal request found | Yes, stored at `$017B` |
| `$10` | AN1, pin 2 | `VOLT` | Battery/voltage processing | Yes, `$017C` |
| `$20` | AN2, pin 3 | `O2` | O2 acquisition/filtering | Yes, `$017D` |
| `$30` | AN3, pin 4 | `MAP` | No explicit normal request found | Yes, `$017E` |
| `$40` | AN4, pin 5 | `CTS` | Coolant acquisition/conversion | Yes, `$017F` |
| `$50` | AN5, pin 6 | `TPS` | TPS acquisition/normalization | Yes, `$0180` |
| `$60` | AN6, pin 7 | `PUMPVOLT` | Fuel-pump voltage | Yes, `$0181` |
| `$70` | AN7, pin 8 | `DIAG` | Diagnostic/start-run voltage mode | Yes, `$0182` |
| `$80` | AN8, pin 9 | `MAT` | Manifold-air-temperature acquisition | Yes, `$0183` |
| `$90` | AN9, pin 11 | `ESC` | No explicit normal request found | Yes, `$0184` |
| `$A0` | AN10, pin 12 | `VMAF` | Analog MAF acquisition/burn-off test | Yes, `$0185` |
| `$B0` | not identified on retained U10 drawing | unresolved | No explicit normal request found | Yes, `$0186` |

Factory code `$FDB5-$FDC5` scans selectors `$00` through `$B0` and stores 12
bytes sequentially at `$017B-$0186`. The retained schematic identifies only
AN0-AN10; selector `$B0` must remain unnamed rather than being assigned an
undocumented external input.

## A/D transaction boundary

Routine `$F1BE-$F1DF` masks interrupts around `$F1C3`, selects the A/D device
through the SPI latch, transmits the selector in A, stores the returned sample
temporarily at `$0064`, clocks a second byte, and deselects the device. This
establishes an 8-bit conversion-result interface. It does not establish U10's
sampling aperture, reference-voltage accuracy, conversion time, or electrical
transfer beyond what is drawn externally.

U10 `REF+`, `REF-`, supply and ground connections are visible, but the complete
reference-source behavior is not documented. Consequently raw counts and
firmware table behavior are stronger evidence than inferred volts-per-count.

## Normal-operation channel paths

### `$10` — VOLT / firmware battery value

The input schematic derives `VOLT` from a VIGN divider and capacitor and routes
it to U10 AN1. Firmware requests `$10` during startup and Segment E, stores the
sample at `$007E`, and uses it for voltage qualification, injector offset,
dwell/feedback handling, diagnostics, and output gating. “Battery” is the
firmware's functional label; the direct analog source drawn at AN1 is `VOLT`
from the VIGN-related conditioning network.

### `$20` — O2

The schematic's oxygen-sensor conditioning produces `O2` at AN2. Firmware
requests `$20` at `$D9AD`, filters it through `$006F/$0071/$0073`, and uses the
result for closed-loop fuel and O2 diagnostics. The op-amp and resistor network
are direct hardware evidence; exact sensor-to-count behavior remains an
electrical/HAL model.

### `$40` — CTS

The coolant input reaches U10 AN4. Segment 6 requests `$40` at `$F3B9`, applies
the listing's two-range table logic, and updates coolant state around
`$005B-$005F` plus diagnostic flags/timers. The calibration tables define the
executable count-to-temperature mapping; they do not by themselves prove every
external thermistor or bias tolerance.

### `$50` — TPS

TPS reaches AN5. Startup and the 6.25-ms common path request `$50`, store raw
counts at `$0081`, maintain learned closed-throttle state at `$0086/$0087`, and
produce normalized TPS/load-axis state at `$0082` plus transient TPS at
`$00DD-$00DE`.

### `$60` — PUMPVOLT

The pump-voltage divider/filter reaches AN6. `$E816-$E81B` requests `$60` and
stores the raw result at `$007F`; later code uses it in Error-54 and MAF
diagnostic qualification. This is distinct from AN1 `VOLT`.

### `$70` — DIAG

The diagnostic input reaches AN7. Startup and Segment 3 request `$70` and
compare raw counts with literal thresholds to select ordinary, diagnostic,
ALDL, or factory-related paths. The code often consumes the result immediately
rather than assigning one canonical raw-RAM variable.

### `$80` — MAT

MAT reaches AN8. Segment A requests `$80` at `$EBB3`, complements the raw byte,
stores the inverted value at `$012B`, updates processed MAT at `$0060`, and
maintains Error-23/25 qualification state. Raw electrical polarity and the
firmware's complement operation must both be preserved.

### `$A0` — VMAF / analog MAF

The VMAF-related input reaches AN10. Normal airflow code `$F7AC-$F7B6` requests
`$A0`, stores the raw sample at `$00ED`, and scales it into the MAF/airflow
path. Key-off burn-off diagnostics also request `$A0` at `$EB05`. This channel,
not MAP/MAP2, feeds the supplied image's main load producer.

## Channels without explicit normal acquisition

No normal-operation `JSR LF1BE` call in the supplied listing is preceded by
selectors `$00`, `$30`, or `$90`. Thus MAP2, MAP, and ESC are physically wired
to U10 but are only directly demonstrated as converted values by the factory
scan.

This absence is image-specific evidence, not a universal P4 claim. Possible
use inside U11/U12, other firmware variants, service procedures, or external
test interpretation remains unresolved. In particular:

- CAL61 and conditioned MAP meet at U11 pin 28 independently of U10 AN3.
- CAL29 participates in the analog ESC network, while CAL32 reaches U12
  `KNOCK`; those paths must not be collapsed into AN9 behavior.

## Raw versus processed state

| Channel | First persistent/raw state | Principal processed state |
| --- | --- | --- |
| AN1/VOLT | `$007E` | voltage-qualified modes, offsets and diagnostics |
| AN2/O2 | filter inputs/state around `$006F/$0071` | closed-loop and O2 diagnostic state |
| AN4/CTS | conversion flow into `$005D` | coolant words `$005B-$005F` and diagnostics |
| AN5/TPS | `$0081` | `$0082`, `$0086/$0087`, `$00DD/$00DE` |
| AN6/PUMPVOLT | `$007F` | Error-54 and MAF-diagnostic qualification |
| AN7/DIAG | often immediate compare | mode/status flags |
| AN8/MAT | complemented sample `$012B` | `$0060` and Error-23/25 state |
| AN10/VMAF | `$00ED` | `$00EF`, airflow `$00EA/$00EB`, load `$0063` |

`$0064` is the common A/D routine's temporary return buffer and must not be
mistaken for a dedicated sensor variable. Similarly, factory results
`$017B-$0186` are a test snapshot, not the normal acquisition layout.

## C and verification correspondence

The C port uses raw-selector calls such as `hw_adc(0x10u)`, `0x20u`, `0x40u`,
`0x50u`, `0x60u`, `0x70u`, `0x80u`, and `0xA0u`, preserving the listing's
channel interface. Normal consumers are split across startup, major-loop,
fuel/air, diagnostics and scheduler modules. Factory test explicitly samples
all 12 selector slots.

The PC HAL supplies raw 8-bit stimuli. Any volts, temperature, pressure,
airflow, or sensor-curve conversion outside the listing's own tables remains
F3 plant/HAL behavior rather than firmware fact. Step 133 therefore requires
no C or behavioral-baseline change.

## Remaining evidence needs

- Identify the physical/internal meaning of selector `$B0` from authoritative
  U10 documentation or controlled hardware testing.
- Determine whether AN0/MAP2, AN3/MAP, or AN9/ESC are used by unavailable code,
  other PROM variants, or only service/factory functions.
- Measure actual U10 reference voltages and input-transfer networks if an
  electrical-accuracy model is later required.
- Correlate factory-test `$017B-$0186` values with controlled pin voltages to
  verify channel order and polarity on hardware.
