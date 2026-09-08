# MEMCAL-to-motherboard-to-firmware paths

## Scope and evidence language

This document follows selected CAL connections from the photograph-authoritative
MEMCAL reconstruction, through the 1227165 motherboard schematic, and toward
observable firmware behavior. It does not infer undocumented internal circuitry
for U11 (`16054995`) or U12 (`16034984`). A resistor relationship establishes an
electrical connection, not its transfer function or functional meaning.

The evidence labels used below are deliberately narrower than the F0-F4 fidelity
classes in `HARDWARE_FIRMWARE_CROSS_REFERENCE.md`:

- **Direct evidence**: a visible schematic connection, emitted PROM byte, or
  executable listing instruction.
- **Mechanically derived**: follows from the physically established 66-pin
  carrier numbering and odd/even J4 interleave.
- **Strong inference**: multiple direct paths agree, but an undocumented custom
  device prevents a complete causal proof.
- **Unresolved**: the available evidence does not establish the behavior.

## Path summary

| Starting point | Motherboard path | Firmware-visible consequence | Evidence conclusion |
| --- | --- | --- | --- |
| CAL42 / 16055375 terminal 10 | U11 pin 18 `OSC`; reconstructed 91 kOhm branch to CAL34 | No direct CPU read or independently identified register consequence | Connection direct/mechanically derived; oscillator behavior unresolved |
| CAL56 / 16055376 terminal 13 | U12 pin 11 `CYL`; reconstructed 7.5 kOhm relationship to CAL53 | MPU status `$002F & $18` is compared with PROM fuel-mode byte `LC225` at `$F682-$F68B`; mismatch sets the Error-41 precursor bit, later latched at `$E6A1-$E6AA` | Hardware and software endpoints direct; causal encoding through U12/MPU strongly inferred, exact code unresolved |
| CAL61 / 16055376 terminal 7 | U11 pin 28 `MAP`; same MEMCAL node as J4 64; motherboard MAP sensor path also reaches U11 pin 28 | Firmware uses MAP/load-derived state extensively, but no instruction proves how U11 transforms or uses the CAL61 node | Connection direct/mechanically derived; transfer and firmware consequence unresolved |
| CAL59 / 16055376 terminal 6 | 100 Ohm to `VIGN` plus shunt capacitor; 24 kOhm network relationship to the CAL55/CAL57 common node | No direct CPU-readable CAL59 state found | Direct bias/supply connection; downstream U11 effect unresolved |
| CAL29 | 100 Ohm from `ESC#`; capacitor to ground, 4.02 kOhm pull-up, 200 kOhm to `ESC`; `ESC` reaches U10 A/D input AN9 | Firmware Error 43/knock logic uses hardware-derived state plus load, coolant, RPM, battery and timing evidence | Analog ESC hardware path direct; exact A/D-to-error linkage is incomplete |
| CAL32 | U12 pin 15 `KNOCK`, with 470 kOhm to ground; U12 pin 14 drives net `KNOCK` to U9 pin 42 | Knock event count enters `$D0D1-$D157`; knock-failure timing/flags occur at `$EB3A-$EBB1` and `$E6DF-$E6EC` | Endpoints direct; U12/U9 event conversion unresolved |

## CAL42: oscillator configuration boundary

The carrier/J4 mapping places 16055375 terminal 10 at CAL42. The reconstructed
external-terminal equivalent has a 91 kOhm CAL42-to-CAL34 branch. The ignition
and injection schematic connects CAL42 to U11 pin 18, labelled `OSC`.

Those facts establish a configurable oscillator-related electrical path. They
do **not** establish frequency, duty cycle, threshold, timing range, or which
U11 outputs are affected. CAL34's motherboard destination is also unresolved.
No firmware instruction directly reads CAL42, so this path remains an F1
connection terminating at an F4 custom-device boundary.

## CAL56: physical cylinder selection and firmware consistency check

