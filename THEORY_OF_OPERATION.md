# GM P4 1227165 ECM simulator — theory of operation

## Document control

| Field | Value |
| --- | --- |
| System | GM P4 engine control module (ECM), service number 1227165 |
| Firmware basis | Supplied 9340 image and corrected BUA source/listing provenance |
| Firmware authority | `evidence/firmware/bua-hac.lst` |
| Implementation | Strict-C89 behavioral port and deterministic PC simulator |
| Revision | 1.3 — Step 183, 2026-10-06 |
| Status | Controlled engineering publication |
| Detailed authority | Linked reference chapters and hardware/firmware cross-reference |

This document is the master operational description of the repository's
evidence-supported system. It explains how the supplied program image,
processor-visible hardware, MEMCAL, portable C implementation, and simulator
fit together. It consolidates the sixteen completed theory chapters; it does
not replace their detailed address, net, calibration, implementation, and
regression records.

The behavioral port is closed against the supplied image at the documented
processor-visible boundary. That conclusion does **not** claim cycle-exact
processor emulation, complete custom-device reconstruction, electrical
equivalence, a production engine model, or safety qualification for connection
to physical hardware.

## 1. Scope and evidence method

### 1.1 Controlled identity

The hardware identity is ECM service number 1227165. The program basis is the
corrected BUA source family associated with the supplied 9340 material. `BUA`
is retained as source and label text; it is not, by itself, proof that every
external artifact represents the same calibration, MEMCAL, or board variant.

The authoritative executable record is
[`evidence/firmware/bua-hac.lst`](evidence/firmware/bua-hac.lst). Its emitted
instructions, bytes, addresses, targets, tables, and execution order control
firmware claims. `bua-hac.txt` is excluded because its corrected-revision status
has not been established. Source provenance and image differences are recorded
in the [evidence register](docs/reference/EVIDENCE_REGISTER.md) and
[`BUA_SOURCE_PROVENANCE.md`](evidence/firmware/BUA_SOURCE_PROVENANCE.md).

### 1.2 Evidence order and claim status

When evidence conflicts, this work uses the following order:

1. verified emitted instructions, addresses, and bytes;
2. calibration data together with its executable use;
3. repeatable tests, measurements, and independent cross-checks;
4. source labels and comments;
5. secondary descriptions and historical research.

Claims use the repository statuses **Confirmed**, **Strongly supported**,
**Provisional**, **Variant-dependent**, **Unknown**, and **Boundary**. Status
belongs to an individual claim, not to an entire artifact. Observable behavior
is established before a functional name is assigned. Missing device
documentation remains an explicit boundary rather than being completed by
analogy.

### 1.3 Fidelity classes

| Class | Meaning in this repository |
| --- | --- |
| F0 | Listing-backed firmware state, arithmetic, ordering, branch, table, or raw write preserved by the port. |
| F1 | Directly supported hardware connection, schematic path, physical construction, or MEMCAL relationship. |
| F2 | Raw processor-visible input, event, register state, or output observation at the hardware-abstraction-layer (HAL) seam. |
| F3 | Optional simulation assumption: sensor transfer, electrical response, engine/vehicle dynamics, actuator response, or other plant behavior. |
| F4 | External or unresolved behavior: undocumented custom-device internals, unavailable code, physical power/reset effects, or uncharacterized circuitry. |

The governing flow is:

```mermaid
flowchart TD
    P["Physical system / plant — F3/F4"] -->|raw stimuli| H["HAL boundary — F2"]
    H --> C["Firmware behavior — F0"]
    C -->|raw commands| H
    E["Proven connections — F1"] --- H
```

The simulator may add an F3 plant around the F0 core, but an F3 result never
upgrades itself into firmware or hardware evidence.

## 2. System architecture

The ECM combines a processor, volatile and retained state, external program
memory, processor-visible custom peripherals, a serial analog-to-digital (A/D)
converter, discrete and event inputs, output drivers, and a removable MEMCAL.
The supplied program window is `$8000-$FFFF`; the interface exposes `A0-A14`,
`D0-D7`, `/ROMCS`, and `/ROMOE`. Eight vector words occupy `$FFF0-$FFFE`.

The custom-device boundary is central to the architecture:

- **U9** exposes the `$3FC0-$3FFF` processor-visible timing/register window.
  Firmware use establishes particular addresses for reference, injection,
  spark, dwell, knock, and status. It does not establish U9's internal timer
  topology or a complete pin-to-register specification.
- **U10** (`16034988`) is the serial A/D boundary. Firmware supplies a selector
  and receives an 8-bit raw sample through `$F1BE-$F1DF`.
- **U11** receives MEMCAL configuration and participates in oscillator, MAP,
  injector-limit, and reference-related connections. Its internal transfer
  functions remain F4.
- **U12** participates in distributor reference, cylinder/knock selection,
  ignition/EST feedback, and injector drive/current-control paths. Its internal
  thresholds, polarity, phase, and control laws remain F4.

The [processor and custom-peripheral chapter](docs/reference/PROCESSOR_MEMORY_CUSTOM_PERIPHERAL_ARCHITECTURE.md),
[U9 map](docs/reference/U9_REGISTER_WINDOW_MAP.md), and
[U11/U12 boundary chapter](docs/reference/U11_U12_FUNCTIONAL_BOUNDARIES.md)
are controlling references. Unclassified locations in `$3FC0-$3FFF` are not
assumed to be unused or reserved.

## 3. MEMCAL and calibration architecture

The MEMCAL is both program/calibration carrier and physical configuration
hardware. Its 66-contact relationship to motherboard connector J4, EPROM, two
resistor networks, populated jumpers, and CAL paths are part of the system
architecture. Original owner-confirmed resistance measurements control replacement
resistance targets. Photographs and direct physical evidence control the earlier
as-built reconstruction where legible. KiCad, LTspice, and historical notes
provide supporting evidence; discrepancies remain visible.

Step 179 records the owner's confirmation that the preserved workbook contains
original NetRes pin-to-pin measurements and defines the resistance target.
The earlier hand reconstruction is approximate. New passive DC candidates
closely match the measured matrices but still require physical validation.
Photographs remain authoritative for the earlier as-built population.
See the [reconstruction report](docs/NETRES_MEASURED_MATRIX_RECONSTRUCTION.md).

Important established anchors include CAL42 to U11 pin 18 `OSC`, CAL56 to U12
pin 11 `CYL`, CAL61 to U11 pin 28 `MAP`, CAL59 to VIGN, and the CAL45/CAL46
common node. These are connection or configuration statements, not licenses to
invent the receiving devices' internal behavior. In particular:

- CAL42/U11 `OSC` is not automatically the U9 timer clock;
- CAL56/U12 `CYL` is not a PROM byte and must not be equated with `LC009` or
  `LC225`;
- CAL29's analog ESC route is distinct from the CAL32/U12/U9 knock-event route;
- resistor-network values or topology that remain visually or electrically
  ambiguous remain unresolved.

The detailed reconstruction is in
[MEMCAL architecture](docs/reference/MEMCAL_ARCHITECTURE.md),
[functional networks](docs/reference/MEMCAL_FUNCTIONAL_NETWORKS.md), and
[motherboard/firmware paths](docs/reference/MEMCAL_MOTHERBOARD_FIRMWARE_PATHS.md).

### 3.1 MEMCAL/U11 bias, configuration, and injection block

**Schematic location.** Sheets 4 (Ignition-Injection) and 5 (Connectors),
U11 `16054995`, U12 `16034984`, J4, and both NetRes packages. Original package
terminal numbering and complete mappings are in the
[Step-180 circuit review](docs/NETRES_ECM_CIRCUIT_INTERPRETATION.md).

**Electrical operation.** For the preferred measured-matrix candidate, 375
pin 14 is VCC; pins 4 and 8 are ground. The measured 7–8 link grounds U11
pin 8. The pin-5/6/9 resistor group is a coupled bias network, including the
267-kΩ branch between U11 pins 2 and 13. The motherboard adds 51.1 kΩ from
VIGN and 5.49 kΩ to ground at the pin-9/U11-13/26 node, so its voltage is not
set by the MEMCAL alone. The 88.7-kΩ branch pulls U11 OSC toward VCC; the
0.033-µF capacitor is on the separately labeled U11 C terminal. An internal
oscillator equation is not established.

