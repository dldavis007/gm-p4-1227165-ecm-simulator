# Startup, shutdown, and exceptional modes

## Scope

This chapter integrates the evidence-supported lifecycle behavior of the supplied 1227165 / 9340 firmware: reset and power-on sequencing, retained-memory validation and recovery, normal startup, factory-test selection and execution, ignition-off shutdown, idle-air-control (IAC) homing, software-interrupt (SWI) boundaries, the emitted vector table, and the optional external `$5800` ROM boundary.

The authoritative firmware source remains the verified assembled `bua-hac.lst`. The historical `bua-hac.txt` is not used to resolve disagreements. This chapter consolidates the already-audited behavior from Steps 111-121; it does not introduce a new firmware algorithm.

The central lifecycle distinction is:

`external power/reset event -> F4/F2 acknowledgement -> F0 emitted $C800 power-on path -> ordinary F0 scheduler -> F0 key-off/shutdown path -> F0 SWI endpoint -> F4 physical power/reset consequence`

Retained RAM crosses that lifecycle only where the emitted code preserves it. The host must not pre-clear or pre-populate memory in ways that replace the listing-backed reset sequence.

## Evidence classes

- **F0** - listing-backed firmware-exact state, ordering, branch, table, vector or raw write.
- **F1** - directly supported hardware connection such as power, reset, connector or schematic path.
- **F2** - processor-visible or lifecycle stimulus supplied/observed by the host/HAL.
- **F3** - optional plant/electrical timing assumption used for simulation.
- **F4** - processor exception mechanics, power rails, custom devices, optional external ROM, reset physics, keep-alive behavior or other hardware not defined by the PROM.

A reset request can therefore be F0 while the electrical event that causes the processor to restart remains F4.

## Source-ordered power-on path

Step 120 is the current live authority for the integrated power-on software path. It composes the emitted `$C800-$C9F3` sequence in source order rather than treating `ecm_reset()` as physical power-on behavior.

The source-ordered path is:

- `$C800-$C824` - initial mode, stack/startup state, MPU setup and early clearing;
- `$C826-$C85D` - serial/I/O setup and the exact volatile-RAM clear through `$002E`;
- `$C85F-$C88B` - socket/ROM result handling, run control state and battery sampling;
- `$C895-$C8DF` - factory-test selection before retained-memory recovery;
- `$C8E2` - optional `$5800` expansion/HUD probe and external-call boundary;
- `$C8ED-$C90C` - retained error-RAM validation and recovery;
- `$C90E-$C939` - Error-51/lamp/checksum handling and the startup SWI boundary;
- `$C93B-$C9F3` - normal subsystem initialization, SCI setup, timer match and scheduler handoff.

This ordering matters. In particular, factory-test selection occurs before retained-memory recovery, and normal initialization does not execute when the factory path is selected.

The whole-ROM checksum result, socket-check result, battery/pump/diagnostic A/D samples, initial factory-monitor-device (FMD) byte and optional-ROM presence are explicit inputs to the host model. Their physical generation is not invented.

## Early processor, RAM and I/O initialization

The emitted reset front initializes processor-visible state before normal control begins. Step 112 records the exact early sequence, including:

- internal MPU RAM clear in `$3FC0-$3FF9`;
- serial prescaler `$8C`;
- direction register `$8F`;
- I/O control `$90`, subsequently changed to `$98` by `$C848`;
- A/D-test boundary;
- ordinary RAM clear from `$01B2` down through `$002E`;
- FMD/SPI initialization boundary;
- final early MPU control selection before branch-specific startup work.

These are F0 operations. They do not establish processor clock behavior, SPI electrical timing, A/D transfer functions or the analog/power behavior behind those registers.

## Retained-memory validation and recovery

The retained error/history area is deliberately separate from volatile startup clearing.

The listing-backed checksum helper `LF3A7` at `$F3A7` starts with 16-bit value one and adds exactly five retained error bytes `$0005-$0009` with 16-bit carry propagation. The checksum is stored at `$0018:$0019` where the relevant startup and diagnostic paths require it.

When retained data is invalid, the emitted recovery path at `$C8F5`:

1. clears `$002D` down through `$0001` but **does not clear `$0000`**;
2. recomputes the retained-error checksum;
3. invokes `LF434`;
4. initializes IAC position `$002C` to the calibrated park value 144;
5. stores `$40` at `$003D`.

`LF434` writes `$8000` to the two SAM words `$000A:$000B` and `$000C:$000D`, and writes `$80` to all sixteen BLM cells `$001C-$002B`. The emitted `$80` value controls; older host values such as 120 belong only to historical frozen replay, not current firmware semantics.

