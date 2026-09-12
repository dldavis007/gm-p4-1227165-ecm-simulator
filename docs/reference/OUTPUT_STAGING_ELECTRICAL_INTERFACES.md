# Output staging and electrical interfaces

## Scope

This chapter integrates the processor-visible output side of the supplied 1227165 / 9340 firmware with the hardware-abstraction boundary established by the schematic work and the PC behavioral port. Its central rule is simple:

**a firmware request or raw register write is not automatically a physical voltage, current, relay state, valve position, motor torque, injector waveform, or clutch state.**

The authoritative executable evidence establishes software ordering, masks, arithmetic, RAM state, and raw writes. Schematics establish connections where visible. The PC hardware abstraction layer (HAL) exposes processor-visible state. Electrical polarity, driver behavior, load dynamics, and undocumented custom-device transformations remain separate unless independently established.

## Fidelity layers

This chapter uses the repository's Step-127 fidelity classes:

- **F0 — firmware-exact state/write:** listing-backed address, ordering, mask, branch, table, or raw write preserved by the C port.
- **F1 — hardware-connected:** connector, schematic net, component path, or MEMCAL connection is directly identified.
- **F2 — HAL signal/observer:** the PC harness supplies or observes the processor-visible quantity without pretending to reproduce the physical circuit.
- **F3 — plant/transfer assumption:** PC-only polarity, electrical transfer, mechanical response, vehicle dynamics, sensor behavior, or timing assumption.
- **F4 — external/unknown boundary:** undocumented custom-device internals, driver/transceiver behavior, physical power system, optional hardware, or other unavailable implementation.

The intended output dependency is therefore:

`F0 subsystem request -> F0 raw MPU/I/O write -> F1 proven connection -> F2 raw observer -> optional F3 plant/electrical interpretation`

Any unresolved custom-device or driver transformation remains F4 rather than being filled in for convenience.

## Segment-1 output staging

Major Segment 1 begins at listing address `$EDA3`. Step 110 closed the translated block through `$EF03` and classified it as `FULL-CORE/RAW-HAL`.

The source-shaped staging preserves:

- the two-pass `>=171` battery latch and forced-all-off continuation;
- engine-off ordinary `$D000` and field-diagnostic `$DFFF` staging;
- normal AIR, acceleration-enrichment, torque-converter-clutch (TCC), purge, fan, and exhaust-gas-recirculation (EGR) selection;
- Mode-4 raw pulse-width-modulation (PWM) overrides from `$0152/$0153/$0156/$0157`;
- calibration branch consequences for `LC014=$B4`, `LC015=$04`, and `LC017=$00`;
- direct writes to `$3FCC`, `$3FD2`, `$3FD4`, `$3FD6`, `$3FD8`, and `$4004`;
- complement-before-scaling for normal purge/EGR but not Mode-4 forcing; and
- 16-bit wrap in the source `$34xx` shift-by-two PWM construction.

The listing's `LF3B5` delays between engine-off MPU writes do not change source RAM. The PC translation therefore does not invent an equivalent physical delay. Timing consequences outside the processor-visible state remain a target/HAL boundary.

## Raw write set

The primary common-output locations currently regression-locked are:

| Location | Established role in this chapter | Evidence boundary |
| --- | --- | --- |
| `$3FCC` | Segment-1 raw MPU output word | F0 raw write; external meaning only where separately established |
| `$3FD2` | Segment-1 raw MPU output word | F0 raw write; external meaning only where separately established |
| `$3FD4` | Segment-1 raw MPU output word | F0 raw write; external meaning only where separately established |
| `$3FD6` | Segment-1 raw MPU output word | F0 raw write; external meaning only where separately established |
| `$3FD8` | Segment-1 raw MPU output word | F0 raw write; external meaning only where separately established |
| `$4004` | Parallel-I/O output byte modified by Segment 1 | F0 raw write; board/load consequence is F1-F4 by signal |

These addresses are intentionally described first as raw processor-visible outputs. A convenient subsystem name must not be promoted into a pin polarity or energized-load claim without the intervening hardware path.

`tests/output_stage_regression.inc.h` checks all five MPU words plus `$4004` and includes the integrated Step-110 output-stage signature `FAADF8A6`.

## Forced-off and diagnostic forcing

Segment 1 contains behavior that is easiest to understand as output staging rather than independent subsystem control.

With the engine-off ordinary path, the translated MPU words stage `$D000`; the field-diagnostic path can stage `$DFFF`. The battery qualification path can also force the outputs off after its source-defined two-pass qualification. Mode 4 can instead supply raw PWM override state.

These are F0 statements about the values written by the firmware. They do not establish that every external load is electrically off for one particular bit value. The physical driver topology must be consulted before assigning active-high, active-low, sourcing, sinking, protected, or fail-safe semantics.

