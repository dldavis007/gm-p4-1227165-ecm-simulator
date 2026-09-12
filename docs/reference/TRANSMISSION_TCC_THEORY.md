# Transmission and torque-converter-clutch theory

## Scope

This chapter integrates the evidence-supported transmission and torque-converter clutch (TCC) behavior of the supplied 1227165 / 9340 firmware and the current behavioral C port. It deliberately separates firmware-exact qualification and state from the external transmission, brake-power, driver, hydraulic, ratio and slip behavior used by the simulator.

The executable listing remains authoritative for firmware behavior. `docs/STEP105_TRANSMISSION_AUDIT.txt`, the current C implementation, `tests/tcc_transmission_regression.inc.h`, the schematic evidence inventory, and `simulation/transmission_drive.inc.h` are supporting cross-checks. Plant constants are simulation assumptions, not PROM calibration facts.

## Evidence classes used here

- **F0** - emitted/listing-backed software state, branches, calibration use and raw output staging.
- **F1** - directly supported connector/schematic/electrical connection.
- **F2** - processor-visible stimulus or observation exposed by the host/HAL.
- **F3** - transmission plant, ratio, slip, vehicle or electrical assumption used to exercise the firmware.
- **F4** - unresolved external driver, hydraulic, actuator or other hardware transformation.

A path can cross several classes. A firmware request may be F0 while its physical clutch consequence remains F4 and its simulated slip response remains F3.

## Firmware execution path

Transmission/TCC work is part of the ordinary major-loop schedule. The integrated scheduler chapter establishes the one-of-sixteen major-segment dispatch; the transmission audit identifies the relevant Segment-E behavior and the common raw output-staging path. The current C port keeps those concerns separate: transmission/TCC qualification produces firmware state, while common output handling stages the raw processor-visible output.

This separation is important. The model must not treat a calculated TCC request as proof that the physical clutch is energized, nor treat a simulated clutch engagement as if it were an emitted firmware instruction.

## Selector and transmission state

The supplied firmware maintains transmission/selector-related state used by the TCC decision path. Step 105 translated and regression-tested the automatic-transmission branch and preserved the manual/alternate branch as a distinct executable path rather than collapsing all vehicles into one selector model.

The current regression suite exercises the translated selector/TCC state and its interaction with vehicle-speed and operating conditions. Where the PC harness supplies selector or transmission state, that stimulus is an F2 host boundary. Any mapping from a real switch voltage, connector pin or mechanical lever position to that processor-visible state is F1/F4 unless independently established by the hardware evidence.

Do not infer a specific transmission model, gear-ratio set or hydraulic state merely from a firmware selector bit.

## TCC qualification

The listing-backed TCC path is a qualification/state machine rather than a direct physical clutch model. Its decision is conditioned by operating state represented in RAM and calibration and includes the translated warm automatic-transmission behavior exercised by `tests/tcc_transmission_regression.inc.h`.

The regression evidence is intentionally stronger than a single steady-state lock command: it checks qualification, inhibit/release behavior, hysteresis/delay behavior where translated, and transitions among representative transmission operating states. These are F0 software properties when they reproduce the listing-backed state and branch behavior.

Calibration values used by the translated routine remain calibration evidence. They must not be converted into engineering units unless that conversion is independently supported. Likewise, a host test value chosen to cross a threshold is test stimulus, not a new calibration fact.

## Vehicle speed and TCC

Vehicle speed is a separate producer feeding the transmission/TCC logic. The cross-reference places normal VSS processing in Major Segment 2 and the TCC/transmission consumer in Segment E. This producer/consumer distinction should be preserved in both documentation and simulation.

The firmware-visible VSS/status representation is F0/F2 as appropriate. Pulse frequency, tire diameter, axle ratio and vehicle acceleration belong to the F3 plant unless direct 1227165 evidence establishes them. The transmission drive model may therefore generate realistic speed trajectories without redefining firmware semantics.

## Brake release / series-power boundary

The physical TCC path includes an external brake-related series-power boundary. This is not equivalent to a firmware brake bit.

The evidence supports treating the brake path as an independent physical means by which clutch power can be interrupted/released even when the ECM-side command path is otherwise requesting TCC operation. Consequently the model should keep three states conceptually distinct:

1. firmware qualification/request state;
2. raw ECM output/driver command state;
3. actual powered/hydraulic clutch state after the brake and external hardware path.

The first is principally F0, the second is F0/F1 at the processor/electrical boundary, and the third remains F4 unless the plant intentionally supplies an F3 approximation. The external brake path must not be reverse-invented from C convenience logic.

