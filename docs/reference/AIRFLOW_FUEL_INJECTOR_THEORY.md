# Airflow, fuel, and injector operation

## Purpose and evidence boundary

This chapter integrates the supplied 9340 image's established airflow-to-injector path. It connects Chapter 7 sensor acquisition to MAF/airflow processing, load production, air/fuel-ratio selection, startup/cranking fuel, closed-loop correction, block-learn memory (BLM), transient enrichment, deceleration fuel cutoff (DFCO), pulse-width production, and the processor-visible injector command boundary.

The evidence boundary remains deliberately strict:

- emitted listing arithmetic, state, ordering, branches, tables, and U9 writes are firmware-exact F0;
- schematic-proven U9/U11/U12 injector connections are F1;
- raw host inputs and command observations are F2;
- engine, injector-flow, fuel-pressure, battery/electrical, and combustion models are F3;
- undocumented U9/U11/U12 phase, polarity, limiting, current control, and waveform generation remain F4.

The firmware command path can therefore be modeled exactly without pretending that the custom injector hardware has been internally reverse-engineered.

## End-to-end operating chain

For ordinary running operation the established software path is:

`U10 AN10 VMAF -> raw $00ED -> MAF/airflow -> $00EA:$00EB`

`airflow + reference period $0095:$0096 -> load $0063`

`operating state + calibrations -> commanded AFR/fuel terms`

`airflow / fuel relationship -> base injector pulse width`

`BLM + closed-loop + transient/AE + mode gates -> corrected pulse width`

`injector-output shaping -> U9 $3FD0`

`U9 INJS/INJA -> U12 injector driver -> INJOUT/Q1 -> external injector circuit`

The first six stages are processor-visible/listing-backed behavior. The final electrical waveform/current-control transformation is a custom-device/hardware boundary.

## Airflow source

The supplied image uses analog mass-airflow input `VMAF` at U10 AN10. The common 6.25-ms path requests selector `$A0`; `$F7AC-$F7B6` stores the raw sample at `$00ED` and begins the analog MAF processing path. The translated C retains this as `hw_adc(0xA0u)` and stores the raw value before filtering/scaling.

Subsequent processing produces the airflow word at `$00EA-$00EB`. This state is the principal airflow quantity used by the fuel path and also participates in BLM cell selection.

This is image-specific executable evidence. MAP and MAP2 remain physically convertible U10 inputs but are not substituted into the ordinary load/fuel chain for this image.

## Load production

At `$D769-$D7A0`, firmware combines airflow `$00EA-$00EB` with reference period `$0095-$0096`, applies the image's load scaling, rounds/saturates, and writes current load `$0063`. Previous load values shift through `$0061` and `$0062` before replacement.

The translated odd-path producer preserves this ordering. It also records the previous TPS load-axis byte before producing the new load.

The result is an engine-cycle-related load representation because reference period participates directly. It is not evidence of a hidden MAP calculation.

## Cranking versus normal fuel selection

The odd fuel path reaches a crank/normal selection immediately after load production. The engine-running state determines whether the normal running calculation or the startup/cranking branch is selected.

Cranking/startup fuel is therefore not merely a multiplier applied to the completed normal pulse width. It is a mode-specific path with its own listing-backed selection and calibration behavior. The existing startup-fuel translation and regressions should remain the authority for the exact branch arithmetic and state transitions.

Once running is established, the normal path proceeds through AFR selection, closed-loop/BLM correction, transient addition, and output shaping.

## AFR selection and enrichment

The normal fuel calculation forms a commanded air/fuel relationship from calibration and operating state. Established translated inputs include startup enrichment state, coolant-related AFR terms, load axis/state, closed-loop state, and enrichment mode flags.

Power enrichment and startup enrichment are therefore software modes that alter commanded fueling before the final pulse-width command. They must not be represented solely as arbitrary host-side injector multipliers.

The exact calibration-table meanings remain defined by their executable use. Convenient labels such as AFR, power enrichment, or coolant enrichment describe the translated role; they do not add undocumented physical stoichiometric or injector-flow assumptions.

## Oxygen feedback and closed-loop correction

Chapter 7 establishes O2 acquisition and filtering. In the fuel path, fast rich/lean processing executes before the closed-loop gate, so O2 readiness and rich/lean state can evolve even before closed-loop correction is enabled.

