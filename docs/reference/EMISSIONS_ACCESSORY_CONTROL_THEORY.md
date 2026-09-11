# Emissions and accessory controls

## Purpose and evidence boundary

This chapter integrates the supplied 9340 image's established secondary-air, exhaust-gas-recirculation (EGR), canister-purge (CCP), cooling-fan, and air-conditioning (A/C) control behavior. It connects the already translated major-loop control decisions to their raw output-request state while preserving the hardware boundary at the output drivers, relays, valves, compressor clutch, and other external loads.

The evidence classes remain:

- **F0** — listing-backed control state, calibration, arithmetic, scheduling, and raw output writes;
- **F1** — schematic-proven net/load connection;
- **F2** — raw input/output observation at the HAL boundary;
- **F3** — external plant/load behavior, pressures, temperatures, actuator dynamics, and electrical polarity assumptions;
- **F4** — unresolved driver/transceiver/electrical behavior not established by the supplied evidence.

The chapter therefore describes when firmware requests a function, not an invented active-high/active-low electrical implementation.

## Major-loop organization

These controls are slower supervisory functions distributed through the sixteen-way major-loop scheduler rather than one monolithic emissions task. Individual segments recur nominally every 100 ms. Existing translations place:

- secondary-air management in Major Segment 9;
- EGR in Major Segment B;
- canister purge in Major Segment C;
- A/C/IAC interaction in Major Segment 5;
- cooling-fan decision logic in its translated major-loop path;
- final raw output staging in Major Segment 1.

This distinction matters because decision cadence and electrical output update cadence are separate software stages.

## Secondary-air management

The translated `$E34E-$E432` path preserves secondary-air management as raw request bits in `$003E` rather than assigning valve polarity.

The 9340 calibration `LC015=$04` selects the source's two-air-valve configuration. The decision path considers coolant, operating/mode state, closed-loop history, RPM, power-enrichment state, O2 rich/lean behavior, load, vehicle speed, and load-drop history.

Important established behaviors include:

- coolant below the calibrated minimum or the applicable mode inhibit diverts/disables the ordinary air request;
- current/prior closed-loop state affects delay/history behavior;
- sustained high RPM can force the divert path;
- power enrichment immediately enters the calibrated divert behavior in this image because the PE count calibration is zero;
- sustained rich/lean O2 conditions maintain their own counters and can change diversion state;
- low load and the high-load/high-VSS combination can inhibit the normal two-valve request;
- after closed-loop history or a qualifying load drop, the one-valve path can be selected;
- `$003E` bits 1 and 2 remain raw software requests.

The translated calibration thresholds include RPM/25 160, low/high load 25/100, high VSS 60, minimum coolant 73, and the image-specific two-valve calibration value 4. These are executable calibration facts, not statements about external valve airflow or polarity.

## Secondary-air output boundary

Normal Segment-1 output staging later converts the raw AIR request state to processor/output-device counts. The current translation deliberately treats those as software-side commands and leaves electrical polarity and valve mechanics to the HAL/hardware boundary.

Accordingly, Chapter 10 distinguishes:

`AIR decision state -> raw request bits -> raw output staging -> external valve/relay hardware`

Only the first three stages are firmware-exact.

## EGR control

The surviving listing around `$DEE5-$DF49` establishes the EGR path despite a known unrelated pasted-source fragment in the historical source text. The assembled/listing-backed surrounding behavior is sufficient to preserve the executable EGR calculation.

The EGR path is inhibited or qualified by Park/Neutral state, ALDL diagnostic mode, manifold-air-temperature (MAT), throttle position, power-enrichment state, RPM, load, coolant, and prior EGR command state.

The translated thresholds preserve:

- zero-command turn-on TPS threshold = 8;
- nonzero-command hold TPS threshold = 5;
- minimum MAT = 30;
- RPM/25 capped at 80 for table use;
- load capped at 160 for table use.

Park/Neutral normally inhibits EGR, while the diagnostic-mode path can bypass that inhibit. Power enrichment inhibits EGR.

