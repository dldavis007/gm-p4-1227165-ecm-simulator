# Sensor acquisition and filtering

## Purpose and evidence boundary

This chapter integrates the established sensor-acquisition paths for the supplied 9340 image: the U10 analog-to-digital (A/D) interface, raw and processed sensor state, firmware filtering/normalization, the image-specific MAF/load path, and non-A/D event/discrete inputs where the existing cross-reference supports them.

The governing distinction is:

- **F0** — listing-backed selector, storage, arithmetic, table, branch, and scheduling behavior;
- **F1** — schematic-proven sensor/net/channel connection;
- **F2** — raw processor-visible stimulus supplied by the simulator HAL;
- **F3** — volts, temperature, pressure, airflow, vehicle dynamics, pulse calibration, sensor transfer, polarity, and other plant assumptions;
- **F4** — undocumented U10/custom-device/internal electrical behavior.

A raw A/D count or pulse state is therefore stronger evidence than an engineering-unit interpretation unless the firmware itself performs the conversion.

## U10 analog acquisition boundary

U10 (`16034988`) is the serial A/D device selected by `ADCCS` and connected through the processor-side SPI nets. The retained schematic identifies AN0 through AN10. Firmware uses selector values in increments of `$10`.

| Selector | U10 channel | Schematic net | Supplied-image normal use | Factory scan |
| ---: | --- | --- | --- | --- |
| `$00` | AN0 | `MAP2` | no explicit normal request found | `$017B` |
| `$10` | AN1 | `VOLT` | voltage/battery processing | `$017C` |
| `$20` | AN2 | `O2` | oxygen acquisition/filtering | `$017D` |
| `$30` | AN3 | `MAP` | no explicit normal request found | `$017E` |
| `$40` | AN4 | `CTS` | coolant conversion | `$017F` |
| `$50` | AN5 | `TPS` | throttle acquisition/normalization | `$0180` |
| `$60` | AN6 | `PUMPVOLT` | pump-voltage processing | `$0181` |
| `$70` | AN7 | `DIAG` | diagnostic/start-run mode selection | `$0182` |
| `$80` | AN8 | `MAT` | manifold-air-temperature processing | `$0183` |
| `$90` | AN9 | `ESC` | no explicit normal request found | `$0184` |
| `$A0` | AN10 | `VMAF` | analog MAF acquisition/burn-off test | `$0185` |
| `$B0` | unresolved | unresolved | no explicit normal request found | `$0186` |

The `$B0` selector remains unnamed because the retained schematic establishes only AN0-AN10. Absence of a normal request for `$00`, `$30`, or `$90` is specific to the supplied image and must not be generalized to every P4 calibration.

## A/D transaction

Routine `$F1BE-$F1DF` is the common conversion boundary. Firmware masks interrupts around the transaction, selects U10, sends the selector, stores the returned 8-bit sample temporarily at `$0064`, clocks the remaining byte, and deselects the device.

This proves the processor-visible 8-bit sample interface. It does not establish U10 sampling aperture, conversion accuracy, reference tolerance, or a universal volts-per-count relationship. `$0064` is a common temporary result, not a dedicated sensor variable.

Factory code `$FDB5-$FDC5` scans `$00` through `$B0` and stores 12 raw samples at `$017B-$0186`. Those bytes are a factory-test snapshot, not the normal-operation sensor layout.

## Voltage channels

### AN1 `VOLT`

The schematic derives `VOLT` from VIGN-related conditioning and routes it to U10 AN1. Firmware requests `$10`, persists the sample at `$007E`, and uses the resulting state for voltage qualifications, injector compensation, dwell/feedback handling, diagnostics, and output gating.

The firmware's convenient battery-voltage terminology does not change the direct schematic fact that the A/D net is `VOLT` from the VIGN conditioning path.

### AN6 `PUMPVOLT`

AN6 is electrically distinct. `$E816-$E81B` requests `$60` and stores the raw result at `$007F`; later logic uses it in Error-54 and MAF-diagnostic qualification. AN1 and AN6 must therefore remain separate voltage observations.

## Oxygen sensor acquisition and filtering

The oxygen-sensor conditioning network produces `O2` at U10 AN2. Firmware requests `$20` at `$D9AD`. The acquired value enters state around `$006F/$0071/$0073` and the established slow O2 filtering path, which is scheduled on the even minor path.

