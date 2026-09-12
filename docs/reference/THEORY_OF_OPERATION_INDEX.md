# ECM theory-of-operation index

This is the planned structure for the integrated theory of operation. Chapters
should link to exact assembly/listing addresses, schematic nets, C modules, and
regressions through `HARDWARE_FIRMWARE_CROSS_REFERENCE.md`.

## Planned chapters

1. **System overview and evidence method**
   - ECM 1227165 scope, PROM material, terminology, confidence language.
2. **Power supplies, reset, and retained power**
   - VIGN, VBATT, standby supply, reset controller, key-off boundary.
3. **Processor, memory map, and custom peripherals**
   - Integrated chapter: [`PROCESSOR_MEMORY_CUSTOM_PERIPHERAL_ARCHITECTURE.md`](PROCESSOR_MEMORY_CUSTOM_PERIPHERAL_ARCHITECTURE.md).
   - Covers the external program window, vectors, U10 analog acquisition,
     U9 processor-visible register window, U11/U12 external roles, MEMCAL
     configuration, and the F0-F4 simulation boundary.
   - Use [`U11_U12_FUNCTIONAL_BOUNDARIES.md`](U11_U12_FUNCTIONAL_BOUNDARIES.md)
     for the evidence-bounded external roles of U11 and U12; do not substitute
     inferred internal transfer functions for the documented F4 boundaries.
   - Use [`U9_REGISTER_WINDOW_MAP.md`](U9_REGISTER_WINDOW_MAP) and
     [`U9_REGISTER_WINDOW_CLOSURE.md`](U9_REGISTER_WINDOW_CLOSURE.md) for the
     evidence-supported `$3FC0-$3FFF` processor-visible map. Addresses without
     established semantics remain unclassified, not assumed unused or reserved.
4. **MEMCAL construction and calibration hardware**
   - EPROM, both resistor networks, J4/CAL connections, model variations.
   - Use [`MEMCAL_ARCHITECTURE.md`](MEMCAL_ARCHITECTURE.md) for the physical
     carrier and absolute J4 mapping.
   - Use [`MEMCAL_FUNCTIONAL_NETWORKS.md`](MEMCAL_FUNCTIONAL_NETWORKS.md) to
     collapse the generic zero-ohm-jumper grid into electrically meaningful
     resistor nodes tied to CAL and motherboard destinations.
   - Validate the reconstruction bidirectionally: physical/KiCad/LTspice
     evidence -> CAL destination -> theory of operation, and theory of
     operation -> expected CAL electrical role -> reconstructed network.
   - Current high-value anchors include CAL42/U11-18 `OSC`, CAL56/U12-11
     `CYL`, CAL61/U11-28 `MAP`, CAL59/VIGN, and the CAL45/CAL46 common node.
   - Use [`MEMCAL_MOTHERBOARD_FIRMWARE_PATHS.md`](MEMCAL_MOTHERBOARD_FIRMWARE_PATHS.md)
     for the evidence-classified end-to-end traces. In particular, keep the
     CAL29 analog ESC path separate from the CAL32/U12/U9 KNOCK path, and do
     not equate CAL56 directly with PROM bytes `LC009` or `LC225`.
5. **Interrupt and scheduler architecture**
   - Integrated chapter: [`INTERRUPT_SCHEDULER_ARCHITECTURE.md`](INTERRUPT_SCHEDULER_ARCHITECTURE.md).
   - Establishes the 6.25-ms ordinary IRQ, alternating 12.5-ms odd/even minor
     paths, one-of-sixteen major dispatch (100-ms cadence per segment), common
     service ordering, and separation of scheduler time from reference events.
