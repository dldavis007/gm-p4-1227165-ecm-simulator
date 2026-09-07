# Hardware/firmware cross-reference

This is a living index, not a replacement for instruction-level audit notes.
Addresses and literal behavior take priority over the convenient functional
names used here.

Step 127 adds an explicit fidelity vocabulary for hardware-facing work:

- **F0** - listing-backed firmware-exact state, ordering, branch, table, or raw write.
- **F1** - hardware-connected by schematic, connector, MEMCAL, or other direct physical evidence.
- **F2** - processor-visible HAL signal supplied/observed by the PC harness.
- **F3** - plant, sensor-transfer, polarity, timing, vehicle, or electrical assumption used for simulation.
- **F4** - external/unknown custom-device, transceiver, analog, power, optional-ROM, or other hardware boundary.

A single interface may legitimately span several classes; for example, the
software write can be F0 while its external electrical consequence remains F4.

| Function or signal | Assembly/listing evidence | RAM, register, or calibration | Hardware/net boundary | C implementation | Regression/evidence | Status / fidelity |
| --- | --- | --- | --- | --- | --- | --- |
| IRQ/minor scheduler | IRQ front, `LD6D1`, `LCD05`, `LCCAC` | `L0000` low nibble selects major segment | Ordinary IRQ source | `src/scheduler_serial.inc.h` | `tests/scheduler_regression.inc.h` | Confirmed; F0 software, F2/F4 interrupt source |
| Major-segment dispatch | `LCCAC`, sixteen-entry table | `L0000 & $0F` | None beyond IRQ | `src/major_loop.inc.h` | `tests/major_loop_regression.inc.h` | Confirmed; F0 |
| Slow O2 filter | Even path beginning near `LCD05`; `LF250` lag helper | Input `L00D2`, output `L0071`, `LC565`, table `LC58B` | O2 analog input conditioning | `src/scheduler_serial.inc.h`, `src/ecm_core.inc.h` | `tests/scheduler_regression.inc.h` | F0 behavior; F1 connection; F2/F3 analog stimulus/transfer |
| Reference/RPM/dwell/spark | Even minor branch | MPU reference/counter state and spark RAM/calibration | `IGNREF`, `EST`, custom MPU event timing | `src/scheduler_serial.inc.h`, `src/ecm_core.inc.h` | `tests/spark_reference_regression.inc.h`, `tests/fuel_spark_regression.inc.h` | F0 software; F1 connection; F2/F3/F4 waveform/custom MPU |
| MAF/load/fuel chain | Odd Air/Fuel branch | MAF, load, transient, AFR and injector state | `FMAF`, analog MAF, injector hardware | `src/fuel_air.inc.h`, `src/segment_f.inc.h`, `src/injector_service.inc.h` | MAF, startup-fuel, fuel/spark and scheduler regressions | F0 software; F1 connection; F2/F3 input transfer |
| Batch injection | Reference-driven injector service | `LC014=$B4`; injector pulse/bookkeeping RAM | `INJ`, `INJA`, injector current driver | `src/injector_service.inc.h` | Fuel/spark and startup-fuel regressions | F0 cadence/command; F1 connection; F2/F3/F4 driver/plant |
| IAC control and motor | 50-ms producer plus 6.25-ms service; `$D370-$D3DB` key-off homing | IAC target/position/phase RAM | `IACA`, `IACB`, `IACEN`, stepper mechanics | `src/ecm_core.inc.h`, `src/ac_control.inc.h`, `src/ignition_shutdown.inc.h` | IAC/A-C, closed-loop/IAC and shutdown regressions | F0 software; F1 connection; F2/F3 motor/engine plant |
| Segment 1 output staging | `$EDA3-$EF03` | `$3FCC`, `$3FD2`, `$3FD4`, `$3FD6`, `$3FD8`, `$4004` | Output drivers and physical polarity | `src/output_handlers.inc.h`, `src/major_loop.inc.h` | `tests/output_stage_regression.inc.h` | F0 raw writes; F1 where schematic-proven; F3/F4 polarity/load |
| Vehicle speed | Major Segment 2 `$E07F` | VSS/status RAM | `VSS` conditioned input | `src/major_loop.inc.h`, `src/scheduler_serial.inc.h` | Scheduler and transmission regressions | F0 software; F1 connection; F2/F3 pulse calibration |
| TCC and transmission | Segment E and output staging | Selector/TCC state | External brake series-power boundary and TCC driver | `src/ecm_core.inc.h`, `simulation/transmission_drive.inc.h` | `tests/tcc_transmission_regression.inc.h` | F0 ECM logic; F1 connection; F3 plant ratios/slip |
| Coolant processing | Segment 6 and listing-resolved exits | Coolant variables and tables | CTS analog conditioning | `src/coolant_control.inc.h` | Coolant and listing-coolant regressions | F0 software; F1 connection; F2/F3/F4 analog transfer |
| MAT and knock recovery | Major Segment A | MAT and knock RAM/calibration | `MAT`, `KNOCK`, ESC/CAL29/CAL32 networks | `src/mat_control.inc.h` | MAT and spark regressions | F0 software; F1 connection; F2 MAT signal; F4 knock/custom analog |
| Diagnostics qualification | Segment D `$EF04`; `$E4F7-$E75C` | Diagnostic flags, timers and history | Diagnostic inputs and lamp output | `src/diagnostics.inc.h`, `src/diagnostic_qualification.inc.h`, `src/diagnostic_flash.inc.h` | Diagnostic regressions | F0 translated blocks; hardware inputs/outputs F1-F4 by signal |
| ALDL 160-baud transmit | IRQ/serial path | Serial state and transmit data | ALDL transceiver, shared line | `src/scheduler_serial.inc.h` | Existing scheduler/serial coverage | Partial F0; F1 connection; F3/F4 electrical timing/transceiver |
| ALDL 8192-baud protocol | IRQ front near `$C9F4`, `$FA58..$FC71`, lifecycle `$CB5F..$CBD9` | SCI RAM, device `$80`, Modes 0–4 | SCI registers and ALDL transceiver | `src/sci_8192.inc.h`, `src/mode4_lifecycle.inc.h` | Step-115/116 regressions | F0 software protocol/lifecycle; F1 connection; F3/F4 physical SCI/line |
| Ignition shutdown | `$D6D1-$D769`, `LF447` | Key-off timers, BLM cells, IAC reset state | Keep-alive power and SWI endpoint | `src/ignition_shutdown.inc.h` | `tests/ignition_shutdown_regression.inc.h` | F0 software; F4 physical powerdown/reset consequences |
| Factory test | `$C8B2-$C8DF`, `$CA6F-$CA73`, `$FC72-$FEA3` | `$0031/$0032/$0047-$004A`, `$0172-$018C`, MPU and I/O windows | FMD/SPI, A/D inputs, COP, output drivers and SWI/reset | `src/factory_test_control.inc.h`, `src/factory_test_execution.inc.h` | Step-117/118 regressions | F0 emitted software/raw writes; F1/F4 electrical/device semantics |
| Vector and exceptional entry | `$FFF0-$FFFE`; targets `$6000/$C9F4/$F27B/$C800` | Eight emitted vector words | Processor vector assignment, reset/COP/clock consequences, external `$6000` code | `src/vector_boundaries.inc.h` | `tests/vector_boundaries_regression.inc.h` | F0 table/routes; F4 processor/external-code consequences |
| MEMCAL cylinder selection | ECM schematic `CAL56 -> CYL` | Fixed MEMCAL network configuration | Custom U12 input | Represent as target hardware profile if processor-visible behavior requires it | Schematic connection only | F1 connection; F4 custom-device consequence |
| MEMCAL oscillator configuration | ECM schematic `CAL42 -> U11 OSC` | Fixed MEMCAL network configuration | U11 analog/timing network | Normally implicit at HAL signal boundary | Schematic connection only | F1 connection; F4 transfer unknown |
| Optional HUD ROM | Major Segment 4 and optional calls | `$5800` external window | Separate optional ROM | Explicit boundary/no invented implementation | Whole-image audit | F4 boundary |