The 376 network provides VCC pull-ups, a 150-kΩ/1.4-kΩ divider, a VIGN-derived
24.9-kΩ/130-kΩ divider, and a 7.5-kΩ ground return at U12 CYL. Its 8.25-kΩ
resistor is on the MEMCAL side of the series capacitor leading to U11 pin 16.
The measured 7–9 link routes conditioned TPS at J4-64 to J4-61/U11 pin 28,
labeled MAP. Actual conditioned MAP at J4-63 is isolated inside this package.
This is a mapping deduction pending original-hardware continuity confirmation;
it does not short external MAP and TPS together.

**Firmware operation.** The NetRes resistors are fixed electrical configuration,
not programmable EPROM tables. Firmware consumes U9 reference occurrence and
period state (`$CAC6-$CAD3`, `$CB5A-$CB5D`) and derives RPM at `$CDE6-$CE41`.
Cylinder/configuration consistency checks at `$F682-$F68B` compare masked
`$002F` state with `LC225`; Error 41 qualification includes `$E6A1-$E6AA`.
These paths do not establish U12's analog CYL decoding or equate its resistor
with the separate normalization constant `LC009`. Ordinary firmware load remains
VMAF-derived, not a conversion of the NetRes's U11 MAP-labeled terminal.

**Complete signal path.** MEMCAL configuration and motherboard bias act at
U11/U12; engine references and sensors also reach these chips. U11 emits
INJ/INJLIMP. U12's reference and injector interfaces connect to U9 and the
injector output stage. Firmware commands processor-visible timing state; it
does not directly calculate the undocumented U11 analog transfer function.

**Fault behavior.** Incorrect resistors, missing ground links, or open carrier
contacts can alter configuration/bias even with a good EPROM. Isolated resistance
matches do not prove loaded voltages, oscillator timing or backup fueling.
The `~LIMP` net crosses U12, output gating, and power circuitry. ALDL mode,
firmware sensor substitutions, and hardware backup operation are separate
concepts. A MEMCAL-configured backup injection role is strongly supported by
these external connections; exact entry conditions and fueling equations remain
unknown. No claim is made that every diagnostic error asserts `~LIMP`.

**Evidence boundaries.** Candidate DC accuracy is established numerically against
the owner's original measurements; original hidden topology and powered/dynamic
behavior are not. Do not infer a frequency from 1/(RC), a cylinder count from
7.5 kΩ alone, or an injector pulse width from unloaded divider voltages.

## 4. Reset, startup, and retained state

Reset vectors enter the source-ordered power-on path at `$C800-$C9F3`. That
path clears the specified volatile state, samples socket/checksum and mode
conditions, selects factory or normal operation, handles the optional-ROM
decision, validates retained state, performs recovery when required, and enters
normal initialization. The port preserves this order rather than replacing it
with a generic initialization routine.

Retained validation includes the listing-defined checksum computation at
`$F3A7-$F3B4`. Recovery at `$F434-$F446` stores `$8000` in both save-area marker
words and initializes all sixteen block-learn-memory (BLM) cells to `$80`.
Normal startup then establishes controller state, initial sensor samples,
engine-running/reference state, injector and spark state, and initial idle-air
control (IAC) positioning before periodic operation.

Factory selection executes a separate listing-backed control and test path.
The port preserves its RAM fill, table, checksum, A/D scan, communications,
raw output exercises, and restart decisions where emitted code is available.
The external factory fixture and FMD exchange remain boundaries.

Physical VIGN/VBATT rail timing, keep-alive supply behavior, processor reset
electrical consequences, unavailable optional code at `$5800`, and vector
targets at external `$6000` remain F4. The simulator exposes decisions and
events at F2; it does not claim to reproduce those physical mechanisms.

### 4.1 Power rails, reset and retained-power block

**Schematic location.** Sheet 6, U1 `16034992`, battery-input diodes,
input/output capacitors, VIGN bias network and reset pull-up; sheet 1 U8/U9/U12
reset and standby connections; sheet 5 vehicle supply connectors.

**Electrical operation.** BATT# enters two diode-fed paths: VQUAD feeds output
stages, while VBATT feeds U1 VIN1/pin 11 and VIN2/pin 13 through a 3-ohm
series resistor and a 4.7-µF bypass capacitor. The diodes provide isolation;
the drawn suppression devices and capacitors support transient conditioning.
Their unlabeled characteristics do not establish protection limits. These
battery-derived rails must not be confused with regulated VCC or switched VIGN.

| U1 connection | Net and externally visible use |
| --- | --- |
| VCC1, pin 12 | VCC; logic, converters and MEMCAL supply connections |
| VCC2, pin 8 | VCC#; vehicle connector reference supply |
| VCC3, pin 4 | VCCSTANDBY; U8 standby connection and decoupling |
| RESET, pin 1 | ~RESET, with 3.3-kΩ pull-up; U8, U9 and U12 reset connections |
| POWERON, pin 2 | Node biased from VIGN through 51 kΩ, with capacitor |
| POWEROFF, pin 15 | Connected to ~LIMP from U12 pin 20 |

U1 pins 3 and 5 have additional bias/filter networks. Feedback/programming
resistors connect pins 6, 7, 9 and 10, but their internal roles are undocumented.
The schematic establishes separate rail connections, not exact regulation
voltages, dropout thresholds, reset pulse duration or standby switching rules.
The sheet-6 30-ohm VIGN feed to CAL30 belongs to MEMCAL circuitry and is separate
from the sheet-4 100-ohm VIGN feed to CAL59.

**Firmware operation.** Startup begins at `$C800`. The emitted sequence initializes
processor-visible registers and clears volatile RAM, selects factory paths before
retained-memory recovery, then validates/reinitializes retained error and learning
state as necessary. `$F3A7` sums retained error bytes `$0005-$0009`; invalid
retained state invokes the established recovery and IAC park initialization.
Firmware expects useful retained state across some power cycles; it does not
prove the physical supply behavior that preserves it.

The odd 12.5-ms path `$D6D1-$D769` recognizes ignition-off state and performs
housekeeping before terminal shutdown. The even path `$D370-$D3DB` closes and
parks IAC while the common 6.25-ms service executes motor steps. At the terminal
key-off timer boundary `LC012=$0385`, execution reaches SWI at `$D6EA`.
The vector table and software endpoint do not by themselves specify which rail
then switches off or how a physical restart occurs.

**Complete signal path.** Battery/VIGN and U1 establish rails/reset; reset permits
U8 startup; firmware validates RAM and enters the scheduler; ignition-off state
starts cleanup and IAC parking; the terminal software endpoint returns to the
unresolved physical power/reset boundary. This layered explanation avoids treating
key-off as instantaneous loss of all ECM power.

**Fault behavior.** Bad input, ground, regulator or reset connections can disturb
multiple subsystems together. Loss of retained supply could repeatedly force
startup recovery even when normal running power appears acceptable. These are
circuit-based troubleshooting hypotheses, not proof of a specific failed part.
Check VBATT, VCC, VCC# and VCCSTANDBY separately; compare reset and VIGN timing
with the actual startup/shutdown sequence rather than infer them from the names.

**Evidence boundaries.** Rail routing and software order are established. U1's
internal control truth table, undervoltage response, retention voltage, current
capacity and relationship between POWEROFF and actual rail removal remain unknown.

### 4.2 LIMP source, output gating and watchdog boundary

**Schematic location.** Sheet 1 U12 pin 20 `LIMP` with inversion bubble and net
`~LIMP`; sheet 3 U3/U5 gating; sheet 4 U12 repeated LIMP terminal; sheet 6
U1 POWEROFF. U11 does not have a directly drawn ~LIMP input on sheet 4.

**Electrical operation.** Sheet 1 identifies U12 pin 20 as the source of this
external net. The other occurrences of pin 20 are sections of the same U12,
not separate chips. The net reaches U3 output-enable/gating circuitry, U5 fan
logic and a U5 resistor/capacitor branch, and U1 POWEROFF. U3's drawn gates
share the LIMP control for its four driver channels. The fan path combines FAN
and ~LIMP through NAND/inverting logic; driver and external load polarity still
matter when translating that to a physical fan state.

The U5 delay branch has 365 kΩ and 2.7 µF, with a parallel 10-kΩ/diode route
for asymmetric charging/discharging. Nominal RC products are 0.986 s and 0.027 s;
these are component time scales, not validated limp-entry or reset delays. The
second gate output is shown without a destination on this sheet, so this branch
cannot be assigned a complete shutdown/watchdog function from the drawing alone.

**Firmware operation.** The listing exposes watchdog-related service boundaries:
`$F21A` modifies the FMD/SPI control byte, and emitted writes to `$400B` include
`$F349-$F34C` during checksum work and `$FD23-$FD28` in factory execution.
The literal value is `$FF00`. This establishes servicing operations, not the
watchdog's timeout, its internal implementation or a proven direct path to
U12 LIMP. Vector entries targeting `$C800` establish software restart destinations;
exception labels in comments do not independently establish the triggering physics.

