# U11/U12 custom-device functional boundaries

## Purpose

This document consolidates what the current 1227165 evidence actually supports
about custom devices U11 (`16054995`) and U12 (`16034984`). It is a black-box
boundary description, not a reconstruction of undocumented internal circuitry.
It combines the direct schematic pin/net evidence, the photograph-authoritative
MEMCAL reconstruction, and listing-backed firmware consequences established in
Steps 130-135.

Evidence language follows the project convention:

- **Direct hardware evidence**: a visible schematic pin/net connection.
- **Mechanically derived**: a CAL identity established from the physical
  66-contact carrier/J4 mapping.
- **Direct firmware evidence**: emitted instructions, addresses, or data in
  `evidence/firmware/bua-hac.lst`.
- **Strong inference**: independently demonstrated endpoints form a coherent
  causal path, but undocumented custom logic hides the transformation.
- **Unresolved boundary**: the transformation, polarity, threshold, phase,
  encoding, or transfer function is not established by available evidence.

## Device-level envelope

| Device | Directly visible inputs/configuration | Directly visible outputs/interconnections | Supported role envelope | Unresolved internals |
| --- | --- | --- | --- | --- |
| U11 `16054995` | `INJREF` pin 5, `IGNREF` pin 6, `TPS` pin 20, `CTS` pin 23, `MAP` pin 28, `OSC` pin 18, and multiple CAL/MEMCAL-connected pins | `INJ` pin 19 to `INJLIMP` plus custom-device interconnections shown on the ignition/injection sheet | U11 is a MEMCAL-configured reference/sensor custom-device boundary associated with injection limiting/conditioning and MAP/OSC configuration | Injection computation or limiting rule, oscillator transfer, MAP transfer, thresholds, polarities, and the meaning of most CAL inputs |
| U12 `16034984` reference/control section | `REF` pin 29, `CYL` pin 11, `KNOCK` pin 15, `IGN` pin 23, `EST` pin 24, `BYPASS` pin 21, `INJLOOP` pin 52 and related analog/reference inputs | `INJREF` pin 18, `IGNREF` pin 19, `ESTLOOP` pin 25, `IRQ` pin 32, `PUMP` pin 10, `KNOCK` pin 14 | U12 participates in distributor-reference conditioning, cylinder configuration, EST/bypass feedback, knock conversion, and control signaling toward U9/U11 | REF-to-reference pulse decoding, CYL encoding, KNOCK threshold/filtering, EST truth table, pulse phase/polarity, and timing |
| U12 injector-driver section | `INJS` pin 47, `INJA` pin 46, `~LIMP` pin 20, `ESENSE`, `ISENSE+`, `ISENSE-` | `INJOUT` pin 2 to Q1 | U12 participates directly in injector actuation and current-sense/limiting hardware | Current-control algorithm, waveform, polarity, and distinction between `INJS` and `INJA` |

## U11 boundary

### Directly supported connections

The strongest current U11 anchors are:

- CAL42 -> 16055375 terminal 10 -> U11 pin 18 `OSC`.
- CAL61 -> 16055376 terminal 7 -> U11 pin 28 `MAP`; the conditioned MAP
  sensor path also reaches this pin.
- CAL59 -> 16055376 terminal 6 -> 100 Ohm -> `VIGN`; the reconstructed network
  places 24 kOhm between CAL59 and the CAL55/CAL57 common node, which reaches
  U11 pins 4 and 3.
- U11 receives U12-generated `INJREF` and `IGNREF` at pins 5 and 6.
- U11 receives directly visible sensor inputs including TPS and CTS.
- U11 outputs `INJ` at pin 19 to the `INJLIMP` net.

These connections prove that U11 combines fixed MEMCAL configuration,
reference activity, and sensor signals at one custom-device boundary. They do
not identify the equations, thresholds, or timing performed inside U11.

### What must not be inferred

The current evidence does not support any claim that:

- CAL42 establishes the processor or U9 counter clock;
- the 91-kOhm CAL42-to-CAL34 relationship directly determines a known
  frequency;
- CAL61 is a processor-readable MAP channel or is the producer of firmware
  load `$0063`;
- CAL59's VIGN bias proves a specific gain, threshold, or range;
- U11's `INJ` output can be reconstructed from TPS, CTS, MAP, reference, and
  CAL inputs without additional evidence.

The firmware's principal load producer is independently listing-backed as a
MAF/reference-period calculation, so the U11 MAP boundary must remain distinct
from `$0063` load semantics.

## U12 boundary

### Distributor reference

The schematic shows conditioned external `ESTHI/ESTLO` reaching U12 pin 29
`REF`. U12 outputs `INJREF` and `IGNREF` at pins 18 and 19 to both U9 and U11.
The firmware independently consumes reference occurrence and period state in
the U9-associated `$3FC0-$3FFF` register window:

- `$CAC6-$CAD3`: `$3FFA` reference-status consumption;
- `$CB5A-$CB5D`: `$3FC0` reference-period copy;
- `$CDE6-$CE41`: period normalization and RPM derivation;
- `$3FC8`: reference/spark-period state used by spark processing.

The endpoints are direct. The U12 transformation from `REF` into `INJREF` and
`IGNREF`, including pulse count, phase and polarity, remains unresolved.

### Cylinder configuration

CAL56 -> 16055376 terminal 13 -> U12 pin 11 `CYL` is a direct/mechanically
derived path. The photo-authoritative resistor network places 7.5 kOhm between
CAL56 and unresolved CAL53.

Firmware independently masks `$002F` with `$18` and compares the result with
PROM byte `LC225` at `$F682-$F68B`; mismatch feeds Error 41 at
`$E6A1-$E6AA`. PROM byte `LC009` separately controls reference-period
normalization at `$CDE6-$CDF5`.

The overall hardware/software cylinder-consistency relationship is strongly
supported, but CAL56 voltage encoding, U12 decoding, `$002F` bit provenance,
and any direct relationship between CAL56 and `LC009`/`LC225` remain
unresolved.

### EST/bypass feedback

U12 directly receives `IGN`, `EST`, `BYPASS`, and conditioned `REF`, and outputs
`ESTLOOP` to U9. Firmware writes and reads the U9-associated spark/control
window including `$3FDC`, `$3FE4`, `$3FE6`, `$3FE8`, `$3FEC`, `$3FF6`, and
`$3FFC`, and Error 42 evaluates feedback consistency around EST/bypass
transitions.

This proves a closed hardware/software control envelope. It does not reveal the
U12 EST/bypass truth table, electrical polarity, or exact correspondence
between `ESTLOOP` and firmware feedback state.

### Knock path

CAL32 and external `KNOCK#` meet at U12 pin 15. U12 pin 14 outputs `KNOCK` to
U9 pin 42. Firmware uses changes in U9-window counter `$3FCA` as the knock-event
quantity at `$D0D1-$D157`, applies retard in `$00A5`, recovers it at
`$EB3A-$EB59`, and evaluates Error 43 through `$EB5B-$EBB1` and
`$E6DF-$E6E9`.

The U9 pin-42-to-`$3FCA` relationship is a strong inference from coherent
hardware and firmware endpoints, not a documented pin/register mapping. U12's
threshold, filtering, polarity, and CAL32's exact electrical role remain
unknown. The separate CAL29/U10-AN9 analog ESC path must not be substituted for
this missing transformation.

### Injector driver

U9 outputs `INJS` and `INJA` to U12. U12 receives current-sense signals and
`~LIMP`, then drives `INJOUT` to Q1. Firmware writes synchronous pulse width to
`$3FD0`, uses sampled status `$00A0` bit 6 for injector service, and factory
test independently exercises `$3FCE` as EFI delay.

The processor-visible command side is listing-backed, while U9/U12 waveform,
phase, bank/double-fire details, current limiting, and driver polarity remain
custom-device boundaries.

## Consolidated black-box theory

The supported black-box theory is therefore:

1. U12 accepts conditioned distributor/reference, cylinder, knock, EST/bypass,
   and injector-control signals and presents reference/feedback/knock/driver
   signals to U9, U11, and the external injector stage.
2. U9 exposes processor-visible timing, status, spark, injection, and knock
   state through the `$3FC0-$3FFF` window.
3. U11 accepts U12-generated reference signals, sensor inputs, and fixed MEMCAL
   configuration and participates in the injection-limiting/custom analog
   boundary.
4. The firmware consumes and commands the U9-visible side exactly where the
   listing establishes it.
5. No current evidence justifies replacing U11 or U12 with an inferred internal
   behavioral model.

This envelope is sufficient for theory-of-operation documentation and for
preserving a clean HAL boundary. It is not sufficient for transistor-,
comparator-, counter-, or waveform-level emulation of either custom device.

## Highest-value unresolved questions

1. What electrical reference do CAL34 and CAL53 provide to the OSC and CYL
   resistor networks?
2. What are the voltage/frequency relationships at CAL42/U11 `OSC` and
   CAL56/U12 `CYL` on a running ECM?
3. How does U12 transform `REF` into `INJREF` and `IGNREF`?
4. What is the exact U9 pin-to-register/status mapping for `INJREF`, `IGNREF`,
   `ESTLOOP`, and `KNOCK`?
5. What are the meanings and polarities of `INJS`, `INJA`, `INJLIMP`,
   `INJLOOP`, and `~LIMP`?
6. How does U11 use `OSC`, MAP, TPS, CTS, reference, and fixed CAL inputs to
   produce its visible output behavior?

## Simulation consequence

Step 136 does not justify a C implementation change. The existing architecture
is correct to preserve U11/U12 as F4 hardware boundaries while translating the
F0 firmware-visible register behavior and exposing only raw processor-visible
stimulus/observation through the HAL. Any future custom-device model should be
introduced only when new physical measurements or independent device
documentation closes one of the unresolved transformations above.