## Step-127 implementation guidance

The next HAL implementation should preserve this ordering of responsibility:

1. Keep translated firmware and raw register semantics in F0 code.
2. Expose processor-visible inputs and raw outputs through explicit F2 setters/observers.
3. Put engineering-unit conversion, sensor curves, vehicle dynamics, electrical polarity and timing in a separate optional F3 plant/HAL layer.
4. Leave F4 behavior unimplemented unless new evidence supports it.

Priority candidates for explicit F2 input setters are MAF, TPS, CTS, MAT, O2,
battery, knock-result, VSS/reference pulse sources, and discrete hardware inputs.
Priority output observers are raw injector, IAC, TCC, AIR, EGR, purge, fan,
CCP and serial/register state. Any external active-high/active-low interpretation
must remain separate until directly supported by the 1227165 hardware path.

See `docs/STEP127_HARDWARE_HAL_FIDELITY_AUDIT.txt` for the complete classification
and prioritized backlog.

## Cross-reference rules

- Add exact address ranges rather than only semantic names whenever known.
- A C helper is not considered live until its scheduler or call-chain entry is
  documented and regression-tested.
- Simulator plant constants are identified as assumptions and never promoted
  to calibration facts.
- Raw hardware-register writes can be complete even when the electrical device
  behavior remains a HAL boundary.
- MEMCAL resistance values do not by themselves prove the meaning of a CAL
  connection; schematic destination and executable consequences are separate
  evidence steps.
- F2 host interfaces must not silently incorporate F3 engineering-unit or plant
  assumptions.
- F4 behavior must remain explicit rather than being filled with convenient
  inferred semantics.
