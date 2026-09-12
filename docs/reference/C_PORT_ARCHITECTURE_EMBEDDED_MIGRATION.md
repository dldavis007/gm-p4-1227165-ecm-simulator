# C port architecture and embedded migration

## Scope

This chapter describes how the evidence-backed 1227165 translation is organized as a C89 PC model and which boundaries must be preserved if the translated core is moved to an embedded target. It is an architecture and migration guide, not evidence that the original ECM was written in C and not a claim that the present PC harness is cycle-exact hardware emulation.

The governing rule is the same one used throughout the reference set: preserve established firmware behavior first, then isolate target-specific mechanisms behind explicit interfaces. Do not replace listing-backed arithmetic, ordering, state or raw register semantics with a more convenient host interpretation.

## 1. Single translation unit

`main.c` intentionally compiles the implementation as one C translation unit. The `.inc.h` files are implementation fragments, not independently compiled public headers. Their order preserves the historical static linkage, declaration order and arithmetic behavior inherited from the frozen Step-104 port.

The current organization is:

- `src/` — translated ECM behavior, RAM/MPU model, scheduler and hardware-facing support;
- `simulation/` — PC-only engine/vehicle/cluster plants and scenario support;
- `tests/` — regression models and regression entry points;
- `main.c` — include map, demonstration harness and regression entry point.

This organization is deliberate. Splitting fragments into independent object files is a possible future refactor, but it is not required for behavioral fidelity and must not silently change file-local state, declaration visibility, initialization order or integer behavior.

## 2. C89 portability contract

The build contract remains strict C89:

`gcc -std=c89 -Wall -Wextra -pedantic main.c`

The translated core therefore avoids depending on C99/C11 language features. Exact-width intent is expressed through the project `bua_u8`, `bua_u16`, `bua_u32`, signed companion types and explicit masking/casting where the original processor behavior requires bounded arithmetic.

The important portability requirement is behavioral width, not the host compiler's native `int` size. Any embedded migration must revalidate:

- 8-bit and 16-bit wraparound at translated instruction boundaries;
- signed versus unsigned comparisons;
- big-endian RAM/MPU word helpers;
- shifts, complements and promotions that can differ when host/target integer widths differ;
- explicit saturation or carry behavior where the listing establishes it.

A compiler accepting the source is not sufficient proof of equivalence.

## 3. Memory and processor-visible state

The PC model represents low RAM, the U9 `$3FC0-$3FFF` processor-visible window and parallel I/O as software state. Helpers such as `RAM8`, `ram16be_get`, `ram16be_set`, `mpu16be_get` and the raw HAL observers keep address-level behavior visible to the translation.

For an embedded target there are two legitimate strategies:

1. retain the software memory model and have a hardware abstraction layer (HAL) synchronize the required external state; or
2. map selected established addresses onto real target registers or driver state.

The second strategy must not be done merely because an address has a convenient semantic label. U9, U11 and U12 include unresolved custom-device behavior. The C port establishes processor-visible behavior at many boundaries but does not reproduce undocumented internal silicon transformations.

## 4. Scheduler and event model

The ordinary firmware timing model is source-derived:

- 6.25-ms ordinary interrupt service;
- alternating 12.5-ms odd/even minor paths;
- one-of-sixteen major dispatch, giving each major segment a nominal 100-ms cadence;
- reference/injection/ignition events kept distinct from periodic scheduler time.

A PC call to `irq_6p25ms()` is a simulation mechanism for entering the translated interrupt path. On an embedded target, a hardware timer interrupt may replace that host mechanism, but the ordering and cadence of translated work must remain unchanged unless listing evidence supports a change.

Reference pulses, vehicle-speed pulses and other asynchronous inputs likewise should enter through explicit event/HAL boundaries. They should not be converted into precomputed control results outside the translated firmware.

## 5. F0-F4 boundary for migration

The hardware/firmware cross-reference defines the migration boundary:

- **F0** — listing-backed firmware-exact state, arithmetic, branch, ordering, table or raw write;
- **F1** — hardware connection established by schematic, connector, MEMCAL or physical evidence;
- **F2** — processor-visible input/output supplied or observed through the HAL;
- **F3** — simulator plant, engineering-unit conversion, electrical assumption or vehicle dynamics;
- **F4** — unresolved custom-device, analog, transceiver, power or other external hardware behavior.

Embedded migration should preserve F0 and F1, replace PC F2 mechanisms with target-specific acquisition/output code, omit PC-only F3 plants unless they are needed for test, and leave F4 unresolved rather than fabricating it.

## 6. Explicit HAL seam

Step 128 established `src/hal_interface.inc.h` as the explicit raw processor-facing seam. Current named inputs include O2 and battery raw A/D values plus the already-existing VSS and reference-RPM pulse-source helpers. A bounded low-RAM setter exists for tests where the processor-visible byte is established but the external transfer remains unresolved.

Output observers expose low RAM, U9 MPU state, `$4000` I/O state, `$5000`, synchronous injector command `$3FD0`, IAC position bookkeeping and the established Segment-1 raw outputs `$3FCC/$3FD2/$3FD4/$3FD6/$3FD8/$4004`.

These accessors deliberately do not assign voltage polarity, driver current, waveform shape, actuator force or hydraulic behavior. That is the correct migration seam: target drivers can consume or produce established raw state without contaminating the translated algorithm with board-specific implementation.

## 7. What belongs in target-specific code