The normal odd fuel path then applies the listing-backed closed-loop pulse-width correction when its qualification state permits. Rich/lean transitions, closed-loop duration/state, and correction arithmetic are firmware state, not a host-side closed-loop controller.

For simulation, the engine/O2 plant may determine the raw O2 stimulus, but the firmware must retain ownership of filtering, readiness, rich/lean interpretation, and pulse-width correction.

## Block-learn memory

The supplied translation implements the listing-backed BLM cell-selection path at `$DA73-$DB35` and its update behavior.

With `LC017=0` in this image, cell selection uses whole airflow from the high byte of `$00EA`, together with RPM, rather than `$0063` filtered/current load. The translated cell layout is four airflow bands by four RPM bands, producing sixteen cells.

The current cell is retained while RPM and airflow remain inside hysteresis boundaries. A boundary crossing selects a new cell and sets the corresponding cell-change/learning-delay state. Out-of-range BLM contents trigger the listing-backed reinitialization behavior. During power enrichment, a selected BLM below neutral can be forced to neutral for current fuel correction while the stored cell itself remains distinct.

This distinction is important: BLM is persistent/adaptive fuel state selected by operating region; it is not the same quantity as the faster closed-loop O2 correction.

## BLM learning cadence

The scheduler-facing translation separates BLM cell selection from the slower BLM update tail. The established integration places BLM learning/update behavior on the appropriate slower cadence rather than changing adaptive memory on every injector event or every host simulation step.

That ordering should remain intact in any future plant simulation. Accelerating BLM simply because the PC loop runs quickly would change firmware behavior.

## Acceleration/transient enrichment

TPS and load history from Chapter 7 provide transient information. The normal fuel path includes the listing-backed acceleration-enrichment/transient stage before final injector output shaping. The translated path preserves `LF901` acceleration-enrichment addition after base pulse-width/BLM correction.

Transient enrichment is thus an additive/time-dependent firmware contribution associated with changing operating state. It should not be collapsed into the steady-state AFR table or implemented as a plant-side throttle multiplier.

The dedicated `src/dfco_transient.inc.h` translation and its regressions remain the detailed source for transient and DFCO state machines.

## Deceleration fuel cutoff

DFCO is a qualified operating mode, not simply a zero-pulse-width threshold. The translated implementation preserves the listing-backed enable/disable state and its interaction with the odd fuel path and slower scheduler state.

When DFCO qualification is active, fuel delivery is suppressed according to the firmware path; recovery returns through the documented state machine. RPM, throttle/load, temperature, timing/hysteresis, and other translated qualifiers should remain in firmware rather than being replaced with a single host-side 'decelerating' flag.

This is especially important for drive simulation because DFCO transitions affect both fuel output and subsequent closed-loop/adaptive behavior.

## Base pulse-width production

The established fuel translation computes a base injector pulse width from the firmware's airflow/fuel relationship, then applies adaptive and feedback terms in source order. The Step-31 integration already demonstrated the arithmetic/data-flow sequence:

`MAF -> AFR -> base PW -> BLM/closed-loop correction -> injector output`

Later scheduler integration established the live ordering around `$D6D1` and the common/odd paths rather than treating that early integration helper as a scheduler claim.

The distinction between arithmetic equivalence and live scheduling is retained here: a helper that produces the right number is not sufficient evidence until its source-order call path is established.

## Battery-related injector compensation

The injector-output path accepts a voltage-related injector offset/compensation term. AN1 `VOLT` is acquired separately as `$007E`; voltage-related compensation is firmware-exact where the listing performs it.

No physical injector opening-time curve, coil current, supply drop, or driver saturation model is inferred beyond that arithmetic. Those electrical effects belong in F3/F4 if a future bench-calibrated injector plant is added.

## Injector service and U9 command

Batch-injection service is gated by processor-visible status represented by sampled `$00A0` bit 6. The established service path is `$F67B-$F768`; pulse output code at `$F9D2-$F9E4` writes synchronous injector pulse width to U9 `$3FD0`.

Factory test independently exercises `$3FD0` and the EFI-delay register `$3FCE`, strengthening their processor-visible injection roles.

These facts establish the command side of injection. They do not establish whether `$3FD0` directly represents microseconds, which U9 output edge begins or ends a pulse, how INJS and INJA divide or phase injector events, or how U12 controls current.

## U9/U11/U12 injector hardware envelope

The visible hardware path is:

- U9 outputs `INJS` and `INJA` to U12;
- U12 receives injector sense/limit signals and produces `INJOUT` toward Q1/external injector circuitry;
- U11 output `INJ` reaches `INJLIMP`, providing a visible limiting/control relationship;
- U12 also receives `~LIMP` as part of its injector-driver section.

This supports a coherent injector command/limit/current-control envelope. It does not reveal the internal algorithms of U9, U11, or U12.

Accordingly the theory does **not** assign undocumented meanings such as bank A/bank B, alternating/simultaneous phase, active-high/active-low drive, peak/hold current values, or exact limiting thresholds to INJS/INJA/INJLIMP.

## Injection timing versus fuel calculation cadence

Fuel calculation cadence and physical injector-event timing are related but not identical.

MAF and TPS production occur in common scheduler work; load and the principal normal fuel calculation occur in the odd path; adaptive updates include slower scheduler state. Injector service is additionally gated by U9-derived event/status state.

Therefore the firmware can calculate/update a pulse-width command on a periodic scheduler cadence while actual injector servicing is synchronized through the custom peripheral/event boundary. A PC simulation should preserve this distinction rather than firing an injector merely because the fuel-calculation function returned a new value.

## Fuel cutoff and command gating

The final output path includes operating-state gates in addition to the calculated pulse width. Existing translation covers running/cranking state, DFCO, security/fuel authorization state, and other listing-backed suppressions before the command reaches U9.

This means a nonzero calculated base/corrected pulse width is not by itself proof that an injector pulse is commanded. Theory and tests should distinguish:

1. calculated fuel quantity/pulse width;
2. corrected pulse width;
3. permitted/suppressed injector command;
4. U9 processor-visible pulse command;
5. external electrical injector actuation.

Only stages 1-4 are presently firmware-exact/model-visible at high confidence.

## C implementation and regression correspondence

The live C architecture already reflects the established separation:

- `src/fuel_air.inc.h` — MAF, airflow, load, AFR, BLM/closed-loop and normal fuel integration;
- `src/dfco_transient.inc.h` — DFCO and transient enrichment state;
- `src/injector_service.inc.h` — event/status-gated injector service;
- `src/ecm_core.inc.h` — translated arithmetic/helpers and shared state;
- `src/segment_f.inc.h` and scheduler modules — source-order integration where applicable;
- startup/shutdown modules — lifecycle-specific fuel behavior.

Existing MAF, air, load-producer, startup-fuel, fuel/spark, DFCO, closed-loop/IAC, scheduler, injector-service/factory-test, and frozen-profile regressions collectively exercise these paths. Step 143 is therefore theory/documentation integration; it does not justify changing C source or behavioral baselines.

## Evidence-supported theory of operation

1. VMAF is acquired through U10 AN10 and transformed into firmware airflow state.
2. Airflow and distributor reference period produce the supplied image's principal load value.
3. Cranking/startup and normal-running fuel are selected by firmware mode rather than one universal steady-state equation.
4. Normal fuel combines calibrated AFR/enrichment behavior with airflow-derived base pulse width.
5. Fast/slow O2 state and closed-loop correction provide short-term feedback when qualified.
6. Sixteen-cell BLM supplies slower adaptive correction selected by RPM and whole airflow in this calibration.
7. TPS/load transients feed acceleration enrichment; DFCO can suppress fuel under its own qualified state machine.
8. Voltage-related injector compensation and final command gates are applied before processor-visible output.
9. Injector service writes synchronous pulse-width command `$3FD0` through a U9 event/status boundary; `$3FCE` is an EFI-delay register exercised by factory test.
10. U9/U11/U12 convert processor-visible commands and reference/limit state into external injector drive, but their internal phase, polarity, timing, and current-control behavior remain unresolved F4.

## Remaining evidence needs

- Establish the electrical meaning and timing relationship of U9 `INJS` versus `INJA` by controlled bench measurement or authoritative device documentation.
- Determine the exact relationship between U11 `INJ`/`INJLIMP`, U12 `~LIMP`, and injector-current limiting.
- Measure U12/Q1 injector-drive polarity, current waveform, peak/hold behavior if any, and battery dependence on an actual ECM.
- Correlate `$3FD0` and `$3FCE` writes with measured injector output timing to establish physical units and phase without guessing.
- Preserve calibration-specific BLM/load behavior when comparing other 1227165 PROM images rather than generalizing the 9340 strategy.