## Raw output staging

The common Segment-1 output-staging path writes processor-visible output state after subsystem logic has produced requests. TCC therefore follows the same architectural rule used for AIR, EGR, purge, fan and A/C documentation: preserve the raw firmware write independently of external driver polarity and actuator behavior.

A raw bit value must not automatically be described as "12 V applied", "solenoid energized" or "clutch locked" unless the intervening 1227165 driver and brake-power path has been established. Firmware-exact simulation can stop at the raw output. A higher-level plant may interpret that output, but that interpretation is F3/F4 and must remain labeled as such.

## Transmission drive plant

`simulation/transmission_drive.inc.h` exists to exercise the translated firmware dynamically. Its transmission ratios, converter slip, speed response and related vehicle behavior are simulation assumptions. They are useful because they let regressions cross lock/unlock thresholds and observe state transitions under a coherent drive cycle.

Those constants do **not** establish the production Corvette transmission ratios, converter characteristics, shift schedule, hydraulic delays or clutch capacity. Changing the plant to improve test coverage therefore does not, by itself, change the firmware translation.

The correct dependency direction is:

`F3 drive plant -> F2 firmware-visible speed/selector stimuli -> F0 TCC qualification/state -> F0 raw output -> F4 external driver/brake/hydraulic clutch`

A simulation may optionally feed an assumed clutch consequence back into the F3 vehicle plant, but that feedback must not be promoted to F0 evidence.

## C implementation and regression boundary

The current implementation is distributed rather than represented by one monolithic transmission function. Relevant pieces include:

- `src/ecm_core.inc.h` for translated ECM state/logic;
- `src/major_loop.inc.h` for live major-segment scheduling;
- `src/output_handlers.inc.h` for common raw output staging;
- `simulation/transmission_drive.inc.h` for the optional F3 plant;
- `tests/tcc_transmission_regression.inc.h` for transmission/TCC regression coverage.

This arrangement is intentional. Firmware behavior remains usable without the vehicle plant, while the plant can exercise the firmware without becoming firmware evidence.

## What is established

The repository supports the following end-to-end description:

- the ordinary scheduler reaches the translated transmission/TCC major-segment work;
- selector/transmission state participates in the TCC decision path;
- vehicle-speed state is produced separately and consumed by TCC logic;
- the translated automatic-transmission TCC behavior is regression exercised;
- the resulting TCC request reaches the common raw output-staging architecture;
- a physical brake series-power boundary exists outside the firmware decision;
- the optional transmission drive model supplies ratios/slip/dynamics only as F3 simulation behavior.

## What remains unresolved

This chapter does **not** establish:

- undocumented transmission-switch electrical truth tables or connector-pin encodings not directly supported by the hardware evidence;
- a one-to-one claim that every convenient C selector state is a physical switch state;
- external TCC driver polarity where the schematic path has not closed it;
- exact brake-switch contact polarity or voltage at every point in the series-power path unless directly documented;
- hydraulic apply/release delay, converter clutch capacity or real converter slip curve;
- production gear ratios merely because the PC drive plant contains ratio constants;
- transmission variant behavior not exercised by the supplied calibration/listing.

These remain F3/F4 or variant-dependent boundaries rather than gaps to fill by assumption.

## Simulation guidance

Future transmission work should preserve this layering:

1. Keep listing-backed selector/TCC qualification, timers, hysteresis and raw output state in F0 code.
2. Expose required processor-visible selector, VSS and brake-related observations through F2 interfaces when useful.
3. Keep ratios, shifts, slip, inertia and vehicle dynamics in the optional F3 transmission/vehicle plant.
4. Keep driver polarity, brake-series electrical consequences and hydraulic clutch behavior F4 until direct evidence supports them.
5. Add regression cases when new listing-backed branches are translated; do not tune F0 code merely to make the F3 vehicle simulation look realistic.

## Related references

- `docs/STEP105_TRANSMISSION_AUDIT.txt`
- `docs/reference/INTERRUPT_SCHEDULER_ARCHITECTURE.md`
- `docs/reference/HARDWARE_FIRMWARE_CROSS_REFERENCE.md`
- `src/ecm_core.inc.h`
- `src/major_loop.inc.h`
- `src/output_handlers.inc.h`
- `simulation/transmission_drive.inc.h`
- `tests/tcc_transmission_regression.inc.h`

This chapter closes the integrated theory-of-operation slot for transmission and TCC without claiming that the external transmission or converter-clutch hardware has been reverse-engineered beyond the available evidence.