**Complete signal path.** Software service and reset/I/O signals interact with
custom peripherals; U12 presents the external LIMP signal; the signal affects
output gates and U1. The missing link is the documented U12 rule that turns a
lost service sequence, reset event or other condition into an asserted LIMP net.
Neither the firmware nor the external schematic alone closes that link.

**Fault behavior.** A stopped processor/watchdog-service failure is a plausible
backup-entry cause, but is not confirmed as the only cause or a quantified
trigger in this repository. Sensor diagnostic codes and ALDL mode do not provide
proof of an asserted LIMP signal. The owner's DIAG solder repair therefore remains
a mode-selection fault rather than a verified hardware-limp episode.

**Evidence boundaries.** The physical net source and destinations are established.
Exact assertion/release conditions, delay, output defaults and power effects
remain unknown. Useful next bench evidence is a simultaneous trace of U12-20,
~RESET, the supply rails and injector output during normal startup/key-off and
an independently controlled loss of processor service. Do not invent a LIMP
register bit or change the simulator to force hardware fallback from every fault.

## 5. Runtime scheduler and event model

Ordinary IRQ service has a 6.25-ms cadence. The source alternates odd and even
minor paths, so each minor path executes every 12.5 ms. A one-of-sixteen major
dispatcher selects one segment per IRQ; each particular major segment recurs
every 100 ms. Common service ordering, including the 6.25-ms IAC motor
executor, is preserved.

| Execution domain | Established cadence or trigger | Principal role |
| --- | --- | --- |
| Common IRQ | 6.25 ms | common acquisition/service, IAC one-step execution, shared state |
| Odd minor | 12.5 ms | airflow/load/fuel-oriented work |
| Even minor | 12.5 ms | reference/spark/O2-oriented work |
| Major segment | one of 16 per IRQ; 100 ms per segment | slower state machines, diagnostics, outputs, accessories |
| Reference event | external engine event | occurrence/period/counter state for RPM, injection, dwell, and spark |

Reference events are not scheduler ticks. A host or target supplies raw engine
events/state through the HAL; firmware consumes them in the established source
order. Collapsing both domains into one periodic host loop changes observable
firmware behavior. The detailed order is documented in
[Interrupt and scheduler architecture](docs/reference/INTERRUPT_SCHEDULER_ARCHITECTURE.md).

## 6. Input acquisition and signal processing

Firmware explicitly selects U10 channels and receives an 8-bit sample. Normal
operation does not perform one continuous universal scan; cadence depends on
common, minor, major, startup, or factory execution context.

| Selector | Channel/net | Supplied-image role |
| ---: | --- | --- |
| `$00` | AN0 `MAP2` | factory scan; no explicit normal request found |
| `$10` | AN1 `VOLT` | voltage/battery-related processing |
| `$20` | AN2 `O2` | oxygen acquisition and filtering |
| `$30` | AN3 `MAP` | factory scan; no explicit normal request found |
| `$40` | AN4 `CTS` | coolant conversion |
| `$50` | AN5 `TPS` | throttle acquisition, learning, normalization |
| `$60` | AN6 `PUMPVOLT` | pump-voltage diagnostics |
| `$70` | AN7 `DIAG` | mode selection |
| `$80` | AN8 `MAT` | air-temperature processing |
| `$90` | AN9 `ESC` | factory scan; no explicit normal request found |
| `$A0` | AN10 `VMAF` | analog mass-airflow path |
| `$B0` | unresolved | factory scan only; channel identity unresolved |

Raw counts, learned references, filtered state, and engineering-unit or plant
interpretation remain separate. Representative states include TPS raw `$0081`
and normalized `$0082`, VMAF raw `$00ED` and airflow `$00EA:$00EB`, voltage
`$007E`, pump voltage `$007F`, MAT `$012B/$0060`, and current load `$0063`.

The supplied image's principal load path is VMAF-derived airflow multiplied by
reference period `$0095:$0096` at `$D769-$D7A0`. Physical MAP/MAP2 resources
and the CAL61/U11 terminal labeled `MAP` exist (the measured 376 jumper
routes TPS to that terminal), but the evidence does not make them the ordinary
producer of `$0063`. Vehicle speed and distributor reference are discrete/event
paths, not A/D channels.

### 6.1 CTS and MAT temperature-input blocks

**Schematic location.** Sheet 2 CTS#/CTSHI/CTS and MAT#/MAT networks;
sheet 1 U12 CTSHI output/pin 49 and U10 AN4/pin 5, AN8/pin 9.

**Electrical operation.** MAT# is biased toward the logic supply through 1.00 kΩ,
then reaches MAT through 10 kΩ with a capacitor to ground. Assuming a passive
sensor resistance R to ground, negligible input loading and a stiff supply,
V_MAT = V_SUPPLY*R/(1000+R). The series resistor has negligible unloaded DC
drop but isolates the input and contributes to filtering. For an NTC sensor,
heating lowers resistance and voltage. The sensor curve, ground offset and input
leakage must be measured or specified before assigning temperatures to voltages.

CTS# joins a 348-ohm resistor to CTSHI; CTSHI itself has a 3.65-kΩ pull-up and
rail-clamp diodes. CTS# reaches the analog CTS node through 10 kΩ and a filter
capacitor, then U10 and U11. When CTSHI is high impedance, the visible pull-up
path is approximately 3.65k+348 = 3.998 kΩ. If CTSHI is actively held near the
supply, the effective pull-up approaches 348 Ω. The second case depends on
U12's output behavior; the schematic alone does not specify output resistance.
Changing bias changes the voltage for the same thermistor and improves useful
conversion range. A voltage step at a range change need not mean a temperature
step. Do not replace both ranges with one universal sensor-to-count formula.

**Firmware operation.** Segment 6 requests `$40` at `$F3B9-$F3BB`. At
`$F3BF-$F3EA`, selection depends on `$0030` bit 0 and `$003C` bit 7; the emitted
paths use tables `$FEA7` and `$FEB8`. One path adds ten counts with saturation
before lookup. The result goes to `$005D`. A result greater than 120 sets
`$0030` bit 0; a result at most 106 clears it; 107 through 120 retain it.
These exact byte boundaries establish hysteresis. Comments associate the
thresholds with temperature, but the count/table arithmetic controls the claim.
The software selection bit is carried through the existing FMD/SPI boundary;
it is not itself a measurement of CTSHI's electrical level.

MAT acquisition at `$EBB3` requests `$80`; firmware complements the sample,
stores the complemented byte at `$012B`, produces `$0060` and qualifies Error
23/25. CTS processing updates coolant state `$005B-$005F` and Error 14/15
qualification. Complemented MAT, raw A/D and processed temperature are different
representations. Coolant state participates in commanded idle, warm-up fueling,
startup enrichment and other temperature gates; MAT enters temperature-dependent
control. Actual physical engine temperature remains outside the firmware model.

**Complete signal path.** Thermistor and ground -> bias/filter -> U10 -> sample
and range-dependent conversion -> temperature state -> fuel/idle/control and
diagnostics. CTS adds a return path from firmware range selection through U12
CTSHI to the sensor bias circuit. Sensor conversion is therefore a coupled
hardware/software function, rather than an isolated lookup table.

**Fault behavior.** An open sensor tends toward the bias rail and a short tends
toward ground under the passive-sensor assumptions. A poor ground shifts the
reading; a stuck CTS range output makes the voltage inconsistent with the table
selected by software. Incorrect coolant state can alter idle or fueling without
proving hardware LIMP activation. Fault substitution and qualification must follow
the emitted diagnostic paths, not an assumed universal default temperature.

**Evidence boundaries.** Visible resistor values, table selection and byte
hysteresis are established. Thermistor tolerance, CTSHI drive strength, capacitor
values where unlabeled, voltage-to-count transfer and actual sensor temperatures
remain electrical boundaries. Verify both range voltages and learned firmware
state together when troubleshooting.

### 6.2 TPS and analog MAF input blocks

**Schematic location.** Sheet 2 TPS#/TPS and VMAF#/VMAF; sheet 1 U10 AN5/pin 6
and AN10/pin 12; sheet 4 U11 TPS; sheet 5 MEMCAL routing. FMAF through U12 is a
separate digital/event interface, not the analog VMAF input.