The filtered result is then available to closed-loop fuel and O2 diagnostic behavior. The firmware filter arithmetic is F0. The sensor chemistry, exhaust transport delay, op-amp tolerances, and exact sensor-voltage-to-count curve remain F3/F4 electrical/plant concerns unless separately measured.

## Coolant temperature sensor

Coolant temperature sensor (`CTS`) reaches AN4. Segment 6 requests `$40` at `$F3B9`, applies the listing's two-range table logic, and updates coolant state around `$005B-$005F` together with diagnostic flags/timers.

Here the executable tables establish the firmware count-to-temperature representation. They do not prove every external thermistor tolerance or analog-bias value. A simulator may therefore use the firmware's count conversion exactly while keeping the physical thermistor/voltage model outside F0.

## Throttle position sensor

Throttle position sensor (`TPS`) reaches AN5. Startup and the 6.25-ms common path request `$50`. Firmware stores raw counts at `$0081`, maintains learned closed-throttle state at `$0086/$0087`, produces normalized throttle state at `$0082`, and maintains transient TPS state at `$00DD-$00DE`.

This path demonstrates an important acquisition pattern: raw sensor count, learned reference, normalized operating value, and transient/change state are separate software quantities. A host HAL should inject raw A/D state rather than bypassing these firmware transformations with a pre-normalized throttle percentage.

## Diagnostic analog input

`DIAG` reaches AN7. Startup and Segment 3 request `$70` and compare the returned raw count with literal thresholds to select ordinary, diagnostic, ALDL, or factory-related paths. The result is often consumed immediately rather than assigned one permanent raw variable.

The threshold comparisons are F0. The external connector/loading conditions that produce each voltage are electrical interface behavior and should remain outside the firmware-exact layer unless directly established.

## Manifold air temperature

Manifold air temperature (`MAT`) reaches AN8. Segment A requests `$80` at `$EBB3`, complements the raw byte, stores the inverted sample at `$012B`, updates processed MAT at `$0060`, and maintains Error-23/25 qualification state.

Both stages matter: the physical channel has its raw polarity, while the supplied firmware deliberately complements the byte before its principal stored/processed representation. A simulator that injects an already-complemented value at the U10 boundary would move firmware behavior into the HAL and reduce fidelity.

## MAF acquisition and the principal load path

`VMAF` reaches U10 AN10. Normal airflow code `$F7AC-$F7B6` requests `$A0`, stores the raw sample at `$00ED`, multiplies it by seven, and stores the intermediate at `$00EF`. Subsequent airflow processing produces the airflow word at `$00EA-$00EB`.

The principal load producer at `$D769-$D7A0` then combines airflow `$00EA-$00EB` with reference period `$0095-$0096`, applies calibration `LC69A`, rounds/saturates, and writes current load to `$0063`. Before replacement, older load values shift into `$0061/$0062`.

Thus, for this image:

`VMAF raw count -> MAF/airflow processing -> $00EA:$00EB -> x reference period -> $0063 load`

This is direct executable behavior. It is not a MAP-derived load path.

## MAP and MAP2: physically present, not normal load inputs in this image

The motherboard separately conditions `MAP#` to U10 AN3 and `MAP2#` to U10 AN0. Factory test demonstrates that both can be converted. The supplied normal image, however, has no explicit `$30` or `$00` request before the common A/D routine.

CAL61 is also mechanically/electrically established at U11 pin 28 `MAP`, where it meets the conditioned MAP path. This proves a MAP-related U11 configuration boundary, not a processor-readable MAP value or a hidden producer of `$0063`.

Accordingly Chapter 7 keeps three things separate:

1. U10 AN0/AN3 MAP conversion resources;
2. CAL61/U11 MAP-related custom-device configuration;
3. the executable MAF/reference-period load producer.

## ESC analog observation versus knock events

`ESC` is physically connected to U10 AN9 and factory test stores selector `$90` at `$0184`, but the supplied normal image has no explicit normal `$90` request.

The normal knock-retard event path is separately established through CAL32/external `KNOCK#` -> U12 -> U9, with firmware consuming `$3FCA` deltas. Chapter 7 therefore records AN9 as an analog acquisition resource/factory observation and does not silently turn it into the normal knock-event source.

## Vehicle speed and other non-A/D inputs