## Injector boundary

Injector pulse-width command is not produced by the common Segment-1 output words. The established processor-visible synchronous injector command is U9 `$3FD0`, with `$3FCE` independently exercised as EFI delay. U9 then exposes `INJS` and `INJA` to U12; U12 participates in injector current sensing/limiting and drives `INJOUT` toward Q1. U11 separately exposes `INJ` toward `INJLIMP`.

The supported chain is:

`fuel calculation -> permitted injector service -> U9 $3FD0/$3FCE -> U9 custom timing -> INJS/INJA -> U12 driver/current-control boundary -> INJOUT/Q1 -> external injector circuit`

Firmware calculation, command, and service timing are F0. Visible U9/U11/U12 pin/net connections are F1. The PC may observe `$3FD0` through F2. Phase, `INJS` versus `INJA` distinction, waveform polarity, peak/hold behavior, current regulation, opening delay, fuel pressure, flow, and spray behavior remain F3/F4 unless new evidence closes them.

## Ignition / EST boundary

Spark and dwell are staged through the U9 register window rather than through the Segment-1 output words. Established firmware-visible locations include `$3FDC`, `$3FE4`, `$3FE6`, `$3FE8`, `$3FEC`, `$3FF6`, and control/status `$3FFC`.

U9 visibly outputs `IGN` to U12 and receives `ESTLOOP` feedback from U12. This creates a supported command/feedback envelope but does not document U9's internal timer architecture, output-edge polarity, dwell-driver analog behavior, coil current, or the one-to-one electrical meaning of undocumented `$3FFC` bits.

The embedded target may eventually implement these electrical interfaces; the PC firmware model should continue to stop at the established processor-visible state unless an optional F3 plant is explicitly selected.

## Idle-air-control output boundary

Idle-air-control (IAC) firmware is split between a slower regulator and a 6.25-ms one-step motor executor. The source-backed phase sequence and position bookkeeping are F0. The hardware evidence identifies the `IACA`, `IACB`, and `IACEN` interface and the stepper-motor connection as F1.

The PC HAL's named IAC observer reports position bookkeeping at `$002C`; it does not claim that this byte is a measured shaft position. Coil voltage/current, winding dynamics, missed steps, pintle travel, airflow versus position, and resulting engine torque/RPM are F3/F4 plant behavior. The existing idle plant is therefore useful simulation, not additional firmware evidence.

## TCC output boundary

TCC qualification is firmware state, and Segment 1 participates in staging the resulting raw output. The external brake series-power path is a separate hardware boundary. Consequently three stages must remain distinct:

1. firmware TCC qualification/request;
2. raw ECM output/driver command;
3. actual powered/hydraulic converter-clutch state after brake, driver, solenoid, and transmission behavior.

The first two are F0/F1/F2 as supported. Transmission ratios, slip, and vehicle response are F3. Unresolved driver and hydraulic behavior is F4. A brake-series electrical interruption must not be invented as an ECM software input merely to simplify the simulator.

## AIR, EGR, purge/CCP, fan and A/C outputs

The emissions/accessory chapter establishes the subsystem request/state producers. Segment 1 is the common raw staging boundary for several of those outputs.

The current implementation intentionally preserves raw ECM command state and raw MPU/I/O writes without assigning physical polarity. This applies to secondary AIR, EGR, canister purge/CCP, cooling fan, and related discrete/PWM outputs. Where the 1227165 schematic shows the load/driver connection, that connection is F1; active-high/active-low behavior, current capability, relay dynamics, valve response, fan inertia, compressor-clutch behavior, and similar consequences remain F3/F4 unless directly proven.

A raw `0`, `1`, `$D000`, `$DFFF`, or PWM count must therefore not be translated into phrases such as "relay energized" or "valve open" solely from software state.

## Diagnostic lamp and factory-test outputs

Diagnostic qualification and flash sequencing generate firmware state that ultimately reaches output hardware. Factory-test code also exercises raw lamp, fan, IAC, PWM, timer, and other I/O paths.

Those raw writes are useful independent evidence that the processor can command the associated output resources, but factory-test exercise does not by itself establish the normal-operation electrical polarity or load response. The FMD/SPI exchange, physical factory fixture, output-driver behavior, and reset/SWI consequences remain outside the supplied PROM where not directly documented.

## Protected outputs and driver semantics

The schematics contain external driver and protection circuitry, and custom devices U11/U12 participate in several output paths. The repository deliberately does not infer undocumented internal protection algorithms from the mere existence of sense, limit, or feedback nets.

For example, injector sense/limit connections support an F1 current-control/limiting relationship around U12, but they do not reveal the internal threshold, control law, phase, current waveform, fault reaction, or battery dependence. The same rule applies to relay/solenoid/coil outputs: visible transistor, resistor, diode, or protection topology can support the corresponding electrical statement, but software names alone cannot.