A target port should place these mechanisms outside the F0 translated core:

- timer setup and interrupt-vector syntax;
- A/D peripheral setup and raw channel acquisition;
- digital input capture and pulse timestamping;
- physical output-driver writes;
- serial peripheral setup and ALDL transceiver handling;
- watchdog/COP servicing mechanics;
- nonvolatile/retained-memory implementation;
- physical power, reset and keep-alive sequencing.

Where the original listing writes a raw U9 or I/O value, the translated write remains F0. The target-specific consequence of that value is a separate layer unless direct hardware evidence establishes the transformation.

## 8. What must not migrate from the PC plant into firmware

`simulation/` contains useful test behavior, but it is not original ECM firmware. In particular, transmission ratios, converter slip, axle/tire relationships, vehicle acceleration, sensor transfer assumptions and similar plant constants are F3 unless independently established.

An embedded port must not copy those values into the translated controller merely because they produce plausible PC behavior. The target should supply real processor-facing sensor/event state instead.

## 9. Startup, reset and retained state

`ecm_reset()` remains a deterministic PC regression fixture. The source-ordered live power-on authority is the Step-120 `$C800-$C9F3` dispatcher, with Step-121 composing reset-vector acknowledgement, ordinary operation, key-off shutdown, software-powerdown boundary and later restart.

On real embedded hardware, reset entry, retained-memory survival, watchdog behavior and keep-alive power are target/HAL responsibilities. The target must enter the translated source-ordered startup at the established boundary rather than substituting the convenient regression reset fixture.

## 10. Interrupt and exceptional-entry syntax

The source image's vector table and exceptional boundaries are documented separately. C compiler interrupt attributes, vector-section pragmas, linker scripts and startup assembly are inherently target/toolchain-specific and should not be embedded into the portable translated algorithms.

The correct pattern is:

`target vector/ISR wrapper -> translated event or scheduler entry -> F0 behavior`

Similarly, SWI/reset requests generated by translated code should be handed to a target boundary. The portable core must not invent processor reset or power consequences that the current evidence does not establish.

## 11. Regression-preserving migration sequence

A safe embedded migration is incremental:

1. freeze the current PC regression output and signatures;
2. compile the unchanged translated core with the target compiler while retaining software RAM/MPU models;
3. verify type widths, endian helpers and arithmetic boundary tests;
4. replace the 6.25-ms host scheduler trigger with a target timer wrapper;
5. migrate one F2 input at a time to real hardware acquisition and retain a test injection path where practical;
6. migrate one raw output boundary at a time to target drivers while retaining raw observation;
7. keep F3 plants available only in the PC test build;
8. validate startup/retained-state behavior separately from ordinary scheduling;
9. preserve an automated PC build as the behavioral reference throughout the target migration.

This sequence limits the number of simultaneous changes and makes a mismatch attributable to a specific hardware seam rather than to a broad rewrite.

## 12. Build and regression evidence

The repository Makefile builds only `main.c` with strict C89 flags and runs the historical Step-104/105 signatures plus the Step-106 through Step-121 regression gates. Step 128 added a 15-check HAL regression through the existing regression include chain rather than reordering `main.c`.

The GitHub connector cannot execute the build, so documentation-only theory steps do not claim a fresh runtime result. Existing audit records distinguish tests that were actually executed from later static wiring that still requires a local or CI run.

## 13. Current migration readiness

The project is already structured well for a staged embedded port because the difficult separation has largely been made explicit:

`listing-backed F0 core -> raw F2 HAL seam -> target hardware`

with

`PC-only F3 simulation plant`

kept outside the firmware authority and unresolved F4 behavior left explicit.

The remaining work is primarily engineering integration rather than re-deriving controller algorithms: define the target MCU/toolchain, implement target timer/A-D/capture/output/serial/retained-memory wrappers, and prove each replacement against the PC reference.

## 14. Unresolved or target-dependent items

This chapter intentionally does not choose a replacement MCU, compiler, memory map or board. It also does not resolve U9/U11/U12 internal behavior, ALDL electrical characteristics, injector-driver current control, TCC hydraulics, power-hold circuitry or optional external ROM behavior.

Those are separate hardware-selection or evidence tasks. They should remain explicit boundaries until a specific embedded target and sufficient electrical evidence are available.

## Related evidence

- `README.md` — current one-translation-unit build and directory contract.
- `main.c` — actual include order and PC harness entry point.
- `Makefile` — strict C89 build and regression gate.
- `docs/STEP104_MODULAR_REFACTOR_AUDIT.txt` — historical modularization boundary.
- `docs/STEP127_HARDWARE_HAL_FIDELITY_AUDIT.txt` — F0-F4 fidelity classification.
- `docs/STEP128_EXPLICIT_HAL_INTERFACE_AUDIT.txt` — explicit HAL seam.
- `src/hal_interface.inc.h` — current raw input/output accessors.
- `docs/reference/INTERRUPT_SCHEDULER_ARCHITECTURE.md` — scheduler/event timing.
- `docs/reference/PROCESSOR_MEMORY_CUSTOM_PERIPHERAL_ARCHITECTURE.md` — address/custom-device boundaries.
- `docs/reference/STARTUP_SHUTDOWN_EXCEPTIONAL_MODES.md` — startup/reset/retained-power boundary.
- `docs/reference/OUTPUT_STAGING_ELECTRICAL_INTERFACES.md` — raw-output/electrical boundary.