The source of the RAM contents that physically survive a vehicle power cycle remains an F4/F2 keep-alive boundary. The PC model may supply retained contents, but it must let the emitted startup code decide what is preserved, validated or recovered.

## Normal startup continuation

After the power-on front reaches `$C93B`, Step 114 preserves the emitted normal-startup order through `$C9F3`.

The path includes:

- coolant acquisition;
- conditional startup lookups and hot-restart/Error-32 qualification;
- O2-filter initialization;
- TPS A/D acquisition and processing;
- Segment-E transmission/selector initialization;
- spark-period and air/fuel-ratio startup state;
- transmission mode state;
- SCI initialization;
- MPU control/status update;
- final RAM constants;
- timer-match initialization;
- handoff to the ordinary scheduler.

The startup code calls only the source-ordered operations belonging to this path. For example, the C17A startup operation stores its lookup result at `$0013`; it is not replaced by a broader convenience helper that would also initialize state belonging to another listing path.

Normal startup therefore ends at a software scheduling boundary, not at an assumed engine-running condition. Reference qualification and the later transition to running remain part of the ordinary interrupt/control architecture.

## Factory-test entry selection

Factory-test entry is an emitted startup branch, not a host-only test mode.

The unsigned comparisons established by `$C8B2-$C8DF` select the factory path only when:

- battery A/D is below 100;
- pump A/D is at least 160;
- diagnostic A/D is from 40 through 99 according to the emitted inclusive/exclusive branch boundaries.

Comments that suggest different comparison senses do not override the emitted branch instructions.

On entry, the firmware establishes factory state including `$0047` bit 7 and `$0031` bit 2, stores the supplied whole-ROM checksum result, initializes factory counters/state, and may fill exactly `$0000-$002D` with `$AA` depending on the FMD low mode bits. The whole-ROM checksum value is an explicit boundary input; the port does not invent absent ROM bytes to obtain it.

With `$0047` bit 7 set, the IRQ path is diverted away from the ordinary scheduler and into the factory-test control/execution path.

## Factory-test control loop

Step 117 preserves the bounded control path at `$CA6F-$CA73` and `$FC72-$FD28` after startup selection.

Established behavior includes:

- the literal 24-word factory pointer table;
- +205 timer counts when `$0048` bits 0 and 1 are both clear and +11 otherwise;
- source-preserved eight-bit counter wrap;
- explicit SCI/CSR state selection;
- HAL-supplied FMD/SPI response bytes stored at `$0049/$004A`;
- `$0172` increment/reset behavior at 96;
- diagnostic A/D acceptance for 40 through 99;
- the factory low-power SWI boundary;
- the literal `$FF00` COP-restart write at `$FD23-$FD28`.

The FMD mode bits are retained numerically. Their convenient semantic names are not promoted beyond the evidence.

## Factory-test execution

Step 118 continues the live factory path through `$FD29-$FEA3`.

The emitted branches are defined by raw `$0048` state rather than by uncertain comments:

- complete-byte zero selects the zero path;
- nonzero with bit 0 clear selects the asynchronous-pulse/checksum/all-off path;
- nonzero with bit 0 set selects the A/D/output-exercise/fuel-spark path.

The translated execution preserves, among other details:

- a modulo-65536 checksum of exactly `$0000-$002D` where emitted;
- all-off synchronous-fuel and PWM writes;
- twelve factory A/D samples stored at `$017B-$0186`;
- alternating coolant samples in `$0187/$0188` on the specified count cadence;
- the emitted PWM folding/arithmetic and raw writes to the MPU/I/O windows;
- raw IAC phase exercise without assigning physical polarity;
- factory reference-period-dependent fuel, dwell, fire-offset and EFI-delay values with exact equality and wrap behavior.

The physical FMD/SPI exchange, A/D transfer, output currents, continuous wait-loop timing and reset/SWI consequences remain F1/F4 boundaries.

## Ordinary running and ignition-off entry

Once normal startup hands off, the ordinary IRQ architecture owns engine operation. Ignition shutdown is not a separate host event grafted onto that loop; the listing-backed `$D6D1-$D769` front executes on the odd 12.5-ms branch before the ordinary fuel chain.

With ignition on, the key-off timer `$008B:$008C` is reset and normal processing continues. With ignition off:

- the timer advances;
- before count 8, normal tail processing still executes;
- at/after the housekeeping boundary, run-time and mode state are cleared as emitted;
- when the engine had been running, `LF447` bounds the two SAM high bytes to 118..150 and commits them to the retained BLM cells;
- engine-running and related state are cleared;
- the listing tail at `$D727-$D769` still performs its TPS/error/history operations on nonterminal passes.

The key-off shutdown producer therefore performs real retained-state work before the terminal powerdown boundary.

## IAC close-and-park homing

The IAC shutdown state machine at `$D370-$D3DB` runs on the even 12.5-ms path, while the ordinary 6.25-ms IAC motor service remains the step consumer.

The source-ordered sequence is:

1. begin close phase by forcing modeled position to 255 and commanding closing;
2. continue closing until the position bookkeeping reaches zero;
3. switch to the open phase;
4. command opening toward calibrated park position 144;
5. on completion, set the source completion bit;
6. if ignition has returned, run the emitted restart initializer rather than remaining in shutdown state.

The unusual above-park case is preserved: it commands a positive 127-step correction rather than silently declaring completion.

This is firmware-exact command/state behavior. Stepper torque, missed steps, pintle travel, airflow and engine response remain outside F0.

## Terminal software-powerdown boundary

The key-off timer compares against `LC012=$0385`. When the terminal count is reached, the emitted path reaches the SWI at `$D6EA` before the later `$D727` tail and before the normal fuel chain.

The PC model records this exact endpoint and latches software powerdown so subsequent simulated IRQ execution stops. This latch is a host representation of the reached firmware boundary; it is not claimed to be an ECM RAM bit or the electrical implementation of keep-alive power removal.

Physical battery/ignition switching, standby rails, processor halt/reset behavior and the mechanism by which a later key-on causes reset are F4 system behavior.

## Vector table and exceptional entries

The emitted vector table occupies `$FFF0-$FFFE` and contains eight literal words:

| Vector word | Target | Evidence-supported treatment |
| --- | --- | --- |
| `$FFF0` | `$6000` | external-code boundary |
| `$FFF2` | `$C9F4` | existing IRQ front |
| `$FFF4` | `$F27B` | one emitted `RTI`, represented as immediate return |
| `$FFF6` | `$6000` | external-code boundary |
| `$FFF8` | `$C800` | reset/startup request |
| `$FFFA` | `$C800` | reset/startup request |
| `$FFFC` | `$C800` | reset/startup request |
| `$FFFE` | `$C800` | reset/startup request |

The repository deliberately does not assign exception names to slots merely from comments. The `$C800` targets establish a reset/startup request but do not prove processor, COP, clock-fail or power-reset electrical mechanics.

Likewise, the `$6000` targets are outside the supplied PROM and remain unresolved external-code boundaries.

## SWI sites

The assembled image directly proves SWI instructions at several modeled sites, including:

- `$CDCD` - missing-reference wait boundary;
- `$D6EA` - ignition powerdown boundary;
- `$FD03` - factory low-power timeout;
- `$FDB3` - factory diagnostic rejection;
- the startup retained/error path also has the audited `$C938` SWI boundary.

The software sites are F0 facts. The project does **not** connect them to a specific raw vector slot merely because a source comment names an exception. Step 119 explicitly preserves that uncertainty.

## Optional `$5800` ROM / HUD boundary

The supplied firmware tests the external `$5800` window for an expected identity/pattern and conditionally calls entry points in that external region. Major Segment 4 and other startup/major-loop hooks can dispatch there when the optional ROM is present.

Those bytes are absent from the supplied PROM. The repository therefore records calls/probes but does not reconstruct the optional heads-up-unit (HUD) implementation. This is an F4 absent-ROM boundary, not an unimplemented portion of the supplied 9340 image.

## End-to-end lifecycle composition

Step 121 verifies that the independently translated pieces compose coherently:

`reset-vector request -> $C800 power-on -> normal initialization -> ordinary odd/even IRQ operation -> ignition-off shutdown -> retained BLM commit -> IAC homing -> $D6EA SWI/powerdown boundary -> host power-cycle acknowledgement -> later $C800 reset`

The host power-cycle helper releases only the PC-side powerdown latch and re-enters through an emitted reset-vector path. It deliberately does not pre-clear RAM. The `$C800` software owns source-ordered clearing and retained-data validation.

The lifecycle regression also verifies that a valid retained state survives the appropriate reset path, corrupted retained error data invokes Step-113 recovery, and factory-test selection remains isolated from normal initialization.

## Deterministic test reset versus physical power-on

`ecm_reset()` is retained as a deterministic PC regression fixture. It is useful for tests but must not be described as the source-ordered physical power-on implementation.

