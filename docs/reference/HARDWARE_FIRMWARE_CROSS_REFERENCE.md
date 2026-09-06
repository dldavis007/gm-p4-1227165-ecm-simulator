# Hardware/firmware cross-reference

This is a living index, not a replacement for instruction-level audit notes.
Addresses and literal behavior take priority over the convenient functional
names used here.

| Function or signal | Assembly/listing evidence | RAM, register, or calibration | Hardware/net boundary | C implementation | Regression/evidence | Status |
| --- | --- | --- | --- | --- | --- | --- |
| IRQ/minor scheduler | IRQ front, `LD6D1`, `LCD05`, `LCCAC` | `L0000` low nibble selects major segment | Ordinary IRQ source | `src/scheduler_serial.inc.h` | `tests/scheduler_regression.inc.h` | Confirmed |
| Major-segment dispatch | `LCCAC`, sixteen-entry table | `L0000 & $0F` | None beyond IRQ | `src/major_loop.inc.h` | `tests/major_loop_regression.inc.h` | Confirmed |
| Slow O2 filter | Even path beginning near `LCD05`; `LF250` lag helper | Input `L00D2`, output `L0071`, `LC565`, table `LC58B` | O2 analog input conditioning | `src/scheduler_serial.inc.h`, `src/ecm_core.inc.h` | `tests/scheduler_regression.inc.h` | Confirmed behavior; analog transfer is boundary |
| Reference/RPM/dwell/spark | Even minor branch | MPU reference/counter state and spark RAM/calibration | `IGNREF`, `EST`, custom MPU event timing | `src/scheduler_serial.inc.h`, `src/ecm_core.inc.h` | `tests/spark_reference_regression.inc.h`, `tests/fuel_spark_regression.inc.h` | Confirmed software; electrical waveform is boundary |
| MAF/load/fuel chain | Odd Air/Fuel branch | MAF, load, transient, AFR and injector state | `FMAF`, analog MAF, injector hardware | `src/fuel_air.inc.h`, `src/segment_f.inc.h`, `src/injector_service.inc.h` | MAF, startup-fuel, fuel/spark and scheduler regressions | Confirmed normal software path |
| Batch injection | Reference-driven injector service | `LC014=$B4`; injector pulse/bookkeeping RAM | `INJ`, `INJA`, injector current driver | `src/injector_service.inc.h` | Fuel/spark and startup-fuel regressions | Confirmed software cadence; driver waveform is boundary |
| IAC control and motor | 50-ms producer plus 6.25-ms service; `$D370-$D3DB` key-off homing | IAC target/position/phase RAM | `IACA`, `IACB`, `IACEN`, stepper mechanics | `src/ecm_core.inc.h`, `src/ac_control.inc.h`, `src/ignition_shutdown.inc.h` | IAC/A-C, closed-loop/IAC and shutdown regressions | Confirmed software; motor plant is simulated |
| Segment 1 output staging | `$EDA3-$EF03` | `$3FCC`, `$3FD2`, `$3FD4`, `$3FD6`, `$3FD8`, `$4004` | Output drivers and physical polarity | `src/output_handlers.inc.h`, `src/major_loop.inc.h` | `tests/output_stage_regression.inc.h` | RAW-HAL/confirmed writes |
| Vehicle speed | Major Segment 2 `$E07F` | VSS/status RAM | `VSS` conditioned input | `src/major_loop.inc.h`, `src/scheduler_serial.inc.h` | Scheduler and transmission regressions | Confirmed software; transfer constant needs hardware evidence |
| TCC and transmission | Segment E and output staging | Selector/TCC state | External brake series-power boundary and TCC driver | `src/ecm_core.inc.h`, `simulation/transmission_drive.inc.h` | `tests/tcc_transmission_regression.inc.h` | Confirmed ECM logic; plant ratios are simulator parameters |
| Coolant processing | Segment 6 and listing-resolved exits | Coolant variables and tables | CTS analog conditioning | `src/coolant_control.inc.h` | Coolant and listing-coolant regressions | Confirmed software; sensor transfer boundary |
| MAT and knock recovery | Major Segment A | MAT and knock RAM/calibration | `MAT`, `KNOCK`, ESC/CAL29/CAL32 networks | `src/mat_control.inc.h` | MAT and spark regressions | Confirmed translated behavior; custom-device analog details unresolved |
| Diagnostics qualification | Segment D `$EF04`; `$E4F7-$E75C` | Diagnostic flags, timers and history | Diagnostic inputs and lamp output | `src/diagnostics.inc.h`, `src/diagnostic_qualification.inc.h`, `src/diagnostic_flash.inc.h` | Diagnostic regressions | Confirmed translated blocks |
| ALDL 160-baud transmit | IRQ/serial path | Serial state and transmit data | ALDL transceiver, shared line | `src/scheduler_serial.inc.h` | Existing scheduler/serial coverage | Partial; electrical timing reference available |
| ALDL 8192-baud protocol | IRQ front near `$C9F4`, `$FA58..$FC71`, lifecycle `$CB5F..$CBD9` | SCI RAM, device `$80`, Modes 0–4 | SCI registers and ALDL transceiver | `src/sci_8192.inc.h`, `src/mode4_lifecycle.inc.h` | Step-115/116 regressions | Confirmed software protocol and Mode-4 lifecycle; physical byte timing and unrepresented ROM reads remain HAL boundaries |
| Ignition shutdown | `$D6D1-$D769`, `LF447` | Key-off timers, BLM cells, IAC reset state | Keep-alive power and SWI endpoint | `src/ignition_shutdown.inc.h` | `tests/ignition_shutdown_regression.inc.h` | Confirmed software; physical powerdown is HAL |
| MEMCAL cylinder selection | ECM schematic `CAL56 -> CYL` | Fixed MEMCAL network configuration | Custom U12 input | Represent as target hardware profile if processor-visible behavior requires it | Schematic connection only | Connection confirmed; consequence provisional |
| MEMCAL oscillator configuration | ECM schematic `CAL42 -> U11 OSC` | Fixed MEMCAL network configuration | U11 analog/timing network | Normally implicit at HAL signal boundary | Schematic connection only | Connection confirmed; transfer unknown |
| Optional HUD ROM | Major Segment 4 and optional calls | `$5800` external window | Separate optional ROM | Explicit boundary/no invented implementation | Whole-image audit | Boundary |

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