**Electrical operation.** TPS# has a 220-kΩ ground return and feeds conditioned
TPS through 10 kΩ and a capacitor to ground. It is a potentiometer/sensor-voltage
input, not a passive temperature divider. The weak pull-down biases an open
external source toward ground, subject to chip loading. U10 receives conditioned
TPS; U11 pin 20 also receives it, and the measured 376 jumper routes it to
U11's pin 28 labeled MAP. The analog MAF input VMAF# instead has a 1.00-kΩ
pull-up and a 10-kΩ series/filter path to VMAF. Source impedance matters: the
pull-up can load a weak sensor output, so an open VMAF input need not produce
the same fault polarity as an open TPS input.

**Firmware operation.** Startup `$C999` and common acquisition `$CC24` request
TPS selector `$50`. Raw counts go to `$0081`; learned closed-throttle state is
maintained at `$0086/$0087`, normalized throttle at `$0082`, and transient/history
state at `$00DD/$00DE`. Learning means raw voltage and normalized throttle are
not interchangeable. TPS affects throttle-following idle demand, transient fuel,
mode gates and Error 21 qualification.

At `$F7AC-$F7B6`, analog MAF acquisition requests `$A0`, stores raw `$00ED`,
multiplies by seven, and stores the intermediate at `$00EF`. Subsequent table and
filter processing produces airflow `$00EA:$00EB`; `$D769-$D7A0` combines airflow
with reference period to produce load `$0063`. Multiplication by seven is an
internal scaling step, not a direct grams-per-second conversion. Key-off burn-off
diagnostics also sample `$A0` at `$EB05`; Error 33/34 qualification uses the
established MAF diagnostic paths. Do not equate an ordinary airflow sample with
a direct measurement of burn-off current.

**Complete signal path.** TPS voltage -> filter -> raw count -> learned/normalized
throttle and transient state -> idle/fuel/gates. Analog MAF voltage -> filter ->
raw/intermediate -> airflow -> reference-period-dependent load -> fuel calculation.
The supplied image's main load path is VMAF, while MAP/MAP2 A/D channels have no
explicit normal conversion request found in the existing listing audit.

**Fault behavior.** TPS noise can appear as throttle motion; incorrect learned
closed-throttle state can change normalized throttle even with a repeatable raw
sample. MAF wiring/pull-up faults can corrupt computed airflow and fueling.
Those mechanisms support troubleshooting hypotheses, not a claim that every
high idle is TPS-related or every MAF fault invokes U11 fallback. Compare raw
counts, processed state, mode flags and the actual sensor voltage separately.

**Evidence boundaries.** Firmware storage and scaling are established. Exact
sensor curves, filter constants with unlabeled capacitors and internal U11
sensor use remain unknown. Physical MAP and TPS must not be shorted together
when reconstructing the measured MEMCAL selection jumper.

### 6.3 VOLT and PUMPVOLT divider/filter blocks

**Schematic location.** Sheet 2 VIGN-to-VOLT and PUMPVOLT#-to-PUMPVOLT networks;
sheet 1 U10 AN1/pin 2 and AN6/pin 7. These measure separate external sources.

**Electrical operation.** Both use 33.2 kΩ above the node and 8.06 kΩ below it.
With negligible converter loading, V_NODE = V_SOURCE*8.06/(33.2+8.06), or
approximately 0.195347*V_SOURCE. At an illustrative 12 V source this is 2.344 V;
it is not a measured operating voltage or an A/D calibration. The Thevenin
resistance is approximately 6.485 kΩ. VOLT has 1.0 µF to ground, giving nominal
RC time constant 6.485 ms; PUMPVOLT has 4.7 µF, giving 30.481 ms. Component
values imply PUMPVOLT responds more slowly under the ideal divider assumptions.
Capacitor/resistor tolerance, leakage and source impedance affect the real result.

VOLT derives from VIGN on the schematic, even when firmware calls its stored
value battery voltage. PUMPVOLT derives from the separate vehicle PUMPVOLT#
connection; it is not the fuel-pressure measurement or automatically the PUMP#
driver output. Supply/ground drops can make the two channels disagree legitimately.

**Firmware operation.** Selector `$10` samples VOLT during startup and Segment E
and stores `$007E`. Consumers include voltage qualification, injector voltage
compensation, dwell/feedback handling and diagnostics. `$E816-$E81B` requests
`$60` and stores PUMPVOLT at `$007F`; later Error 54 and MAF-diagnostic gates use
that separate state. Factory entry independently compares battery, pump and
DIAG samples, so replacing one voltage channel with the other changes behavior.

**Complete signal path.** VIGN or pump-monitor source -> individual divider/RC ->
U10 -> channel-specific RAM -> compensation or diagnostic/state gates. Divider
voltage, A/D count and the resulting timing compensation are separate quantities;
the firmware's tables establish the last transformation, not the first two.

**Fault behavior.** An open lower resistor or faulty ground can raise the node;
an open upper resistor can make it low under the ideal input assumptions. Leakage
or a failed capacitor can bias or slow the reading. A bad VOLT reading can distort
compensation despite healthy battery terminals. A bad PUMPVOLT reading can affect
pump/MAF diagnostics without proving a failed pump. Check both external source
and divided node before treating the firmware byte as a verified supply voltage.

**Evidence boundaries.** Ideal divider gain and RC products are computed from
printed component values. U10 reference accuracy, leakage, filter tolerances,
actual harness voltages and real transient response are not established here.

## 7. Reference, RPM, dwell, spark, and knock

Conditioned distributor activity reaches U12 `REF`; U12 presents reference-
related connections to U9/U11, and U9 makes occurrence and timing state
processor-visible. Firmware qualifies reference state, derives RPM using the
image's supported V8 geometry, and maintains no-reference and engine-running
state. Four distributor reference pulses per crankshaft revolution are strongly
supported; a reference interval represents 90 crank degrees.

Spark production combines calibrated main and coolant terms, startup behavior,
mode terms, limits, hot-restart effects, and knock retard. At `$D20C-$D267`,
the firmware converts desired angle to timing relative to reference period and
stages U9 state. Important processor-visible locations include `$3FC8` current
period state, `$3FDC` dwell period, `$3FE4/$3FE6` next-dwell state, `$3FE8`
fire/fall delta, `$3FEC` last-reference counter, `$3FF6` reference-to-fire
offset, and `$3FFC` control/status.

Knock is a separate event path. CAL32/external `KNOCK#` reaches U12 and U9;
firmware consumes changes in `$3FCA` at `$D0D1-$D157`, accumulates retard in
`$00A5`, applies it to spark, and recovers it in Segment A at `$EB3A-$EB59`.
Error 43 uses the associated activity checks. CAL29/U10 AN9 remains a distinct
analog ESC observation and is not inserted into this event path.

U9/U12 physical edge generation, timer scaling, EST and bypass polarity,
ignition-coil current, and exact feedback transformation remain F4. Error 42
establishes firmware feedback-consistency behavior, not the undocumented
electrical implementation.

## 8. Airflow, fuel, and injection

Normal airflow begins with U10 selector `$A0`. `$F7AC-$F7B6` stores VMAF raw
state at `$00ED`; subsequent processing produces airflow `$00EA:$00EB`.
`$D769-$D7A0` combines airflow with reference period to produce load `$0063`,
while shifting prior values through `$0061/$0062`.

The fuel path then selects cranking or normal operation and applies the
listing-defined commanded air/fuel relationship, startup and power enrichment,
oxygen feedback, BLM correction, acceleration enrichment, deceleration fuel
cutoff (DFCO), battery-related compensation, pulse-width shaping, and service
gates. The sixteen-cell BLM uses a four-by-four airflow/RPM selection for this
image (`LC017=0`) and remains distinct from the faster closed-loop correction.

The established command chain is:

`VMAF -> airflow -> load/fuel terms -> corrected pulse width -> permitted injector service -> U9 $3FD0`

U9 `$3FCE` is independently exercised as EFI delay. U9 `INJS/INJA`, U12
injector drive/current control, U11 `INJ/INJLIMP`, Q1, and the external
injector circuit form the hardware envelope. Firmware calculation and raw
commands are F0/F2; phase, current limiting, waveform, opening delay, fuel
pressure, flow, and combustion are F3/F4.

### 8.1 Injector gate drive, current sense and feedback block

**Schematic location.** Sheet 4 U12 injector section, Q1, INJ#, INJGND#,
INJSENSE#, charge/boost network and INJLOOP; sheet 1 U8/U9 command connections;
sheet 5 vehicle injector/ground/sense terminals.