For the supplied 9340 calibration, every reachable C23B base-duty cell is 255. Consequently ordinary RPM/load variation does not change the base table result in this image. Coolant multiplication remains active and is the principal calibrated modulation after the qualification gates.

This is a calibration-specific simplification, not a universal P4 EGR algorithm claim.

## EGR output state

The resulting EGR command is stored as the software duty/PW byte represented by `$0112`. Segment-1 output staging converts that byte to the output-device timing/count representation.

The firmware-exact chain is therefore:

`qualifications -> base duty/table -> coolant multiplier -> $0112 -> raw output staging`

Valve flow, vacuum response, exhaust dilution, electrical polarity, and mechanical lag remain plant/hardware behavior.

## Canister purge / CCP

The translated `$E75D-$E816` Major Segment C path implements canister purge (CCP). It is stateful rather than a simple on/off threshold.

Established qualification includes:

- a run-time latch; purge remains disabled through the calibrated run-time boundary of 80;
- ALDL diagnostic mode inhibits ordinary purge;
- minimum coolant is 147;
- DFCO inhibits purge;
- an optional closed-loop-related gate depends on `LC014` and diagnostic state;
- vehicle speed, TPS, and airflow use separate turn-on and hold thresholds based on whether purge was already active.

The calibrated turn-on/hold thresholds are VSS 16/10 and TPS 10/5. Airflow must be strictly above its calibrated boundary.

When qualified, the C272 MAF table maps airflow to requested purge duty. In this 9340 image, the C27C load-gain table is uniformly 128, so load cannot change requested purge duty even though the algorithm retains the load-related structure.

The request is passed through the source lag filter. `$0114` holds the stateful minimum requested-duty latch and `$0113` is the filtered actual command; `$003B bit0` records purge-on state.

Thus the calibration-specific purge chain is primarily:

`run/coolant/mode/VSS/TPS/airflow qualification -> MAF duty -> fixed load gain -> filtered $0113 command`

## Cooling-fan control

The existing Step-79 fan translation and regression establish a stateful fan command with hysteresis and hold timing rather than a single temperature comparator.

Regression evidence confirms distinct behavior for A/C/request state, vehicle speed, coolant thresholds, prior fan state, and a calibrated hold timer. Representative verified boundaries include different on/off behavior around coolant values 197/199/205/206/207 and a 75-count hold interval in the exercised path.

Those values are retained as executable/regression evidence. This chapter does not assign physical degrees, relay polarity, fan speed, or airflow to the raw values without separate evidence.

The raw fan command is subsequently staged by the normal output path. The physical relay/motor circuit remains F1/F3/F4 according to what the schematic and future measurements establish.

## A/C control and idle-load anticipation

A/C is both an accessory-output function and an idle-load-management input. Chapter 9 covers the IAC consequences; this chapter places those consequences in the broader accessory-control envelope.

The exact Segment-5 continuation around `$E005-$E07B` preserves compressor on/off state transitions, A/C learned-step bounds, learning timing, Park/Neutral gain, and the accumulated learned anticipation term.

Established calibration values in that translated continuation include:

- learned A/C step maximum 32;
- learned minimum 1;
- maximum positive learning change 5;
- learning time 20;
- Park/Neutral gain 128.

The software observes A/C state, learns the IAC movement associated with compressor loading, bounds/rate-limits the learned value, and applies mode-dependent gain before accumulating the anticipation term.

This supports the theory that A/C load compensation is anticipatory firmware behavior rather than merely feedback after RPM has already fallen.

## A/C request versus physical compressor

The firmware's request/status/anticipation state must remain separate from physical compressor-clutch engagement. The compressor clutch, pressure switches/cycling behavior, refrigerant pressure, and engine torque draw are external hardware/plant concerns unless explicitly proven in the retained ECM evidence.

For simulation, it is valid to use a separate F3 compressor-load model that reacts to an F2 software command/status boundary, provided that the model does not feed precomputed IAC corrections back into F0 state.

