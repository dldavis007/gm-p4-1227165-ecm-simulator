# Simulation and verification

## Scope

This chapter closes the planned integrated theory-of-operation set by documenting how the C89 model is exercised, what each class of test proves, what the frozen signatures protect, and where PC simulation ends and hardware evidence begins.

The central distinction is:

`translated firmware behavior -> controlled processor-facing stimulus -> observable raw state -> regression assertion`

Simulation is used to expose behavior and protect it from accidental change. It is not evidence that the simulated vehicle, sensor, transceiver or custom-device plant exactly reproduces production hardware unless independent evidence establishes that correspondence.

## 1. Verification layers

The repository uses several complementary verification layers.

### Focused/unit-style regressions

Small regression functions exercise a translated helper, state machine, table, boundary condition or arithmetic path in isolation. Examples include coolant, MAF, spark, fuel, DFCO, IAC, diagnostics, transmission/TCC, factory-test and vector tests.

These are closest to conventional unit tests: they isolate a specific behavior and assert exact state or boundary outcomes. They are valuable for proving edge cases that a long integrated drive may not visit reliably.

### Scheduler/integration regressions

Integration tests verify that helpers are not merely correct in isolation but are live in the source-ordered call graph. Scheduler tests verify the 6.25-ms IRQ, odd/even minor placement, major-segment cadence, asynchronous service and common-service ordering. Lifecycle tests compose startup, ordinary execution, key-off and restart.

A helper is not considered fully integrated merely because its direct regression passes; its scheduler or lifecycle entry must also be demonstrated where the original image makes it reachable.

### Scenario/plant simulations

`simulation/dynamic_drive.inc.h` and `simulation/transmission_drive.inc.h` provide deterministic PC-only profiles that stimulate the translated ECM through processor-facing inputs and event sources.

The Step-104 integrated drive commands speed, RPM, throttle-position-sensor (TPS) and mass-airflow (MAF) stimuli, then observes normalized TPS, RPM, vehicle speed, load, fuel, deceleration fuel cut-off (DFCO), injector and idle-air-control (IAC) behavior. The source explicitly identifies this as a behavioral integration profile, not a vehicle-dynamics claim.

The Step-105 transmission-aware drive goes further by deriving engine RPM in Drive from selected gear, vehicle speed, converter-slip assumption and physical TCC state. Its gear ratios, shift speeds, effective axle/tire factor and slip model are explicitly PC plant parameters, not PROM calibration facts.

## 2. Frozen behavioral signatures

The Step-104 normal-operation signature is `4BA6B7C6`. The signature is deliberately constructed byte-by-byte so it is independent of host structure padding, host byte order and whether `unsigned long` is 32 or 64 bits. Each multiply is reduced modulo 2^32.

The Step-104 hash covers, over the deterministic drive trace:

- low RAM `$0000-$01FF`;
- U9/MPU raw window `$3FC0-$3FFF`;
- parallel-I/O window `$4000-$400F`;
- the `$5000` byte;
- selected final scheduler/event statistics.

That makes it a broad behavioral guard rather than a single-output check.

The Step-105 transmission trace signature is `9732D09B`. Additional frozen signatures include the integrated Segment-D diagnostic signature `F357A5F2`, Segment-1 output signature `FAADF8A6`, and ignition lifecycle signature `16D17C9C`.

A frozen signature is a change detector, not a proof that every hashed state is physically correct. If evidence later requires a legitimate behavior correction, the regression should first demonstrate the intentional difference and the new baseline should be documented rather than silently updated.

## 3. Historical replay isolation

Later translation steps added behavior that did not exist when Step 104 and Step 105 were frozen. To keep historical comparisons meaningful, the simulator contains explicit PC-only replay switches for areas such as Segment D, Segment 1, ignition shutdown and Mode-4 lifecycle.

The old BLM initialization value is likewise restored only inside historical replay paths where necessary to reproduce pre-Step-113 signatures. These replay controls are not ECM RAM, calibration or target behavior.

This distinction is important: a frozen historical signature is preserved as a regression reference without forcing the live translated model to retain behavior later shown by the listing to be incorrect.