**Electrical operation.** Q1 is drawn as a low-side switching transistor: its
load terminal joins INJ#, and its source-side return joins INJGND#. U12 INJOUT
pin 2 drives its gate through 1.2 kΩ with clamp/filter components. A separate
INJ#-to-gate diode/clamp branch includes another 1.2-kΩ resistor. These support
protected inductive-load switching; clamp voltages and exact avalanche/energy
behavior cannot be calculated from the unlabeled devices.

| U12 connection | Electrical role visible on sheet 4 |
| --- | --- |
| INJS pin 47 | Command from U9 pin 8 |
| INJA pin 46 | Command from U9 pin 9 and U8 port connection |
| INJOUT pin 2 | Q1 gate drive |
| ISENSE+ pin 5 / ISENSE- pin 6 | Differential connection across 0.103-ohm current-sense resistor |
| ESENSE pin 3 | INJ# voltage feedback through 68.1-kΩ/20.0-kΩ divider |
| DBL pin 30 / VDBL pin 4 | VIGN-fed diode/capacitor boost network |
| INJLOOP pin 52 (sheet 1) | Conditioned injector-output feedback |
| INJLIMP pin 48 | Named terminal; no external connection drawn on sheet 4 |

The 0.103-ohm, 2-W resistor produces 0.103 V per ampere and dissipates
0.103*I² watts if all measured current passes through it. The 2-W marking does
not establish the regulated peak/hold current or allowable pulse duty. The
actual return routing must also be checked at the vehicle harness.

With negligible ESENSE loading, the 68.1-kΩ/20.0-kΩ divider gives
V_ESENSE = V_INJ * 20/(68.1+20), approximately 0.227*V_INJ. It observes the
switched injector node, not fuel pressure or fuel flow. The lower INJLOOP branch
uses 27 kΩ, a diode to VIGN and 180 kΩ into U12; its logic thresholds and input
bias are undocumented, so a simple unloaded divider equation is insufficient.

The two 1.0-µF capacitors, steering diodes and 1.8-kΩ VIGN feeds are consistent
with boosted gate-drive supply circuitry. The drawing's DBL/VDBL names do not
prove an exact doubled voltage, switching frequency or regulation law. Current
sense plus a boosted gate supply supports an active injector-driver interpretation,
but a specific peak-and-hold waveform remains unestablished.

**Firmware operation.** Common injector bookkeeping at `$F67B-$F768` is gated
by bit 6 in status sampled from `$3FFA`, separately from distributor-reference
bit 3. The synchronous fuel path `$F9D2-$F9E4` limits and writes pulse width
to `$3FD0`. Asynchronous enrichment includes the `$3FF2` write at `$E4D3` and
control operations through `$F4C3/$F4CE`. These are processor-visible commands;
they are not a software reconstruction of U12's current-control loop. Factory
writes provide an independent cross-check on the injector-timing register roles.

**Complete signal path.** Sensor/reference acquisition -> firmware fuel calculation
and permitted timing command -> U9 INJS/INJA interfaces -> U12 gate/current
control -> Q1 -> injector load and ground return. Voltage/current feedback returns
to U12, while conditioned INJLOOP reaches its discrete-input interface. Electrical
pulse shape, injector opening delay and fuel delivery remain downstream of the
firmware timing command.

**Fault behavior.** A good `$3FD0` command does not establish a working output.
A failed Q1, gate path, return connection or sense circuit can prevent or distort
actuation while the processor still computes fuel. Feedback faults can make the
chip see a different electrical state from the commanded state. Injector-open,
short-load and clamp-failure responses require custom-chip or bench evidence;
no automatic diagnosis or protection threshold is asserted here.

**Evidence boundaries and backup path.** U11 emits INJLIMP, which sheet 5 routes
to J3; U12 pin 48 is labeled INJLIMP but has no drawn connection on sheet 4.
Do not silently connect these names in a replacement schematic or claim the
fallback handoff is electrically closed by this drawing. A shared net name on a
wired terminal would be evidence; an unconnected pin label is not sufficient.
The external evidence supports MEMCAL-configured auxiliary/backup injection,
but exact selection, missing interconnection and internal U12 routing remain
unresolved. Verify original-board continuity before choosing a reconstruction.

## 9. Idle-air control

Firmware builds a coolant- and mode-dependent target idle, applies minimum-
position and anticipation state, and runs the established feedback regulator
every 50 ms. The regulator preserves source-specific proportional, derivative,
deadband, residual, and fractional-step behavior. It produces a packed
direction/magnitude request; a separate 6.25-ms motor service consumes at most
one step and updates phase and position bookkeeping.

Throttle follower and A/C, fan, and calibrated auxiliary load anticipation add
feed-forward demand without replacing feedback regulation. Startup establishes
initial position state. During key-off, `$D370-$D3DB` requests closure to the
hard stop, initializes the software counter, then reopens toward calibrated
park `LC62F=144` through repeated ordinary motor services.

The `$002C` HAL observation is software position bookkeeping, not measured
pintle position. Winding current, direction at the connector, missed steps,
hard-stop mechanics, airflow per step, and engine response remain F3/F4.

## 10. Emissions, accessories, and transmission

The major loop qualifies secondary-air (AIR), exhaust-gas-recirculation (EGR),
canister-purge/CCP, cooling-fan, and air-conditioning requests using calibrated
thresholds, delays, and hysteresis. A/C and fan state also feed the IAC
anticipation path. Subsystem request state remains distinct from common output
staging and from the physical valve, relay, fan, or compressor state.

Transmission logic combines selector/transmission state, vehicle speed, and
Segment-E torque-converter-clutch (TCC) qualification. Three stages must remain
distinct: firmware TCC request, raw ECM output, and actual powered/hydraulic
clutch state. The external brake series-power path is not silently rewritten as
an ECM software input. Gear ratios, converter slip, and vehicle dynamics in the
optional PC drive model are F3 assumptions; driver, solenoid, and hydraulic
behavior not directly established remain F4.

## 11. Output staging and electrical interfaces

Major Segment 1 `$EDA3-$EF03` is the common raw output-staging block. It
preserves battery qualification, ordinary and field-diagnostic engine-off
patterns, normal subsystem selection, Mode-4 pulse-width-modulation overrides,
and direct writes to `$3FCC`, `$3FD2`, `$3FD4`, `$3FD6`, `$3FD8`, and `$4004`.
The output regression freezes the integrated raw-stage signature `FAADF8A6`.

A register value is not automatically a voltage, current, relay state, valve
position, or actuator force. The supported output sequence is:

`firmware request (F0) -> raw processor-visible write (F0) -> proven connection (F1) -> HAL observation (F2) -> optional plant interpretation (F3)`

Undocumented driver or custom-device transformation stays F4. This applies to
injectors and ignition as well as IAC, TCC, AIR, EGR, purge, fan, A/C, service
lamp, and factory-test outputs. Raw values such as `0`, `1`, `$D000`, or
`$DFFF` do not establish active-high/low, sourcing/sinking, or energized-load
semantics by themselves.

## 12. Diagnostics and communications

Diagnostics separate current qualification, stored history, service-lamp
request, and physical lamp behavior. Fault-specific counters and state qualify
conditions before history is latched or cleared. Field-service mode sequences
flash codes through firmware state; electrical lamp polarity remains at the
output boundary.

The normal 160-baud asynchronous link is a pulse-width-coded display stream.
The completed raw HAL retains its diagnostic and factory managers, processor-
visible timing/state, and output observations without claiming external wire
voltage or scan-tool presentation.

The 8192-baud path uses the sole device `$80` serial core initialized at
`$C9F4` and implemented through `$FA58-$FC71`. Firmware validates receive
state/checksum, constructs Modes 0-4 responses, and advances transmit state.
Mode 4 becomes active only through the scheduler-facing lifecycle at
`$CB67-$CB72`; entry-only commands and exit cleanup retain their source order.

External ALDL transceiver voltage, polarity, collision behavior, precise
physical bit timing, unavailable memory reads, and scan-tool formatting remain
F3/F4. Protocol state is not evidence of those electrical properties.

### 12.1 DIAG mode selection and dashboard data block

**Schematic location.** Sheet 2 DIAG# input, sheet 1 U10 AN7/pin 8, sheet 3
U2 ALDL transceiver, and sheet 5 DIAG#/ALDL# connector paths. DIAG# and ALDL#
are separate electrical nets: one requests mode; the other carries serial data.

