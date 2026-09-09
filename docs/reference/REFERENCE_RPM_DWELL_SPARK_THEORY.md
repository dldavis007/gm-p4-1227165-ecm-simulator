# Reference pulses, RPM, dwell, and spark

## Purpose and evidence boundary

This chapter integrates the established distributor-reference path, processor-visible U9 timing state, RPM derivation, dwell staging, spark-angle scheduling, EST/bypass handling, feedback diagnostics, and knock-retard contribution for the supplied BUA/9340 image.

It deliberately stops at the external boundaries of U9 (`16045148`), U11 (`16054995`), and U12 (`16034984`). Their visible pins and firmware-visible register consequences are evidence; their undocumented pulse decoding, counter topology, waveform generation, thresholds, polarity, and phase relationships are not reconstructed here.

## End-to-end timing chain

The supported engine-timing chain is:

1. External distributor activity appears on the `ESTHI`/`ESTLO` conditioning network.
2. The conditioned signal reaches U12 pin 29 `REF`.
3. U12 exposes `INJREF` pin 18 and `IGNREF` pin 19 to both U9 and U11.
4. U9 presents processor-visible reference occurrence and period/timing state in the `$3FC0-$3FFF` window.
5. Firmware detects reference occurrence, captures/normalizes period, and derives RPM.
6. Firmware calculates spark terms and dwell, converts the desired spark relationship to U9 timing quantities, and writes the established spark/dwell registers.
7. U9/U12 transform those processor-visible commands into the physical ignition/EST path.
8. Feedback and knock-related processor-visible state allow firmware to qualify the EST path and modify commanded spark.

Steps 1-3 are direct hardware endpoint evidence. Steps 4-6 and the firmware portions of 8 are direct executable evidence. The transformations inside U12/U9 in steps 3-4 and 6-8 remain custom-device boundaries.

## Reference occurrence and period

The assembled listing establishes the processor-visible reference interface:

| Listing/register | Established behavior |
| --- | --- |
| `$CAC6-$CAD3`, `$3FFA` | high-byte bit 3 reports that a distributor reference occurred during the preceding IRQ interval and sets the firmware reference flag |
| `$CB5A-$CB5D`, `$3FC0` | copies reference-period state, historically labelled the last two reference periods, into RAM `$0095-$0096` |
| `$CDC1-$CDCA` | excessive minor loops without a reference cause `$FFFF` to be written to `$3FC0` |
| `$CD2E-$CD47`, `$3FC8` | uses changing spark/reference-period state in normal spark processing |
| `$CDE6-$CE41` | normalizes reference period and derives RPM-related state |

The U9 register map also establishes `$3FEC` as counter state at the last reference. None of these addresses is assigned to an individual `INJREF` or `IGNREF` pin without independent evidence.

## RPM derivation and cylinder configuration

For the supplied calibration, PROM byte `LC009=$00`; the listing therefore uses the eight-cylinder normalization path at `$CDE6-$CDF5` and derives RPM state through `$CDF6-$CE41`, including the established `$0056/$0058` state.

This software calibration must remain separate from the physical MEMCAL cylinder input. CAL56 is the photo-authoritative 16055376 terminal 13 path through 7.5 kOhm to CAL53 and reaches U12 pin 11 `CYL`. Firmware separately compares `$002F & $18` with `LC225=$00` during injector service and uses a mismatch as the Error-41 precursor.

The coherent cylinder endpoints support a cylinder/fuel-mode consistency function, but available evidence does not prove that CAL56 directly encodes either `LC009`, `LC225`, or `$002F` bits 3-4.

CAL42/U11 pin 18 `OSC` is also a separate configuration boundary. No evidence presently ties that oscillator node to U9's reference counters or to the processor's ordinary IRQ timebase.

## Scheduler relationship

Reference edges are engine events, not 12.5-ms scheduler events. The ordinary scheduler runs at 6.25 ms, and the even Spark/O2/reference minor is selected every 12.5 ms. That even path consumes processor-visible reference state and performs reference qualification/RPM/dwell work under the established conditions.

The current scheduler regression verifies that, with valid running/reference state, connected RPM and dwell helpers execute on the even cadence. This establishes firmware consumption cadence, not distributor pulse spacing.

## Engine-running reference qualification

The translated even path preserves the listing-backed startup/running distinction:

- before `ENGINE RUNNING`, reference period must satisfy the startup threshold;
- the firmware requires the established reference-valid progression and startup counter before setting running state;
- once running, missing-reference minor loops are counted;
- the established no-reference boundary reaches the SWI wait behavior after the calibrated count;
- RPM/dwell processing can continue from valid period state according to the source conditions.

This state machine is firmware behavior. The electrical conditions by which U12/U9 decide that a physical edge becomes a processor-visible reference remain unresolved.

## Spark-angle production

The firmware spark calculation is more than a single table lookup. Existing translated and regression-tested terms include the main spark table, coolant correction, startup spark behavior, highway-mode spark, hot-restart retard, knock retard, and mode-dependent limiting/selection. These terms ultimately produce the desired spark relationship to reference.

The main and coolant table regressions preserve exact lookup/interpolation behavior from the supplied calibration. Startup-spark regressions preserve initialization, delay, injector-event decay, blend hold, and fixed-point state. These calculations are F0 firmware behavior; converting their final result into an actual crankshaft angle requires the established reference geometry and the unresolved physical U9/U12 waveform boundary.

