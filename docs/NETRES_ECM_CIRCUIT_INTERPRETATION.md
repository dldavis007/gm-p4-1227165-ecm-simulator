# Measured NetRes candidates in the 1227165 ECM circuits

Step 180, 2026-10-06. This review connects the Step-179 preferred 15-resistor
16055375 and nine-resistor 16055376 candidates to the archived ECM schematic.
It does not establish the original hidden resistor topology or undocumented
U11/U12 transfer functions. No simulator behavior or candidate values change.

## Evidence and numbering

Directly inspected: ECM sheets 2 (Inputs), 4 (Ignition-Injection), 5
(Connectors), and 6 (Power Supply), in
`evidence/hardware/1227165-schematics/`; the original
`evidence/memcal/MemCal, Cal connections.docx`; the BUA MEMCAL photograph;
and the emitted Step-179 SPICE circuits in `evidence/memcal/fitted_models/`.
The photograph supports package identification/orientation, not every hidden trace.

Use original NetRes package terminal numbers. The established carrier mapping
puts carrier pins 1–33 on J4 odd pins via J4 = 2*carrier-1, and carrier pins
34–66 on even pins via J4 = 2*(67-carrier). The DOCX records signal connections
but leaves supply/ground entries blank and contains a repeated CAL40 where
CAL49 would be expected. Sheet 5 resolves these positions explicitly. Calling
J4 positions 34, 39, 46 or 53 “CAL” numbers obscured their electrical roles.
The conclusions below are conditional on this carrier-to-J4 orientation being
correct for the particular board; verify continuity on hardware before assembly.

## 16055375 terminal map

| NetRes pin | J4 pin | ECM connection |
| --- | --- | --- |
| 1 | 33 | U11 pin 22 |
| 2 | 35 | U11 pin 21 |
| 3 | 37 | U11 pin 9 |
| 4 | 39 | Ground |
| 5 | 41 | U11 pin 1 |
| 6 | 43 | U11 pin 2 |
| 7 | 45 | U11 pin 8 |
| 8 | 46 | Ground |
| 9 | 44 | U11 pin 13; also pin 26 and motherboard VIGN divider |
| 10 | 42 | U11 pin 18, OSC |
| 11 | 40 | U11 pin 10 |
| 12 | 38 | U11 pin 11 |
| 13 | 36 | U11 pin 12 |
| 14 | 34 | VCC |

The common pin 14 is **VCC**, not ground. The measured 7–8 short grounds U11
pin 8. Pins 4 and 8 are separate terminals on the isolated network but become
the same ground net when installed; isolated pair measurements must therefore
not be compared directly with installed-board resistance readings.

The 31.6-kΩ ground branch at pin 5, 178-kΩ VCC branch, 45.3-kΩ branch to pin 6,
267-kΩ pin-6-to-pin-9 branch, and pin-9 branches of 71.5 kΩ to ground and
412 kΩ to VCC form a coupled bias network. This is not simply a list of
independent trim resistors. In particular, the repaired 6–9 coupling joins
U11 pins 2 and 13 and changes their interaction.

Sheet 4 adds 51.1 kΩ from VIGN and 5.49 kΩ to ground at pin 9's node, with a
capacitor there. Omitting these motherboard components gives the wrong installed
DC interpretation. The pin-1/2/3 group connects to VCC through 68.1 kΩ and
14.3 kΩ, with 12.7/34.8-kΩ links and a weak 1.5-MΩ ground return. With negligible
chip loading this group sits near VCC; differences in resistance still matter
if U11 draws current or drives these pins.

The 88.7-kΩ pin-10-to-VCC branch supplies U11's OSC terminal. Pins 11, 12 and
13 have VCC pull-ups of 95.3, 9.76 and 16.9 kΩ. These values support bias/current
setting roles, but do not identify their internal functions. Sheet 4 shows a
0.033-µF capacitor at U11 pin 14 labeled C. Without the internal oscillator
circuit, neither 1/(RC) nor a 555-style equation is an established frequency law.

## 16055376 terminal map

| NetRes pin | J4 pin | ECM connection |
| --- | --- | --- |
| 1 | 49 | VCC |
| 2 | 51 | U11 pin 24, also pin 25 on sheet 4 |
| 3 | 53 | Ground |
| 4 | 55 | U11 pin 4 |
| 5 | 57 | U11 pin 3; already joined to pin 4 on motherboard |
| 6 | 59 | VIGN through motherboard 100 Ω; capacitor to ground |
| 7 | 61 | U11 pin 28 labeled MAP |
| 8 | 63 | Conditioned ECM MAP signal; isolated inside this NetRes |
| 9 | 64 | Conditioned ECM TPS signal |
| 10 | 62 | Ground; isolated inside this NetRes |
| 11 | 60 | U11 pin 7 |
| 12 | 58 | VCC |
| 13 | 56 | U12 pin 11, CYL |
| 14 | 54 | No further connection shown on sheet 5 |
| 15 | 52 | Capacitor in series to U11 pin 16 |
| 16 | 50 | U11 pin 17 |