**Electrical operation.** DIAG# has a 10-kΩ pull-up to the logic supply and a
10-kΩ series path to DIAG/U10 AN7, with a capacitor at the converter node.
Ignoring converter leakage, an external 10-kΩ resistor to ground makes the
connector voltage approximately half the pull-up rail. The input series resistor
and capacitor condition the signal; they do not halve its unloaded steady-state
voltage again. An open solder joint at U10 pin 8 can disconnect the internal
converter input from the correctly biased external trace. Its resulting reading
is not predictable without leakage/internal-device information.

**Firmware operation.** The common A/D routine is `$F1BE-$F1DF`. Segment 3 at
`$EA28-$EA47` requests selector `$70`, clears `$0035` bits 4/5, and compares the
sample with literal counts 40, 100 and 152. A count below 40 sets bit 4. Counts
100 through 151 set bit 5 unless `$0046` bit 3 is already set. Other counts leave
these two bits clear. These are verified count thresholds, not measured voltages.
Startup also samples DIAG and has additional factory/exceptional paths.

At `$F8B6-$F8E8`, bits 4/5 select the serial table: ordinary operation uses
`$C6FE`, whereas diagnostic selection uses `$C70D`. The ordinary table points
to `LC009`, `$011A`, `$011E`, and `LC70C`; it carries configuration/scaling and
fuel/distance-related data rather than the ordinary scan-tool list. Emitted
additions into `$011A` at `$E4EE-$E4F3` and `$F733-$F739` independently support
its fuel accumulation role. Table comments alone do not define engineering units.
The ordinary record is commonly observed as five bytes including its mode byte;
the diagnostic record is the longer 25-byte stream.

**Complete signal path.** DIAG voltage -> U10 sample -> mode flags -> serial
payload selection and control-mode consumers -> U2/ALDL line -> dashboard or
scanner. The dashboard needs the normal economy-related record. Replacing it
with the diagnostic record can remove or corrupt the expected economy updates.
Range can consequently be affected through the dashboard's economy estimate;
the dashboard's precise parsing and range algorithm are not in this ECM image.

**Fault behavior.** A false ALDL request can cause high idle alongside an invalid
MPG display. `$D448-$D450` tests bit 5 and substitutes `$50` (80) into the
RPM/25 target path; this is 2000 RPM in that particular command path, not proof
that a faulty engine must physically idle at that speed. Other gates, IAC state,
and engine airflow determine the resulting speed. Do not describe the observed
1000-RPM symptom as an unconditional calibrated 1000-RPM ALDL target.