6. **Reference pulses, RPM, dwell, and spark**
   - Integrated chapter: [`REFERENCE_RPM_DWELL_SPARK_THEORY.md`](REFERENCE_RPM_DWELL_SPARK_THEORY.md).
   - Integrates distributor conditioning, U12/U9 reference state, RPM
     derivation, spark terms, dwell/timing staging, EST/bypass feedback, knock
     retard and the event-versus-scheduler timing boundary.
   - Use [`IGNITION_INJECTION_SIGNAL_CHAIN.md`](IGNITION_INJECTION_SIGNAL_CHAIN.md)
     for the U12-to-U9/U11 reference path, U9 register-window behavior,
     EST/feedback boundary, and injector command/driver boundary.
   - Use [`OSC_CYL_REFERENCE_TIMING_PATHS.md`](OSC_CYL_REFERENCE_TIMING_PATHS.md)
     to keep CAL42/U11 `OSC`, CAL56/U12 `CYL`, and distributor-reference
     timing electrically and evidentially separate.
   - Use [`ESC_KNOCK_SIGNAL_CHAIN.md`](ESC_KNOCK_SIGNAL_CHAIN.md) for the
     separate CAL29 analog monitor and CAL32/U12/U9 event paths, knock-retard
     arithmetic, recovery, spark subtraction, and Error 43.
7. **Sensor acquisition and filtering**
   - Integrated chapter: [`SENSOR_ACQUISITION_FILTERING_THEORY.md`](SENSOR_ACQUISITION_FILTERING_THEORY.md).
   - Integrates the U10 A/D transaction, channel-specific raw/processed state,
     acquisition/filter cadence, MAF/reference-derived load, and separate
     pulse/discrete-input boundaries while preserving F0-F4 layering.
   - Use [`ADC_SENSOR_ACQUISITION.md`](ADC_SENSOR_ACQUISITION.md) for the U10
     channel/pin map, normal selector schedule, factory-test sweep, and
     raw-versus-processed RAM boundaries.
   - Use [`MAP_LOAD_SIGNAL_CHAIN.md`](MAP_LOAD_SIGNAL_CHAIN.md) to distinguish
     U10 MAP/MAP2 conversion channels and CAL61/U11 configuration from the
     supplied firmware's MAF/reference-period-derived load at `$0063`.
8. **Airflow, fuel, and injector operation**
   - Integrated chapter: [`AIRFLOW_FUEL_INJECTOR_THEORY.md`](AIRFLOW_FUEL_INJECTOR_THEORY.md).
   - Integrates VMAF/airflow and load production, crank/normal fuel selection,
     AFR/enrichment, O2/closed-loop correction, 16-cell BLM, AE/transient fuel,
     DFCO, pulse-width shaping, injector-service gating, and the U9/U11/U12
     custom-device boundary.
   - Keep calculated fuel, corrected pulse width, permitted injector command,
     U9 `$3FD0` command, and physical injector actuation as distinct stages.
   - Keep U11 `INJ`/`INJLIMP` and U12/U9 injector-driver transformations as
     custom-device boundaries unless direct electrical evidence closes them.
9. **Idle-air control**
   - Integrated chapter: [`IDLE_AIR_CONTROL_THEORY.md`](IDLE_AIR_CONTROL_THEORY.md).
   - Integrates coolant/mode target idle, 50-ms feedback regulation, fractional
     residual/quantization, 6.25-ms one-step motor service, throttle follower,
     A/C/fan anticipation, startup positioning, and key-off close/reopen homing.
   - Preserve the producer/executor distinction: regulator output can persist
     across multiple IRQs while the motor consumes at most one step per service.
10. **Emissions and accessory controls**
    - Integrated chapter: [`EMISSIONS_ACCESSORY_CONTROL_THEORY.md`](EMISSIONS_ACCESSORY_CONTROL_THEORY.md).
    - Integrates Major-Segment AIR, EGR, CCP/purge, cooling-fan and A/C control,
      their calibrated qualification/hysteresis behavior, A/C/fan idle-load
      anticipation, and the common raw Segment-1 output-staging boundary.
    - Preserve the distinction between firmware request/state and physical
      valve, relay, fan, compressor or driver polarity/dynamics.