## Explicit HAL contract

Step 128 introduced `src/hal_interface.inc.h` as the explicit processor-visible seam. The output-side API currently provides:

- generic raw MPU-byte observation over `$3FC0-$3FFF`;
- generic big-endian MPU-word observation;
- raw parallel-I/O observation over `$4000-$400F`;
- raw `$5000` observation;
- synchronous injector-command observation at `$3FD0`;
- IAC position-bookkeeping observation at `$002C`; and
- named raw Segment-1 observations for `$3FCC`, `$3FD2`, `$3FD4`, `$3FD6`, `$3FD8`, and `$4004`.

The contract is intentionally **raw**. These functions do not assign voltage polarity, current, waveform shape, relay state, actuator motion, or load response. That makes the same F0 core usable by both a future embedded target and increasingly detailed PC plants without contaminating translated firmware behavior with vehicle-specific assumptions.

## Live C and regression path

The output-facing implementation is distributed across the subsystem producers and common infrastructure:

- `src/major_loop.inc.h` — one-of-sixteen major dispatch including Segment 1;
- `src/output_handlers.inc.h` — subsystem output producers and inclusion of the explicit HAL seam;
- `src/hal_interface.inc.h` — raw processor-visible input/output interface;
- `src/injector_service.inc.h` and `src/ecm_core.inc.h` — injector, spark, IAC, TCC, diagnostic and other translated state;
- `tests/output_stage_regression.inc.h` — listing-exact Segment-1 raw-output checks and frozen integrated signature;
- `tests/hal_interface_regression.inc.h` — raw HAL observer/setter coverage.

Step 110 establishes that Major Segment 1 at `$EDA3` is live in the scheduler, not a test-only helper. Step 128 adds the observer seam without changing call ordering or output semantics.

## What is established

The current evidence supports these conclusions:

- Major Segment 1 `$EDA3-$EF03` is a live F0 common output-staging block.
- Its direct raw writes to `$3FCC/$3FD2/$3FD4/$3FD6/$3FD8/$4004` are translated and regression-locked.
- Engine-off, field-diagnostic, battery-forced-off, normal subsystem selection, and Mode-4 forcing are preserved at the raw-write level.
- Injector and ignition commands use additional U9 processor-visible registers and cross into U9/U11/U12 custom-device boundaries.
- IAC, TCC, AIR, EGR, purge/CCP, fan, A/C and diagnostic outputs have firmware request/state that must remain separate from physical load behavior.
- The explicit HAL exposes raw processor-visible observations without assigning unsupported electrical semantics.

## Unresolved / evidence-limited details

This chapter does not claim:

- a complete pin-by-pin electrical decode of every Segment-1 MPU word;
- active-high/active-low polarity for an output unless separately proven by the hardware path;
- undocumented U9/U11/U12 internal register-to-pin mappings;
- injector phase/current-control algorithms or physical injector waveform;
- ignition-coil current or U9/U12 dwell/fire waveform implementation;
- IAC motor torque, missed-step behavior, pintle airflow, or exact winding current;
- TCC hydraulic state from a firmware request alone;
- relay, solenoid, valve, fan, compressor, lamp, or other load dynamics from raw register state;
- that PC plant assumptions are production electrical specifications.

These are not omissions to fill by guesswork. They are explicit F3/F4 boundaries for future schematic tracing, bench measurement, component characterization, or target-HAL implementation.

## Related references

- `docs/STEP110_WHOLE_IMAGE_COVERAGE_AUDIT.txt`
- `docs/STEP127_HARDWARE_HAL_FIDELITY_AUDIT.txt`
- `docs/STEP128_EXPLICIT_HAL_INTERFACE_AUDIT.txt`
- `docs/reference/HARDWARE_FIRMWARE_CROSS_REFERENCE.md`
- `docs/reference/U9_REGISTER_WINDOW_MAP.md`
- `docs/reference/U11_U12_FUNCTIONAL_BOUNDARIES.md`
- `docs/reference/AIRFLOW_FUEL_INJECTOR_THEORY.md`
- `docs/reference/IDLE_AIR_CONTROL_THEORY.md`
- `docs/reference/EMISSIONS_ACCESSORY_CONTROL_THEORY.md`
- `docs/reference/TRANSMISSION_TCC_THEORY.md`
- `docs/reference/DIAGNOSTICS_ALDL_COMMUNICATION_THEORY.md`
- `src/hal_interface.inc.h`
- `tests/output_stage_regression.inc.h`

This closes theory-of-operation Chapter 13 at the processor/electrical boundary while deliberately leaving unsupported driver and load behavior outside the firmware-exact layer.