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
   - RAM, internal registers, external PROM, I/O windows, vectors.
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
   - 6.25-ms IRQ, odd/even minors, sixteen major segments, event interrupts.
6. **Reference pulses, RPM, dwell, and spark**
   - Distributor reference geometry, counter capture, advance and knock.
   - Use [`IGNITION_INJECTION_SIGNAL_CHAIN.md`](IGNITION_INJECTION_SIGNAL_CHAIN.md)
     for the U12-to-U9/U11 reference path, U9 register-window behavior,
     EST/feedback boundary, and injector command/driver boundary.
7. **Sensor acquisition and filtering**
   - MAF, MAP, TPS, CTS, MAT, O2, VSS, battery and discrete inputs.
8. **Airflow, fuel, and injector operation**
   - Load, AFR, startup, AE, DFCO, closed loop, BLM and batch service.
9. **Idle-air control**
   - PID cadence, motor service, A/C compensation, reset and park.
10. **Emissions and accessory controls**
    - AIR, EGR, purge, fan and A/C.
11. **Transmission and torque-converter clutch**
    - Selector state, TCC qualification, output boundary and brake power path.
12. **Diagnostics and ALDL communication**
    - Fault qualification, flash codes, 160 baud, 8192 baud and Mode 4.
13. **Output staging and electrical interfaces**
    - Raw MPU/I/O writes, driver polarity, protected outputs and HAL contract.
14. **Startup, shutdown, and exceptional modes**
    - Boot validation, factory test, SWI/reset vectors and optional ROM.
15. **C port architecture and embedded migration**
    - C89 rules, exact-width/wrap behavior, modular include structure and HALs.
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