## Fan and A/C interaction

The output staging and IAC translations show that fan and A/C states can affect idle-control anticipation and output requests. Their software coupling should therefore be modeled through the existing firmware state rather than by directly altering target RPM or IAC position in the plant.

The supplied 9340 IAC analysis specifically established that A/C/fan calibrations act as step-position anticipation in the translated path rather than simple target-RPM additions at the normal commanded-idle calculation.

## Output staging

Normal Segment-1 output staging converts the higher-level AIR, enrichment/accessory, TCC, purge, EGR, and fan request state into raw processor/output-device values. For purge and EGR, duty bytes are converted to the corresponding count representation; AIR and other boolean requests map to their raw staged states.

The translation intentionally avoids assigning external active-high/active-low semantics. This preserves a clean boundary:

`control algorithm (F0) -> raw staged register/count (F0/F2) -> driver/load electrical behavior (F1/F3/F4)`

Mode-4 and diagnostic forcing remain separate control paths and must not be confused with ordinary-operation output decisions.

## Cross-subsystem dependencies

These controls depend on state produced elsewhere:

- coolant/MAT from Chapter 7;
- airflow/load from Chapters 7 and 8;
- TPS and VSS from sensor/event acquisition;
- O2 and closed-loop state from fuel control;
- PE and DFCO from Chapter 8;
- Park/Neutral/transmission state from Chapter 11;
- A/C and fan anticipation state from Chapter 9.

This explains why the major-loop controls should not be simulated as isolated thermostats or switches. They are consumers of shared ECM state with source-ordered hysteresis and timers.

## C implementation and regression correspondence

The current translation separates these functions into dedicated modules while preserving the common output-staging boundary:

- `src/air_control.inc.h` — secondary-air management;
- `src/egr_control.inc.h` — EGR qualification and duty;
- `src/purge_control.inc.h` — CCP qualification/filtering;
- `src/ac_control.inc.h` — A/C/IAC control continuation and learned anticipation;
- existing fan translation in the core/major-loop implementation;
- `src/output_handlers.inc.h` / major-loop integration — raw output staging.

Existing AIR, EGR/fan, purge, A/C/IAC, output-stage, major-loop, Mode-4, and drive-cycle regressions provide behavioral proof. Step 145 is theory/documentation integration only and does not require C or frozen-baseline changes.

## Evidence-supported theory of operation

1. The major-loop scheduler evaluates AIR, EGR, purge, fan, and A/C functions on their source-assigned slower cadences.
2. Each function applies its own calibrated qualifications, hysteresis, histories, and timers using shared sensor/mode state.
3. AIR produces raw valve/request bits but external valve polarity is unresolved.
4. EGR produces `$0112`; in the 9340 calibration the reachable base-duty table is saturated at 255 and coolant remains the active modulation.
5. Purge produces a filtered `$0113` duty command; this calibration's load gain is fixed at 128 while MAF remains active.
6. Fan control is stateful, with coolant/A-C/VSS conditions, hysteresis, and a verified hold timer.
7. A/C control includes learned IAC anticipation and mode-dependent scaling; physical compressor load remains external.
8. Segment-1 staging translates these software requests to raw hardware-facing counts/register state.
9. Electrical polarity, relay/solenoid dynamics, air/vacuum flow, compressor torque, and fan thermal performance remain outside firmware-exact behavior.

## Remaining evidence needs

- Trace each AIR/EGR/CCP/fan/A-C staged output through the retained schematic to the final connector/driver pin and document polarity only where directly proven.
- Bench-correlate raw Segment-1 output values with valve/relay/clutch voltages on an ECM.
- Determine the physical units of the coolant/MAT/fan thresholds only from authoritative calibration conversion or controlled measurement, not labels alone.
- Measure EGR/CCP actuator response if an emissions plant is ever added to the simulator.
- Characterize fan and compressor torque/thermal loads separately as F3 plant models rather than embedding them in firmware logic.