## 4. Determinism

The primary verification scenarios are deterministic by design. Fixed profiles, explicit raw stimuli and explicit event timing make failures reproducible and allow byte-level signatures to remain meaningful.

Determinism does not mean the real ECM or vehicle lacks noise, tolerance or timing variation. Those effects belong in separate optional robustness/plant tests. They should not contaminate the canonical evidence regression unless the variation itself is part of the behavior being verified.

## 5. Time model

Ordinary scheduler verification uses the source-backed 6.25-ms IRQ entry. Odd/even paths therefore recur every 12.5 ms, and each one-of-sixteen major segment is reached nominally every 100 ms.

Reference pulses and vehicle-speed pulses are separate phase/timing sources. The PC harness may generate them from convenient commands such as RPM or MPH, but the translated firmware consumes the resulting event/timing state. This preserves the distinction between external stimulus generation and internal firmware calculation.

A simulation that simply writes final RPM, load, fuel or TCC state would bypass the behavior under test and is therefore weaker than driving the processor-facing event/input path.

## 6. Assertions versus signatures

The strongest regressions use both explicit assertions and broad signatures.

Assertions are readable and local: they prove facts such as an exact boundary comparison, state bit, timer transition, address write or mode transition.

Signatures are broad and sensitive: they detect changes across a long trace even when no individual assertion happens to target the changed byte.

The two mechanisms complement each other. A signature-only failure can identify that something changed but not why; assertion-only coverage can miss unexpected state outside the asserted set.

## 7. Makefile regression gate

The repository `Makefile` builds only `main.c` with:

`gcc -std=c89 -Wall -Wextra -pedantic`

and runs the executable into a captured output file. It then checks the frozen Step-104/105 signatures and the numbered Step-106 through Step-121 regression result strings, and rejects unexpected `FAIL` output.

This gate verifies the historical core/lifecycle checkpoints represented in the Makefile. Later documentation-only theory steps do not themselves imply a new executable baseline.

The Step-128 HAL regression is wired through the existing regression include chain. Because the current Makefile text predates the later theory-documentation sequence, verification claims should distinguish between regressions known from prior executed audits and checks that are only statically present until the next local or CI run.

## 8. Strict compiler warnings

Historical audit checkpoints require strict C89 compilation with no warnings. This matters because implicit conversions, declaration ordering, signedness and old-C compatibility are all relevant to an eventual embedded compiler migration.

A warning-free host build is not proof of target equivalence, but new warnings are treated as regression signals rather than ignored as cosmetic output.

## 9. Simulation fidelity classes

The F0-F4 vocabulary also applies to verification:

- **F0** — executable/listing-backed behavior directly asserted or observed;
- **F1** — hardware connection established independently and used to frame a test boundary;
- **F2** — raw processor-facing stimulus or output observation supplied by the harness;
- **F3** — optional vehicle, mechanical, electrical or sensor plant assumption used to create a useful scenario;
- **F4** — behavior deliberately not simulated because the custom/external hardware is unresolved.

A successful F3 simulation does not promote its plant constants to F0 or F1 evidence.

## 10. What the dynamic-drive regression proves

The Step-104 integrated drive checks interaction across multiple subsystems. The visible source includes checks that reference qualification catches the engine, cranking hands off to normal fuel, TPS is produced each IRQ, acceleration increases load and fuel, asynchronous acceleration enrichment reaches the raw MPU trigger, BLM cell selection/learning occurs, cruise VSS/RPM settle near commanded values, DFCO enters and writes zero fuel, fuel resumes, and injector-service bookkeeping remains consistent.

Its frozen signature extends coverage beyond those named checks.

What it does **not** prove is factory-accurate vehicle acceleration, exhaust chemistry, exact sensor voltage/noise behavior, or exact production engine response.

## 11. What the transmission-aware simulation proves

The Step-105 plant deliberately prevents independent RPM command while in Drive. Instead, gear, speed, converter-slip assumption and clutch state produce the engine-speed stimulus; the ECM then measures VSS/reference pulses and runs its N/V and TCC logic.

