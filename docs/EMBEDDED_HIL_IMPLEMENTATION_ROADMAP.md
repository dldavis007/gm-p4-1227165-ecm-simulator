# Embedded and HIL implementation roadmap

## Purpose and scope

This roadmap turns the completed behavioral C89 model into phased work packages
for a future embedded or hardware-in-the-loop implementation. It does not select
a replacement MCU, compiler, memory map, board, transceiver, or output-driver
design. Those choices require a separately approved target definition.

Step 171 remains the behavioral-port closure. Embedded work must preserve that
reference rather than reopen translated algorithms merely to fit a target.

## Governing boundary

| Class | Meaning | Migration treatment |
| --- | --- | --- |
| F0 | Listing-backed behavior, ordering, arithmetic, table or raw write | Preserve and regression-compare. |
| F1 | Independently established hardware connection | Preserve as a traceable interface requirement. |
| F2 | Processor-visible stimulus or observation at the HAL | Replace PC mechanism with target acquisition/output. |
| F3 | PC plant, unit conversion or electrical/vehicle assumption | Exclude from production firmware unless independently justified. |
| F4 | Unknown custom device, transceiver, analog, power or absent-ROM behavior | Keep explicit; do not invent an implementation. |

The target architecture is:

`target driver or ISR -> raw F2 seam -> unchanged F0 core -> raw F2 seam -> target driver`

PC-only F3 scenarios remain available as reference tests. F4 behavior remains a
boundary until new evidence or a deliberate replacement-system specification
defines it.

## Phase 0 — define the target

Before target code is written, record:

- MCU, clock, compiler version, ABI and supported C dialect;
- RAM, flash, nonvolatile and retained-memory budgets;
- timer channels and resolution for the 6.25-ms scheduler, reference/VSS capture,
  ignition/injection timing and 160-baud cell timing;
- A/D resolution, reference, acquisition time and input-conditioning ownership;
- digital I/O, serial peripherals, watchdog, reset and power-hold facilities;
- required communications and physical transceivers;
- whether the target drives an actual engine/ECM harness, a bench load, or only
  a HIL interface; and
- safety state, independent shutdown path and output-energy limits.

Exit criteria:

- a versioned target definition exists;
- every required F2 seam has an identified target resource or remains explicitly
  deferred;
- no F4 behavior has been silently converted into a requirement; and
- memory/timing feasibility has a documented margin.

## Phase 1 — target-compiler portability

Compile the translated core with the target compiler while retaining software
RAM, U9 and I/O models and host-style test injection where possible.

Verify explicitly:

- `bua_u8`, `bua_u16`, `bua_u32` and signed companion widths;
- integer promotion, signed comparison, shift and complement behavior;
- 8/16-bit wraparound, saturation and carry boundaries;
- big-endian RAM/U9 word helpers;
- static initialization and one-translation-unit include order; and
- stack, code and data usage.

Exit criteria:

- warning-free target compilation under an agreed strict-warning policy;
- focused arithmetic/endian vectors agree with the PC reference;
- no frozen PC signature changes; and
- any compiler workaround is isolated and documented rather than folded into
  listing-backed algorithms.

## Phase 2 — scheduler and timebase

Replace the host invocation of `irq_6p25ms()` with a target timer/ISR wrapper.
Keep the translated interrupt body and source ordering unchanged.

Verify:

- 6.25-ms ordinary cadence;
- alternating 12.5-ms odd/even paths;
- one-of-sixteen 100-ms major cadence;
- separation of periodic work from reference, injection and ignition events;
- worst-case execution time, interrupt latency and overrun detection; and
- monotonic timer wrap behavior used by event comparisons.

Exit criteria:

- logic-analyzer or timer-capture evidence for cadence and jitter;
- no scheduler overrun at worst-case enabled paths; and
- target scheduler trace agrees with the PC event-order reference.

## Phase 3 — raw inputs and event capture

Migrate one input seam at a time. Retain a compile-time or bench-selectable test
injection path where practical.

Recommended order:

1. raw U10 A/D channels;
2. VSS and distributor-reference pulse capture;
3. established discrete/diagnostic inputs;
4. FMD reply bytes or their deliberate replacement interface;
5. processor-visible U9-derived counters/status only where the replacement
   system has a defined producer; and
6. retained/startup samples.

For each input, record connector/net evidence, conditioning ownership, target
range, sample cadence, raw representation, fault/open/short behavior and test
vector. MAP2, MAP and ESC availability must not be mistaken for normal-image
consumption. Selector `$B0` remains unidentified.

Exit criteria for each seam:

- raw target readings can be captured without bypassing translated conversion;
- injected PC vectors and target-fed vectors produce matching F0 state; and
- engineering-unit conversion or sensor-plant assumptions remain outside F0.

## Phase 4 — raw outputs with shadow-first validation

Initially route translated outputs into shadow registers or de-energized test
loads. Observe before enabling physical drivers.

The established output set includes U9/I/O state, injector pulse-width command,
IAC position/phase bookkeeping, and raw locations `$3FCC`, `$3FD2`, `$3FD4`,
`$3FD6`, `$3FD8`, and `$4004`.

For each output:

- preserve the F0 raw value independently of physical polarity;
- define target safe/off behavior outside the translated algorithm;
- measure update cadence and transitions against PC traces;
- add open-load/short/current limiting only in the driver/safety layer; and
- require explicit evidence or a replacement-system specification before
  assigning electrical meaning to unresolved bits.