11. **Transmission and torque-converter clutch**
    - Integrated chapter: [`TRANSMISSION_TCC_THEORY.md`](TRANSMISSION_TCC_THEORY.md).
    - Integrates selector/transmission state, Segment-E TCC qualification,
      vehicle-speed interaction, common raw output staging and the external
      brake series-power boundary.
    - Keep firmware request, raw ECM output and actual powered/hydraulic clutch
      state distinct. Ratios, converter slip and vehicle dynamics in the PC
      drive model remain F3 assumptions; unresolved driver/hydraulic behavior
      remains F4.
12. **Diagnostics and ALDL communication**
    - Integrated chapter: [`DIAGNOSTICS_ALDL_COMMUNICATION_THEORY.md`](DIAGNOSTICS_ALDL_COMMUNICATION_THEORY.md).
    - Integrates listing-backed fault qualification/history, service-lamp and
      flash-code sequencing, the 160-baud pulse-width-coded display stream,
      8192-baud SCI device `$80` Modes 0-4, and scheduler-facing Mode-4 lifecycle.
    - Preserve the boundary between firmware protocol/state and external ALDL
      transceiver voltage, polarity, collisions, physical baud timing and scan-
      tool presentation.
13. **Output staging and electrical interfaces**
    - Integrated chapter: [`OUTPUT_STAGING_ELECTRICAL_INTERFACES.md`](OUTPUT_STAGING_ELECTRICAL_INTERFACES.md).
    - Integrates live Segment-1 `$EDA3-$EF03` raw staging, `$3FCC/$3FD2/$3FD4/$3FD6/$3FD8/$4004`, injector and ignition U9/U11/U12 boundaries, IAC/TCC/emissions/accessory outputs, protected-driver limits, and the explicit raw HAL observer contract.
    - Preserve the sequence F0 firmware request/write -> F1 proven connection -> F2 raw HAL observation -> optional F3 plant/electrical interpretation, with undocumented custom-device/driver behavior retained as F4.
14. **Startup, shutdown, and exceptional modes**
    - Integrated chapter: [`STARTUP_SHUTDOWN_EXCEPTIONAL_MODES.md`](STARTUP_SHUTDOWN_EXCEPTIONAL_MODES.md).
    - Integrates the source-ordered `$C800-$C9F3` power-on path, retained-memory
      checksum/recovery, normal startup, factory-test selection/execution,
      ordinary-to-key-off transition, LF447 BLM commit, IAC close/reopen-to-park,
      terminal `$D6EA` SWI boundary, literal `$FFF0-$FFFE` vector table and
      end-to-end restart composition.
    - Preserve the distinction between F0 reset/SWI requests and F4 processor,
      keep-alive, power-rail and optional `$5800`/external `$6000` consequences.
15. **C port architecture and embedded migration**
    - Integrated chapter: [`C_PORT_ARCHITECTURE_EMBEDDED_MIGRATION.md`](C_PORT_ARCHITECTURE_EMBEDDED_MIGRATION.md).
    - Defines the strict-C89/single-translation-unit contract, exact-width and
      endian-sensitive behavior, memory/register model, scheduler/event seam,
      F0-F4 migration responsibilities and Step-128 raw HAL boundary.
    - Keep target timer/vector/A-D/capture/output/serial/power mechanics outside
      the portable F0 core; keep PC plant assumptions F3 and unresolved custom-
      device/electrical behavior F4.
    - Migrate incrementally against the frozen PC regression reference rather
      than combining algorithm refactoring with hardware replacement.
16. **Simulation and verification**
    - Unit/regression distinctions, frozen signatures, drive plants and timing.

## Chapter acceptance criteria

A chapter is complete only when it:

- distinguishes executable behavior from assigned semantics;
- cites exact assembly/listing ranges for translated behavior;
- identifies RAM, MPU, I/O and calibration addresses used;
- identifies schematic nets and explicitly marks hardware boundaries;
- links the current C implementation and live call graph;
- links regression proof or records the missing test;
- expands acronyms and initialisms on first use;
- lists unresolved and variant-dependent details without guessing.