**Reported repair example.** In the owner's June 2023
[CorvetteForum repair thread](https://www.corvetteforum.com/forums/c4-tech-performance/4746412-1986-corvette-ecm-problem-always-in-10k-mode.html),
posts under `dldavis` describe roughly 1000-RPM idle, MPG stuck at 1.2, and an
unrequested change from the short record to the diagnostic record. The June 24
post reports resoldering A/D pin 8 (DIAG); the June 28 follow-up reports three
days of correct operation. The owner confirmed the recollection in this session.
This is firsthand repair history, not a reproduced electrical test. A disconnected
pin causing erroneous mode samples is an explanation consistent with the repair;
its actual disconnected voltage was not measured here. This episode is not
proof of hardware limp-mode activation or an incorrect NetRes.

**Evidence boundaries.** Instructions establish mode selection and payload
switching. Exact line voltages, dashboard error handling, and the failed joint's
internal electrical behavior remain hardware or external-document boundaries.

## 13. Key-off, shutdown, and exceptional modes

The live lifecycle composes reset-vector power-on, ordinary IRQ operation, the
`$D6D1-$D769` ignition/key-off transition, retained-state work, IAC homing, and
the terminal `$D6EA` software-interrupt (SWI) powerdown request. BLM commit via
LF447 occurs in its documented order. If ignition returns while the software
still executes, restart follows the established path rather than a host-created
shortcut.

The vectors at `$FFF0-$FFFE` target `$6000`, `$C9F4`, `$F27B`, `$6000`, and
four copies of `$C800`; `$F27B` is an immediate return-from-interrupt. What the
processor, power supply, keep-alive circuitry, or external `$6000` target does
after SWI/reset is outside the supplied image. The HAL reports the software
request; it does not simulate physical loss of power as an F0 fact.

## 14. C port and embedded boundary

The implementation is intentionally a single strict-C89 translation unit:
`main.c` includes the fragments under `src/`, `simulation/`, and `tests/`.
Exact-width assumptions are enforced with compile-time checks, and explicitly
encoded byte/word operations preserve big-endian processor-visible behavior.
The code fragments are not intended to compile independently.

`src/hal_interface.inc.h` is the explicit raw seam. It supplies A/D, reference,
discrete, communications, power/startup, and factory stimuli and observes U9,
parallel-I/O, injector, IAC, and common output state. Target-specific timers,
vectors, A/D and capture peripherals, serial transceivers, output drivers,
retained memory, watchdog, and physical power behavior belong behind that seam.

An embedded port must retain F0 order and widths while replacing F2 mechanics.
It must not migrate the PC's F3 engine/vehicle plant into firmware or fill F4
U9/U11/U12 behavior for convenience. The controlled sequence and exit criteria
are in the [embedded and HIL roadmap](docs/EMBEDDED_HIL_IMPLEMENTATION_ROADMAP.md).

## 15. Simulation and verification

Verification has three complementary layers:

- focused regressions preserve individual arithmetic, state machines, tables,
  and raw interfaces;
- scheduler and lifecycle regressions prove source-order composition;
- deterministic PC scenarios exercise closed-loop interaction with explicitly
  assumed F3 engine and transmission plants.

`make test` compiles only `main.c` using `gcc -std=c89 -Wall -Wextra -pedantic`,
runs the complete regression program, checks every Makefile-gated result and
signature, and rejects genuine `FAIL` output. Frozen signatures include the
normal-operation signature `4BA6B7C6`, transmission-aware signature `9732D09B`,
Segment-D diagnostic signature `F357A5F2`, Segment-1 output signature
`FAADF8A6`, and ignition-lifecycle signature `16D17C9C`. Later raw-HAL and
lifecycle signatures are documented by the verification chapter and audits.

Signatures are broad deterministic change detectors. They do not replace
explicit assertions, prove untested paths, or establish physical truth. A
change that preserves a signature can still be wrong; a deliberate behavior
change requires source evidence, focused checks, integration checks, and a
reviewed baseline update.

## 16. Explicit unresolved boundaries

The following are intentionally unresolved or external to the supplied image:

- undocumented internals, transfer functions, phase, polarity, thresholds,
  timer scaling, and current-control behavior of U9, U11, and U12;
- unclassified U9 `$3FC0-$3FFF` locations and unproven pin/register mappings;
- U10 selector `$B0`, analog reference/accuracy details, and variant-specific
  normal use of MAP2, MAP, or ESC channels;
- exact physical relationships among `OSC`, `CYL`, distributor reference,
  ignition, EST/bypass feedback, and knock events beyond established paths;
- injector and ignition waveforms, driver protection, coil/injector current,
  output polarity, and load electrical behavior;
- IAC mechanics and airflow, relay/solenoid/valve/fan/compressor response, TCC
  hydraulics, sensor transfer functions, and engine/vehicle dynamics;
- optional code at `$5800`, external vector targets at `$6000`, the factory
  fixture/FMD implementation, and unavailable external memory contents;
- physical reset, rail timing, standby/keep-alive power, watchdog, and
  processor consequences after SWI or power transition;
- variant applicability of external photographs, MEMCAL networks,
  calibrations, and historical artifacts not directly tied to this unit.

These are engineering work items, not gaps to be closed by plausible guesses.
New evidence should update the evidence register and cross-reference first,
then the affected detailed chapter and this synthesis.

## Appendix A. Consolidated address and register index

This index collects the principal processor-visible locations used in the
system narrative. Functional names describe established executable use; they
do not imply undocumented electrical or custom-device behavior.

### A.1 RAM and software state

| Address/range | Established role | Detailed reference |
| --- | --- | --- |
| `$002C` | IAC software position bookkeeping | [Idle-air control](docs/reference/IDLE_AIR_CONTROL_THEORY.md) |
| `$005B-$005F` | coolant conversion and diagnostic state | [Sensor acquisition](docs/reference/SENSOR_ACQUISITION_FILTERING_THEORY.md) |
| `$0061-$0063` | prior and current load state | [Airflow/fuel/injection](docs/reference/AIRFLOW_FUEL_INJECTOR_THEORY.md) |
| `$0064` | common temporary A/D result | [A/D acquisition](docs/reference/ADC_SENSOR_ACQUISITION.md) |
| `$006F/$0071/$0073` | oxygen acquisition/filter state | [Sensor acquisition](docs/reference/SENSOR_ACQUISITION_FILTERING_THEORY.md) |
| `$007E` | persistent `VOLT` sample | [Sensor acquisition](docs/reference/SENSOR_ACQUISITION_FILTERING_THEORY.md) |
| `$007F` | persistent `PUMPVOLT` sample | [Sensor acquisition](docs/reference/SENSOR_ACQUISITION_FILTERING_THEORY.md) |
| `$0081/$0082` | raw and normalized TPS state | [Sensor acquisition](docs/reference/SENSOR_ACQUISITION_FILTERING_THEORY.md) |
| `$0086/$0087` | learned closed-throttle state | [Sensor acquisition](docs/reference/SENSOR_ACQUISITION_FILTERING_THEORY.md) |
| `$0095:$0096` | reference-period state used by load/fuel processing | [Airflow/fuel/injection](docs/reference/AIRFLOW_FUEL_INJECTOR_THEORY.md) |
| `$00A5` | accumulated knock-retard state | [ESC/knock chain](docs/reference/ESC_KNOCK_SIGNAL_CHAIN.md) |
| `$00DD-$00DE` | transient TPS state | [Sensor acquisition](docs/reference/SENSOR_ACQUISITION_FILTERING_THEORY.md) |
| `$00EA:$00EB` | processed airflow word | [Airflow/fuel/injection](docs/reference/AIRFLOW_FUEL_INJECTOR_THEORY.md) |
| `$00ED/$00EF` | raw and intermediate VMAF state | [Airflow/fuel/injection](docs/reference/AIRFLOW_FUEL_INJECTOR_THEORY.md) |
| `$012B/$0060` | complemented raw and processed MAT state | [Sensor acquisition](docs/reference/SENSOR_ACQUISITION_FILTERING_THEORY.md) |
| `$0152/$0153/$0156/$0157` | Mode-4 raw PWM override state | [Output staging](docs/reference/OUTPUT_STAGING_ELECTRICAL_INTERFACES.md) |
| `$017B-$0186` | factory-test sequential A/D snapshot | [A/D acquisition](docs/reference/ADC_SENSOR_ACQUISITION.md) |

### A.2 Processor-visible peripheral locations

| Address/range | Established role | Detailed reference |
| --- | --- | --- |
| `$3FC8` | current spark/reference-period state | [U9 register map](docs/reference/U9_REGISTER_WINDOW_MAP.md) |
| `$3FCA` | knock-event quantity consumed by firmware | [ESC/knock chain](docs/reference/ESC_KNOCK_SIGNAL_CHAIN.md) |
| `$3FCC` | Segment-1 raw MPU output word | [Output staging](docs/reference/OUTPUT_STAGING_ELECTRICAL_INTERFACES.md) |
| `$3FCE` | EFI delay state | [Ignition/injection chain](docs/reference/IGNITION_INJECTION_SIGNAL_CHAIN.md) |
| `$3FD0` | synchronous injector command | [Airflow/fuel/injection](docs/reference/AIRFLOW_FUEL_INJECTOR_THEORY.md) |
| `$3FD2/$3FD4/$3FD6/$3FD8` | Segment-1 raw MPU output words | [Output staging](docs/reference/OUTPUT_STAGING_ELECTRICAL_INTERFACES.md) |
| `$3FDC` | dwell-period value | [Reference/RPM/dwell/spark](docs/reference/REFERENCE_RPM_DWELL_SPARK_THEORY.md) |
| `$3FE4/$3FE6` | next-dwell timing and update state | [Reference/RPM/dwell/spark](docs/reference/REFERENCE_RPM_DWELL_SPARK_THEORY.md) |
| `$3FE8` | current fire/fall delta | [Reference/RPM/dwell/spark](docs/reference/REFERENCE_RPM_DWELL_SPARK_THEORY.md) |
| `$3FEC` | counter value at last reference | [Reference/RPM/dwell/spark](docs/reference/REFERENCE_RPM_DWELL_SPARK_THEORY.md) |
| `$3FF6` | reference-to-fire offset | [Reference/RPM/dwell/spark](docs/reference/REFERENCE_RPM_DWELL_SPARK_THEORY.md) |
| `$3FFC` | control/status word used by EST/bypass and other modes | [U9 register map](docs/reference/U9_REGISTER_WINDOW_MAP.md) |
| `$4004` | parallel-I/O byte modified by Segment 1 | [Output staging](docs/reference/OUTPUT_STAGING_ELECTRICAL_INTERFACES.md) |

### A.3 Executable regions and external boundaries

| Address/range | Established role | Detailed reference |
| --- | --- | --- |
| `$5800` | optional-ROM boundary; code unavailable | [Startup/shutdown](docs/reference/STARTUP_SHUTDOWN_EXCEPTIONAL_MODES.md) |
| `$6000` | external vector destination; behavior unavailable | [Startup/shutdown](docs/reference/STARTUP_SHUTDOWN_EXCEPTIONAL_MODES.md) |
| `$8000-$FFFF` | supplied program window | [Processor/memory architecture](docs/reference/PROCESSOR_MEMORY_CUSTOM_PERIPHERAL_ARCHITECTURE.md) |
| `$C800-$C9F3` | reset and power-on sequence | [Startup/shutdown](docs/reference/STARTUP_SHUTDOWN_EXCEPTIONAL_MODES.md) |
| `$C9F4`, `$FA58-$FC71` | device `$80` SCI core | [Diagnostics/ALDL](docs/reference/DIAGNOSTICS_ALDL_COMMUNICATION_THEORY.md) |
| `$CB67-$CB72` | scheduler-facing Mode-4 activation | [Diagnostics/ALDL](docs/reference/DIAGNOSTICS_ALDL_COMMUNICATION_THEORY.md) |
| `$D0D1-$D157` | knock-event consumption and retard | [ESC/knock chain](docs/reference/ESC_KNOCK_SIGNAL_CHAIN.md) |
| `$D20C-$D267` | spark angle-to-time staging | [Reference/RPM/dwell/spark](docs/reference/REFERENCE_RPM_DWELL_SPARK_THEORY.md) |
| `$D370-$D3DB` | IAC reset/homing state machine | [Idle-air control](docs/reference/IDLE_AIR_CONTROL_THEORY.md) |
| `$D6D1-$D769` | key-off transition and adjacent lifecycle path | [Startup/shutdown](docs/reference/STARTUP_SHUTDOWN_EXCEPTIONAL_MODES.md) |
| `$D769-$D7A0` | airflow/reference-period load producer | [Airflow/fuel/injection](docs/reference/AIRFLOW_FUEL_INJECTOR_THEORY.md) |
| `$D6EA` | terminal SWI software-powerdown request | [Startup/shutdown](docs/reference/STARTUP_SHUTDOWN_EXCEPTIONAL_MODES.md) |
| `$EDA3-$EF03` | Segment-1 raw output staging | [Output staging](docs/reference/OUTPUT_STAGING_ELECTRICAL_INTERFACES.md) |
| `$F1BE-$F1DF` | common U10 A/D transaction | [A/D acquisition](docs/reference/ADC_SENSOR_ACQUISITION.md) |
| `$F3A7-$F3B4` | retained checksum computation | [Startup/shutdown](docs/reference/STARTUP_SHUTDOWN_EXCEPTIONAL_MODES.md) |
| `$F434-$F446` | retained-state recovery initialization | [Startup/shutdown](docs/reference/STARTUP_SHUTDOWN_EXCEPTIONAL_MODES.md) |
| `$FFF0-$FFFE` | emitted vector table | [Startup/shutdown](docs/reference/STARTUP_SHUTDOWN_EXCEPTIONAL_MODES.md) |

The complete function-to-listing, RAM/register, calibration, schematic, C, and
test mapping remains in the
[hardware/firmware cross-reference](docs/reference/HARDWARE_FIRMWARE_CROSS_REFERENCE.md).

## Appendix B. Detailed chapter map

| Chapter | Controlling reference |
| ---: | --- |
| 1 | [System overview and evidence method](docs/reference/README.md) |
| 2 | [Power, reset, retained power, startup, and shutdown](docs/reference/STARTUP_SHUTDOWN_EXCEPTIONAL_MODES.md) |
| 3 | [Processor, memory, and custom peripherals](docs/reference/PROCESSOR_MEMORY_CUSTOM_PERIPHERAL_ARCHITECTURE.md) |
| 4 | [MEMCAL architecture and reconstruction](docs/reference/MEMCAL_ARCHITECTURE.md) |
| 5 | [Interrupt and scheduler architecture](docs/reference/INTERRUPT_SCHEDULER_ARCHITECTURE.md) |
| 6 | [Reference, RPM, dwell, and spark](docs/reference/REFERENCE_RPM_DWELL_SPARK_THEORY.md) |
| 7 | [Sensor acquisition and filtering](docs/reference/SENSOR_ACQUISITION_FILTERING_THEORY.md) |
| 8 | [Airflow, fuel, and injection](docs/reference/AIRFLOW_FUEL_INJECTOR_THEORY.md) |
| 9 | [Idle-air control](docs/reference/IDLE_AIR_CONTROL_THEORY.md) |
| 10 | [Emissions and accessory control](docs/reference/EMISSIONS_ACCESSORY_CONTROL_THEORY.md) |
| 11 | [Transmission and TCC](docs/reference/TRANSMISSION_TCC_THEORY.md) |
| 12 | [Diagnostics and ALDL](docs/reference/DIAGNOSTICS_ALDL_COMMUNICATION_THEORY.md) |
| 13 | [Output staging and electrical interfaces](docs/reference/OUTPUT_STAGING_ELECTRICAL_INTERFACES.md) |
| 14 | [Startup, shutdown, and exceptional modes](docs/reference/STARTUP_SHUTDOWN_EXCEPTIONAL_MODES.md) |
| 15 | [C port and embedded migration](docs/reference/C_PORT_ARCHITECTURE_EMBEDDED_MIGRATION.md) |
| 16 | [Simulation and verification](docs/reference/SIMULATION_VERIFICATION_THEORY.md) |

The [theory-of-operation index](docs/reference/THEORY_OF_OPERATION_INDEX.md)
retains the acceptance criteria and supporting signal-chain references for the
sixteen-chapter source set.

## Appendix C. Glossary

| Term | Meaning |
| --- | --- |
| A/C | Air conditioning. |
| A/D | Analog-to-digital conversion. |
| AE | Acceleration enrichment. |
| AFR | Air/fuel ratio. |
| AIR | Secondary-air management system. |
| ALDL | Assembly Line Diagnostic Link. |
| BLM | Block-learn memory; retained adaptive fuel-correction state. |
| CCP | Canister-control purge; used here for the canister-purge function. |
| CTS | Coolant temperature sensor. |
| DFCO | Deceleration fuel cutoff. |
| ECM | Engine control module. |
| EGR | Exhaust-gas recirculation. |
| ESC | Electronic spark control; the analog observation path is distinct from the knock-event path. |
| EST | Electronic spark timing. |
| FMD | Factory-mode data/exchange boundary used by the factory-test path. |
| HAL | Hardware abstraction layer; the raw processor-visible F2 seam. |
| HIL | Hardware-in-the-loop verification. |
| IAC | Idle-air control. |
| IRQ | Interrupt request; ordinary periodic firmware service in this document. |
| MAF | Mass airflow. |
| MAP | Manifold absolute pressure. |
| MAT | Manifold air temperature. |
| MEMCAL | Removable memory/calibration and configuration carrier. |
| MPU | The custom processor-visible peripheral/register resource named by the source material. |
| PROM | Programmable read-only memory containing program and calibration data. |
| PWM | Pulse-width modulation. |
| RPM | Revolutions per minute. |
| SCI | Serial communications interface used by the 8192-baud path. |
| SWI | Software interrupt; used by the terminal powerdown request path. |
| TCC | Torque-converter clutch. |
| TPS | Throttle position sensor. |
| VIGN | Ignition-switched supply/sense domain. |
| VMAF | Analog mass-airflow voltage net presented to U10. |
| VSS | Vehicle-speed signal. |

## Appendix D. Schematic functional-block coverage

The theory retains subsystem chapters, with an electrical-to-firmware block
structure: schematic location, electrical operation, firmware operation, complete
signal path, fault behavior, and evidence boundaries. Sections 3.1, 4.1, 4.2, 6.1-6.3, 8.1 and 12.1 are
the expanded treatments. The index below covers all six schematic sheets
and identifies where further component-level expansion is still needed. A row
is a coverage entry, not a declaration that the custom chip is fully explained.

| Sheet / block | Electrical boundary | Firmware relationship / chapter | Coverage state |
| --- | --- | --- | --- |
| 1: U8 processor, crystal, memory bus | Clock, reset, address/data and J4 EPROM interface | Startup and execution; 2, 4, 5 | Existing overview; expand physical timing |
| 1: U9 timing/output peripheral | Reference, MAF, VSS, timing and output pins | Register window and event scheduling; 5, 7, 8, 11 | Firmware established; internal circuitry bounded |
| 1: U10 A/D and SPI | Analog channels, reference rails, MOSI/MISO/SCK/select | Sensor acquisition; 6 | Channel map established; electrical expansion pending |
| 1: U12 SPI/discrete I/O | Digital inputs, CTS range output, reset and LIMP | Input/output state and exceptional modes; 4, 6, 11, 13 | 4.2 LIMP source traced; internal logic bounded |
| 2: discrete input conditioning | Pull-ups/downs, series resistors and capacitors | Switch/accessory states; 10 | Existing subsystem coverage; expand each input |
| 2: VOLT and PUMPVOLT | 33.2-kΩ/8.06-kΩ dividers and filtering | Voltage state/diagnostics; 6, 12 | 6.3 divider/RC and consumers expanded |
| 2: MAP/MAP2 and TPS | Series filtering and bias paths | TPS learning; MAP factory channels; 6 | 6.2 TPS learning and routing expanded |
| 2: CTS and MAT | Bias, filtering and CTSHI range circuit | Temperature conversion; 6, 9 | 6.1 bias and range hysteresis expanded |
| 2: O2 and U4 | Differential sensor amplifier and reference supply | O2 filtering/closed-loop fuel; 6, 8 | Existing firmware; amplifier transfer bounded |
| 2: ESC and KNOCK | Separate analog ESC and U12 digital KNOCK paths | Factory ESC versus U9 knock counter; 6, 7 | Separation established; filter internals bounded |
| 2: VSS and FMAF | U12 signal conditioning with input bias/filtering | Speed/events and MAF interfaces; 6, 8, 10 | Existing firmware; conversion details bounded |
| 2/1: DIAG | Pull-up, series resistor, AN7 | Mode decoding and data-table selection; 12.1 | Expanded with firsthand repair example |
| 3: U13 pump driver | PUMP command to PUMP# | Pump request; 10, 11 | Existing output boundary; driver expansion pending |
| 3: U7 IAC driver | IACA/IACB/IACEN to four winding connections | Step sequencing and regulator; 9, 11 | Existing firmware; winding/load behavior bounded |
| 3: U3/U6 quad drivers | Output protection, fault feedback and limp gating | Accessory outputs and SES; 10, 11 | Existing staging; expand gate/polarity mapping |
| 3: U5 fan/limp gates and RC branch | LIMP-conditioned fan/output logic and delay network | Output and exceptional-mode boundaries; 11, 13 | 4.2 RC scales reviewed; destination incomplete |
| 3: U2 ALDL transceiver | TX/RX/enable and shared ALDL# line | 160-baud and 8192-baud protocols; 12 | 12.1 payload path; physical transceiver bounded |
| 4: distributor/reference and EST | Differential conditioning, REF, EST and BYPASS feedback | Reference, RPM, spark and Error 42; 7 | Existing firmware; comparator/feedback expansion pending |
| 4: U11/MEMCAL configuration | Fixed bias, OSC, sensors and INJLIMP | Auxiliary injection/custom-chip boundary; 3.1, 8 | Expanded; analog equations unknown |
| 4: U12/Q1 injector output | Gate drive, protection, sense resistor, INJLOOP | Injector timing and feedback; 8, 11 | 8.1 expanded; loaded driver behavior bounded |
| 5: J1/J2 vehicle and J3/J4 interfaces | Physical routing, supplies, programming and MEMCAL | Cross-reference for all chapters | Routing index; orientation check still physical |
| 6: U1 power/reset and retained rails | VBATT/VIGN, VCC, standby, reset and LIMP connections | Reset, retention, key-off; 4, 13 | 4.1/4.2 expanded; internal sequencing bounded |

Detailed chapter destinations and register anchors remain in Appendix B and the
[hardware/firmware cross-reference](docs/reference/HARDWARE_FIRMWARE_CROSS_REFERENCE.md).
Next expansions should cover the remaining analog conditioning and output-driver
blocks using this same structure. U1/U12 internal LIMP activation remains an
evidence gap rather than a completed electrical explanation.