The nine resistors have straightforward circuit interpretations:

| Candidate branch | Installed circuit interpretation |
| --- | --- |
| 1–2, 150 kΩ; 2–3, 1.4 kΩ | VCC-to-ground divider feeding U11 pins 24/25 |
| 1–16, 10 kΩ | VCC pull-up to U11 pin 17 |
| 3–4, 130 kΩ; 4–6, 24.9 kΩ | VIGN-derived divider feeding U11 pins 3/4 |
| 11–12, 10 kΩ | VCC pull-up to U11 pin 7 |
| 3–13, 7.5 kΩ | Ground return on U12 CYL configuration input |
| 3–14, 4.99 kΩ | Ground return at J4 pin 54; no active destination shown |
| 3–15, 8.25 kΩ | Ground return on the MEMCAL side of the U11-pin-16 series capacitor |

The 7.5-kΩ CYL resistor is a pull-down/configuration load, not a divider to an
unknown reference. Its actual CYL voltage and decoding still require U12's
internal bias circuit. The pin-15 resistor and series capacitor suggest an
AC-coupling/time-constant role; the capacitor value and chip impedance are
needed to calculate it. This is not a DC pull-down directly on U11 pin 16.

### The MAP/TPS finding

The measured short 7–9 ties J4 pin 61 to pin 64. Sheet 5 identifies pin 64 as
TPS; sheet 4 identifies pin 61 as U11 pin 28, labeled MAP. Consequently this
MEMCAL topology feeds **conditioned TPS to the U11 terminal labeled MAP**.
U11 pin 20 independently also receives TPS. Sheet 2 shows the conditioned TPS
node after the 10-kΩ input series resistor. J4 pin 63 carries the actual
conditioned MAP node, but NetRes pin 8 is isolated. The network does not short
external MAP and TPS together.

This is a schematic-plus-measurement deduction, not proof of U11's internal
meaning. A reused/custom-chip pin label could describe another application,
but the reason for this routing is not documented. It is consistent with the
MAF-equipped BUA application using throttle information in the auxiliary
injection circuit. Do not treat that explanation as established, rename the
physical pin, or silently swap the measured jumper to MAP. Priority bench check:
verify J4-61-to-64 continuity and J4-61-to-63 isolation on the original MEMCAL,
then observe whether U11 pin 28 tracks TPS rather than MAP while powered.

## Illustrative DC calculations

`python tools/check_netres_ecm_bias.py` parses the emitted 375 SPICE resistor
values, solves its nodal equations, and reports the following. VCC = 5 V and
VIGN = 12 V are illustrative assumptions, not measurements. U11/U12 input
currents, internal clamps and active behavior are omitted; capacitors are open
at DC. These are candidate-network predictions, not claimed operating voltages.

| 375 terminal / U11 pin | Network alone | Including ECM 51.1-kΩ/5.49-kΩ divider |
| --- | ---: | ---: |
| 1 / 22 | 4.975 V | 4.975 V |
| 2 / 21 | 4.971 V | 4.971 V |
| 3 / 9 | 4.958 V | 4.958 V |
| 5 / 1 | 0.753 V | 0.783 V |
| 6 / 2 | 0.751 V | 0.833 V |
| 9 / 13,26 | 0.742 V | 1.127 V |

Unloaded 376 pin 2 is VCC*1.4/(150+1.4), or 46.2 mV at 5 V. Unloaded pin 4/5
is VIGN*130/(130+24.9+0.1), or 10.065 V at 12 V, including the motherboard
100-Ω resistor. That latter result is a reason to investigate U11 input loading
or clamps, not proof the custom chip actually sees 10 V. It is connected to a
VIGN-derived circuit, not a simple 5-V logic input. Pull-up terminals approach
VCC only in the no-load model; their resistor values govern delivered current.

## What the networks appear to do

Established externally: they ground/select some custom-chip terminals, supply
pull-ups, set coupled U11 biases, provide VIGN-derived bias, connect OSC,
configure U12 CYL, and route TPS to a U11 terminal. U11 also receives injector
and ignition references, TPS and CTS, and emits INJ/INJLIMP. Thus a MEMCAL-specific
analog injection/backup-conditioning role is a strong interpretation, alongside
U12's reference/cylinder configuration. Exact fallback fueling, oscillator
frequency, thresholds, cylinder-code decoding and sensor transfer equations
remain unresolved. These resistors are outside the EPROM's programmable fuel
and spark tables and are not themselves the knock-filter circuit.

The new topology is consistent with the visible circuit roles and explains more
than the earlier isolated-resistance view. Its DC measurement accuracy remains
Step 179's result, not proof of powered/dynamic equivalence. Retain the original
measured jumper topology and preferred values; verify isolated pair resistances,
then installed continuity and powered node voltages. The most useful next evidence
is the original MEMCAL's TPS/MAP continuity and loaded U11/CYL/OSC measurements.