## Angle-to-time and U9 spark staging

At `$D20C-$D267`, firmware converts desired spark angle to timing relative to the reference period, applies range/rate constraints, and stages the result in the U9-associated window.

| U9 address | Established processor-visible role |
| --- | --- |
| `$3FC8` | current spark/reference-period state |
| `$3FDC` | dwell-period value |
| `$3FE4` | next-dwell start/timing state, seeded from `$3FEC` |
| `$3FE6` | dwell delta/update state |
| `$3FE8` | current fire/fall delta |
| `$3FEC` | counter value at last reference |
| `$3FF6` | reference-to-fire offset |
| `$3FFC` | control/status word used during EST/bypass and other modes |

These meanings are established by executable use and, for several addresses, independent factory-test exercise. They are a processor-visible functional map, not a fabricated U9 internal timer specification.

## Dwell

Dwell is computed from reference-period and operating-state information and is maintained as fixed-width timing state. The translated implementation and `tests/spark_reference_regression.inc.h` preserve the listing arithmetic and the even-minor connection.

The firmware's `$3FDC/$3FE4/$3FE6/$3FEC` interactions establish when and how dwell timing values are staged. They do not establish which physical edge turns the ignition coil on, U9's timer frequency from schematic evidence, or the polarity of U9 `IGN`.

## EST, bypass, and feedback

U9 pin 7 `IGN` feeds the U12 ignition/EST path. U12 visibly receives `IGN`, `EST`, `BYPASS`, and conditioned `REF`, and returns `ESTLOOP` to U9 pin 41. Firmware changes `$3FFC` and related timing state during low-RPM/bypass transitions around `$CE5D-$CEB0` and `$D28D-$D301`.

Error 42 supplies a software feedback-consistency check. Firmware counts spark feedback in `$00B4`, evaluates it around first-reference and EST/bypass transitions at `$D271-$D2D7`, and qualifies/latches the diagnostic at `$E6AC-$E6DC`.

The closed hardware endpoints and software diagnostic are coherent, but the evidence does not prove the exact U12 `ESTLOOP` electrical transfer, the polarity of bypass/EST, or a one-to-one mapping between `ESTLOOP` and `$00B4`.

## Knock contribution to spark

Knock is a separate event path that modifies the spark result rather than defining the distributor reference.

CAL32 and external `KNOCK#` meet at U12 pin 15; U12 pin 14 `KNOCK` reaches U9 pin 42. Firmware at `$D0D1-$D157` uses changes in U9-window `$3FCA` as the knock-event quantity, applies operating qualifications and an RPM-indexed attack rate, and accumulates retard in `$00A5`.

At `$D157`, firmware halves `$00A5` before subtracting it from spark relative to reference. Segment A `$EB3A-$EB59` recovers retard over time. `$EB5B-$EBB1` and `$E6DF-$E6E9` implement the activity-based Error-43 path and fail-safe behavior.

The pin-42-to-`$3FCA` relationship remains a strong inference, not direct pin/register documentation. The separate CAL29/U10 AN9 ESC monitor is not inserted into normal knock-retard calculations because the supplied normal image does not explicitly request selector `$90` there.

## Timing model for simulation

The evidence supports a deterministic event model with two clocks of concern:

- scheduler events: 6.25-ms IRQ -> alternating 12.5-ms minor processing;
- engine events: externally generated distributor/reference events updating U9-visible occurrence/period/counter state.

A PC or embedded HAL should therefore inject raw reference events/state at F2, let F0 firmware consume `$3FFA/$3FC0/$3FC8/...` semantics in source order, and keep optional crank/engine dynamics in F3. U12/U9 pulse shaping, counter topology, waveform generation and electrical polarity remain F4 until measured or documented.

## Evidence-supported theory of operation

The integrated theory is:

1. Distributor activity is electrically conditioned into U12 `REF`.
2. U12 presents reference-related outputs to U9/U11.
3. U9 makes occurrence and timing state processor-visible.
4. Firmware qualifies reference state and derives RPM.
5. Firmware combines calibrated spark terms, startup/mode corrections and knock retard.
6. Firmware derives dwell and converts spark relative to reference into U9 timing registers.
7. U9/U12 produce the physical ignition/EST behavior and return feedback through a custom-device boundary.
8. Firmware monitors feedback and reference loss and changes EST/bypass/fault state when the established conditions require it.

This is sufficiently complete for theory-of-operation Chapter 6 while preserving the unknown device interiors.

## Remaining evidence needs

- Simultaneously observe U12 `REF`, `INJREF`, `IGNREF`, U9 `IGN`, and U12 `ESTLOOP` on a running ECM to establish phase, polarity and pulse-count relationships.
- Correlate physical reference edges with `$3FFA`, `$3FC0`, `$3FC8`, and `$3FEC` changes.
- Determine U9 counter/timer scaling from authoritative documentation or controlled measurement rather than assigning it from CAL42/U11 `OSC`.
- Establish the exact `$3FFC` bit-to-EST/bypass electrical relationships.
- Correlate U9 pin 42 knock pulses with `$3FCA` changes.
- Determine CAL53 and CAL34 motherboard destinations to improve the physical CYL/OSC configuration model.