The integrated power-on dispatcher introduced by Step 120 is the authority for firmware startup behavior. This distinction prevents host test initialization from being mistaken for PROM execution.

## Regression evidence

The integrated chapter is supported by the existing audits/regressions rather than new behavioral code:

- Step 111 ignition-shutdown regression and frozen lifecycle signature;
- Step 112 reset/startup comparison-boundary regression;
- Step 113 retained-memory/startup regression;
- Step 114 normal-startup regression;
- Steps 117-118 factory-test control/execution regressions;
- Step 119 vector/exception-boundary regression;
- Step 120 unified power-on-dispatch regression;
- Step 121 end-to-end ignition-lifecycle integration regression.

No Chapter-14 documentation claim requires changing those behavioral baselines.

## C implementation map

The relevant live implementation is distributed across the lifecycle modules rather than one monolithic function:

- `src/ignition_shutdown.inc.h` - `$D370-$D3DB`, `$D6D1-$D769`, retained BLM commit and SWI powerdown endpoint;
- startup/power-on code in the current core - `$C800-$C9F3`, retained validation/recovery and normal-startup composition;
- `src/factory_test_control.inc.h` - factory selection/control continuation;
- `src/factory_test_execution.inc.h` - factory execution through `$FEA3`;
- `src/vector_boundaries.inc.h` - emitted `$FFF0-$FFFE` table and bounded dispatch;
- scheduler/core modules - IRQ entry, normal odd/even operation and optional `$5800` hooks.

The historical Step 111-121 audit files remain the instruction-level checkpoint record for exact helper names and regression outputs.

## What is established

The supplied repository establishes all of the following without requiring physical-power assumptions:

- source-ordered `$C800-$C9F3` startup dispatch;
- exact volatile clearing versus retained-low-RAM handling;
- retained-error checksum and invalid-data recovery;
- listing-ordered normal subsystem startup;
- factory-test selection before retained recovery and isolation from normal initialization;
- live factory IRQ control and execution paths through `$FEA3`;
- ordinary scheduler handoff;
- key-off timer/state cleanup and BLM commit;
- IAC close/reopen-to-park behavior with park position 144;
- terminal `$D6EA` software-powerdown boundary;
- literal vector words and their bounded software destinations;
- several exact SWI instruction sites without fabricated slot assignment;
- explicit optional `$5800` absent-ROM boundary;
- coherent end-to-end lifecycle regression across reset, running, shutdown and restart.

## Unresolved boundaries

This chapter deliberately does not establish:

- physical ignition-switch, battery, keep-alive or standby-rail behavior;
- processor reset, COP, clock-fail or SWI exception-entry electrical/mechanical details;
- a comment-derived name for every vector slot;
- code or semantics for the external `$6000` targets;
- code inside the optional `$5800` ROM;
- FMD/SPI electrical timing or undocumented FMD mode semantics;
- exact A/D electrical transfer during factory testing;
- physical output consequences of factory raw writes;
- IAC mechanical motion beyond the firmware command/bookkeeping state;
- host preconditions as factory firmware behavior.

Those remain F2/F3/F4 boundaries unless new primary evidence closes them.

## Related references

- `docs/STEP111_IGNITION_SHUTDOWN_AUDIT.txt`
- `docs/STEP112_RESET_STARTUP_AUDIT.txt`
- `docs/STEP113_RETAINED_MEMORY_AUDIT.txt`
- `docs/STEP114_NORMAL_STARTUP_AUDIT.txt`
- `docs/STEP117_FACTORY_TEST_CONTROL_AUDIT.txt`
- `docs/STEP118_FACTORY_TEST_EXECUTION_AUDIT.txt`
- `docs/STEP119_VECTOR_BOUNDARY_AUDIT.txt`
- `docs/STEP120_POWER_ON_INTEGRATION_AUDIT.txt`
- `docs/STEP121_LIFECYCLE_INTEGRATION_AUDIT.txt`
- `docs/reference/INTERRUPT_SCHEDULER_ARCHITECTURE.md`
- `docs/reference/OUTPUT_STAGING_ELECTRICAL_INTERFACES.md`
- `docs/reference/HARDWARE_FIRMWARE_CROSS_REFERENCE.md`
- `src/ignition_shutdown.inc.h`
- `src/factory_test_control.inc.h`
- `src/factory_test_execution.inc.h`
- `src/vector_boundaries.inc.h`

This chapter closes the integrated theory-of-operation slot for startup, shutdown and exceptional modes while preserving the distinction between emitted software behavior and external power/processor/optional-ROM consequences.