This makes the scenario useful for validating feedback between translated transmission/TCC behavior and a simple plant. It also exercises the external brake-series-power distinction: the plant can remove physical TCC power while the ECM command remains logically present.

Its shift thresholds, ratios, smoothing and slip values remain F3 assumptions unless separately supported by hardware/service evidence.

## 12. Lifecycle and exceptional-mode verification

Steps 111-121 extend verification beyond normal driving. The suite covers key-off timing, BLM commit, IAC homing, software-powerdown boundary, retained-memory recovery, normal startup, 8192-baud protocol/Mode 4, factory-test control/execution, vector dispatch, unified power-on and end-to-end reset/shutdown/restart composition.

This matters because normal-drive coverage alone would leave substantial listing-backed behavior unverified.

## 13. Regression evidence versus code coverage

The project uses evidence-oriented behavioral coverage rather than claiming formal branch, MC/DC or instruction-by-instruction coverage. Step 122's whole-image closure concluded that no known reachable internally defined firmware area remained classified as unported, but that is not equivalent to formal exhaustive path coverage or whole-machine equivalence.

Optional ROM, custom-device internals, physical electrical timing and absent hardware behavior remain outside that claim.

## 14. Recommended verification discipline for future changes

For any future behavioral change:

1. identify the listing or hardware evidence requiring the change;
2. add or update the smallest focused regression first;
3. verify source-order/scheduler integration if the path is live;
4. run the strict C89 build and complete regression suite;
5. compare frozen signatures;
6. if a signature changes intentionally, document exactly why and which evidence required it;
7. keep PC-only plant changes separate from F0 algorithm changes;
8. when adding target hardware, run the same processor-facing vectors against both PC and target implementations where practical.

For refactoring with no intended behavior change, the frozen signatures and all exact assertions should remain unchanged.

## 15. Current limitations

The present verification framework is strong for deterministic behavioral regression, but it is not yet a complete hardware-in-the-loop system and does not establish:

- exact analog noise/tolerance behavior;
- production ALDL electrical timing/loading;
- U9/U11/U12 internal timing/current-control behavior;
- injector hydraulic/fuel-delivery dynamics;
- ignition-coil electrical dynamics;
- exact transmission hydraulic response;
- processor-cycle timing for every translated C operation;
- exhaustive formal path coverage.

Those limitations are explicit rather than hidden behind the PC model.

## 16. Closure of the planned theory set

With this chapter, all sixteen planned theory-of-operation chapters have an integrated reference document or an explicit evidence-boundary description. The theory set should now be maintained as a living reference rather than extended merely by increasing step numbers.

Future numbered steps should correspond to new evidence, corrections, implementation work or verification improvements, not to recreating already integrated chapters.

## Related evidence

- `simulation/dynamic_drive.inc.h` — deterministic integrated normal-operation profile and Step-104 signature.
- `simulation/transmission_drive.inc.h` — deterministic PC-only transmission/TCC plant and Step-105 signature.
- `tests/` — focused and integration regression fragments.
- `Makefile` — strict-C89 build and regression gate.
- `main.c` — single-translation-unit harness and regression entry point.
- `docs/STEP104_BASELINE_FREEZE.txt` and `docs/STEP104_SIGNATURE.txt` — frozen baseline evidence.
- `docs/STEP105_TRANSMISSION_AUDIT.txt` — transmission-aware simulation boundary.
- `docs/STEP110_WHOLE_IMAGE_COVERAGE_AUDIT.txt` and `docs/STEP122_WHOLE_IMAGE_CLOSURE_AUDIT.txt` — coverage/closure context.
- `docs/STEP121_LIFECYCLE_INTEGRATION_AUDIT.txt` — end-to-end lifecycle integration.
- `docs/STEP127_HARDWARE_HAL_FIDELITY_AUDIT.txt` — F0-F4 fidelity vocabulary.
- `docs/reference/C_PORT_ARCHITECTURE_EMBEDDED_MIGRATION.md` — migration and regression-preservation guidance.