Not every sensor-like input is acquired through U10. The hardware/firmware cross-reference identifies vehicle speed (`VSS`) as a conditioned pulse input consumed in Major Segment 2 and related scheduler/transmission behavior. Its firmware state is F0 and the physical connection is F1; pulse calibration and simulated vehicle dynamics remain F2/F3.

Likewise, distributor/reference timing is an event path through U12/U9 rather than U10 and is covered by Chapter 6. Other discrete inputs should be represented at their actual processor-visible boundary rather than forced into the A/D model.

The current evidence set does not justify fabricating a single universal acquisition mechanism for all discrete inputs. Their exact electrical polarity and conditioning remain signal-specific evidence questions.

## Acquisition cadence and filtering architecture

Sensor acquisition occurs at several firmware cadences rather than one global scan:

- TPS participates in the 6.25-ms common path;
- O2 slow filtering is associated with the even 12.5-ms path;
- CTS, MAT, pump voltage, and other channels are acquired in their assigned major segments or startup paths;
- each of sixteen major segments recurs every 100 ms;
- factory test uses its own explicit sequential scan.

This architecture matters for simulation. Updating every processed sensor variable continuously would erase source ordering, sample-and-hold behavior, learned state, and filter cadence that the firmware actually implements.

## Raw, processed, and plant layers

Representative state boundaries are:

| Signal | Raw/first persistent state | Processed firmware state |
| --- | --- | --- |
| VOLT | `$007E` | voltage qualifications/offset consumers |
| O2 | `$006F/$0071` filter state | closed-loop/O2 diagnostic state |
| CTS | conversion flow into `$005D` | `$005B-$005F` and diagnostics |
| TPS | `$0081` | `$0082`, `$0086/$0087`, `$00DD/$00DE` |
| PUMPVOLT | `$007F` | Error-54/MAF diagnostic qualification |
| DIAG | immediate raw compare | mode/status state |
| MAT | complemented `$012B` | `$0060`, Error-23/25 state |
| VMAF | `$00ED` | `$00EF`, `$00EA/$00EB`, `$0063` load |

The recommended model boundary is therefore:

`physical sensor/plant (F3) -> raw hardware/HAL value (F2) -> listing acquisition and processing (F0) -> subsystem consumers (F0)`

Electrical conditioning and undocumented device behavior remain F1/F4 as appropriate.

## C and regression correspondence

The current C port already preserves raw selector calls such as `hw_adc(0x10u)`, `0x20u`, `0x40u`, `0x50u`, `0x60u`, `0x70u`, `0x80u`, and `0xA0u`. Sensor consumers are distributed across scheduler, startup, fuel/air, coolant, diagnostics, and factory-test modules. Factory test explicitly exercises all twelve selector slots.

Existing sensor, scheduler, MAF/load, coolant, diagnostic, and factory-test regressions support these translated paths. Step 142 is therefore documentation/theory integration and does not justify changing C source or frozen behavioral baselines.

## Evidence-supported theory of operation

1. External analog sensors are conditioned by signal-specific motherboard networks and presented to U10 channels.
2. Firmware explicitly selects a channel and receives an 8-bit raw result through the common A/D routine.
3. Acquisition cadence depends on startup/common/minor/major/factory execution context rather than a universal sensor scan.
4. Firmware stores, filters, complements, normalizes, learns, or table-converts each signal according to its own path.
5. Processed state is then consumed by fuel, spark, idle, emissions, transmission, and diagnostic functions.
6. In the supplied image, principal load is produced from VMAF-derived airflow and reference period; MAP/MAP2 are physically convertible but are not explicitly sampled in normal operation.
7. Pulse/event inputs such as VSS and distributor reference remain separate from U10 A/D acquisition.
8. Simulation should inject raw processor-visible values/events and let firmware perform its documented transformations.

## Remaining evidence needs

- Identify selector `$B0` from authoritative U10 information or controlled hardware testing.
- Determine whether MAP2, MAP, and ESC normal acquisition appears in other PROM variants or unavailable code.
- Measure U10 reference voltages and analog input transfer networks if electrical-accuracy simulation becomes necessary.
- Correlate factory-test `$017B-$0186` with controlled channel voltages to verify order, polarity, and scale.
- Expand the signal-specific documentation for VSS and remaining discrete inputs before assigning exact external polarity or engineering-unit pulse calibration.

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