The mechanically derived mapping places 16055376 terminal 13 at CAL56. The
photo-authoritative functional reconstruction connects CAL56 to CAL53 through
7.5 kOhm. The motherboard schematic connects CAL56 directly to U12 pin 11,
labelled `CYL`.

The executable firmware contains two distinct cylinder-related mechanisms:

1. PROM byte `LC009` at `$C009` is `0` for the supplied eight-cylinder
   calibration. At `$CDE6-$CDF5`, the zero/nonzero choice selects how the
   reference period is normalized for RPM calculation.
2. At `$F682-$F68B`, firmware masks MPU-derived RAM `$002F` with `$18`, compares
   the result with `LC225` at `$C225` (zero in this image), and sets `$0040` bit
   0 on mismatch. `$E6A1-$E6AA` transfers that precursor into Error 41 and
   clears the transient flag.

The schematic's `CYL` input and the executable cylinder-mode consistency check
strongly support the interpretation that fixed MEMCAL hardware configuration
and PROM software configuration are expected to agree. The missing link is
inside U12 and the custom MPU: the evidence does not document the CAL56 voltage
encoding, U12's decoding rule, or the exact origin/meaning of `$002F` bits 3-4.
Accordingly, CAL56 must not be equated directly with `LC009` or `LC225`.

## CAL61 and CAL59: MAP-related configuration and ignition bias

The mechanically derived mapping places 16055376 terminal 7 at CAL61 and
terminal 6 at CAL59. The reconstructed network makes CAL61 common with J4 64.
The motherboard schematic connects CAL61 to U11 pin 28, labelled `MAP`; the
conditioned MAP sensor signal also reaches that U11 pin. This directly proves a
MAP-related U11 boundary but not whether the MEMCAL branch is a pull-up,
scaling, threshold, range, or variant-selection element.

CAL59 is connected through 100 Ohm to `VIGN` and has a shunt capacitor on the
motherboard. In the reconstructed 16055376 network, CAL59 is connected through
24 kOhm to the CAL55/CAL57 common node, which reaches U11 pins 4 and 3. This is
a direct supply/bias anchor useful for validating the network electrically.
The internal U11 consequence and any effect on MAP or injection remain
unresolved.

## CAL29/CAL32: ESC and knock paths must remain separate

The input schematic shows two related but distinct paths:

- `ESC#` passes through 100 Ohm to CAL29, with a capacitor to ground, a 4.02
  kOhm pull-up, and a 200 kOhm connection to analog `ESC`; `ESC` reaches U10
  A/D channel AN9.
- CAL32 reaches U12 pin 15 `KNOCK`, with 470 kOhm to ground. U12 pin 14 drives
  the net `KNOCK`, which reaches U9 pin 42 on the processor schematic.

Firmware applies an event count to knock attack at `$D0D1-$D157` and maintains
knock-failure timing/status at `$EB3A-$EBB1` and `$E6DF-$E6EC`. These executable
paths are direct evidence of processor-visible knock behavior. The schematics
are direct evidence for the analog ESC and digital/custom-device knock routes.
The internal conversion performed by U12 and U9, and the precise software
source of every failure flag, remain unresolved; the resistor networks alone
cannot fill those gaps.

## Firmware and simulator boundary

The existing C implementation correctly treats custom-device signal production
as a boundary while translating the listing-backed RPM, cylinder-consistency,
knock attack/recovery, and diagnostic behavior. No C source or behavioral
baseline change is justified by this cross-reference. A future hardware model
may expose raw processor-visible inputs, but it must not encode an invented U11
or U12 transfer function.

## Targeted future evidence

- Measure the complete 16055375 terminal-to-terminal resistance matrix to
  distinguish external equivalence from literal LTspice internal topology.
- Trace CAL34, CAL53, J4 62, J4 63, and J4 64 on the motherboard where possible.
- Identify `$002F` bits 3-4 from independent MPU documentation or controlled
  hardware observations.
- Observe CAL56/U12 cylinder-selection levels across known MEMCAL variants.
- Correlate U9 knock-event state, U10 AN9 ESC readings, and firmware RAM during
  controlled knock/ESC tests.