Exit criteria progress from shadow memory, to logic-level capture, to protected
dummy loads, and only then to authorized physical loads.

## Phase 5 — communications

### 8192-baud path

The translated core owns frame validation, checksum, device and Mode 0-4
interpretation. The target owns byte timing, buffering, line arbitration,
collision/error handling and the physical transceiver.

### 160-baud path

The translated manager owns normal/diagnostic/factory selection and byte/bit
state. The target owns timer compare scheduling around each cell, external line
polarity, loading and transceiver behavior.

### FMD/SPI boundary

Only established raw reply-byte behavior may enter the core. An implementation
of undocumented original-device internals is not implied.

Exit criteria:

- protocol vectors match the PC core at the raw-message/cell seam;
- timing and error tests are separately identified as target/transceiver tests;
- malformed input cannot overrun bounded buffers; and
- communication failure leaves outputs in the defined system safety state.

## Phase 6 — startup, retained state, watchdog and power

Real reset and retained memory must enter the source-ordered startup boundary,
not substitute the deterministic `ecm_reset()` regression fixture.

Verify cold start, valid retained restart, corrupted retained recovery, key-off
commit, IAC homing, software-powerdown request, watchdog/COP action and power
interruption at each relevant phase.

The target power layer owns keep-alive circuitry, brownout behavior, physical
reset, nonvolatile endurance and atomicity. The supplied PROM does not define
the external `$5800` or `$6000` implementations.

Exit criteria:

- repeatable cold/warm/corrupt-state tests;
- documented retention and endurance policy;
- no partial commit can masquerade as valid retained state; and
- independent safe shutdown remains effective if the translated core stalls.

## Phase 7 — HIL bench

Build HIL around raw seams rather than around precomputed final control results.
Minimum capabilities should include:

- programmable A/D and discrete stimuli with recorded raw values;
- phase-controlled reference and VSS pulse generation;
- timestamped raw-output capture;
- 160/8192-baud communication stimulus and observation;
- ignition/key-off, reset, retained-memory and fault injection; and
- independent output inhibit and emergency stop for energized benches.

Use a shared vector format containing test ID, evidence class, initial state,
timestamped raw inputs, expected raw outputs/state, tolerances and source
reference. Run the same deterministic vector at the PC seam and target seam
where practical.

Exit criteria:

- representative normal, diagnostic, factory and lifecycle vectors agree;
- timing tolerances are specified and measured rather than visually inferred;
- every mismatch is assigned to core, target HAL, HIL fixture or unresolved
  boundary; and
- safety interlocks are tested independently of software success.

## Phase 8 — controlled physical integration

Physical harness, actuator or engine testing is a separate authorization gate.
Proceed subsystem-by-subsystem with current-limited supplies, protected dummy
loads, independent shutdown and captured evidence before enabling higher-energy
ignition or injector outputs.

Passing PC or HIL regressions does not authorize physical operation by itself.
The production safety case, environmental qualification, EMC, fault containment
and legal/regulatory obligations depend on the selected application.

## Traceability record

Every migrated seam should record:

| Field | Required content |
| --- | --- |
| Seam | Function/API and raw representation |
| Firmware authority | Listing addresses and translated implementation |
| Hardware authority | Schematic/net/measurement or replacement specification |
| Fidelity | F0-F4 classification |
| Target resource | Peripheral, pin, channel, ISR or driver |
| Verification | PC vector, target test and HIL capture |
| Limits | Polarity, scaling, timing or behavior still unresolved |
| Safety | Off state, inhibit, fault response and independent protection |

## Milestone sequence

Recommended implementation milestones are:

1. target definition and feasibility;
2. target-compiler smoke build with software peripherals;
3. timer/scheduler equivalence;
4. one-input/one-output loopback demonstration;
5. complete raw input migration;
6. shadowed raw output migration;
7. communications and lifecycle integration;
8. complete HIL regression; and
9. separately authorized physical integration.

Each milestone should preserve the PC `make test` gate, add target-specific
evidence, and use a separate commit or release only when it creates a stable,
reproducible checkpoint.

## Stop conditions

Stop and request new evidence or a design decision when work would require:

- assigning semantics to unidentified U9/U11/U12 behavior or U10 selector `$B0`;
- inventing `$5800` or `$6000` code;
- moving an F3 plant assumption into production control logic;
- changing F0 behavior solely for target convenience;
- energizing hardware without a defined independent safe state; or
- accepting a target mismatch by weakening the PC regression baseline.

## Related documents

- [Current project status](CURRENT_PROJECT_STATUS.md)
- [Simulator, build, and testing guide](SIMULATOR_BUILD_TEST_GUIDE.md)
- [C-port architecture and embedded migration](reference/C_PORT_ARCHITECTURE_EMBEDDED_MIGRATION.md)
- [Hardware/firmware cross-reference](reference/HARDWARE_FIRMWARE_CROSS_REFERENCE.md)
- [Output staging and electrical interfaces](reference/OUTPUT_STAGING_ELECTRICAL_INTERFACES.md)
- [Startup, shutdown and exceptional modes](reference/STARTUP_SHUTDOWN_EXCEPTIONAL_MODES.md)
- [Diagnostics and ALDL communication](reference/DIAGNOSTICS_ALDL_COMMUNICATION_THEORY.md)
