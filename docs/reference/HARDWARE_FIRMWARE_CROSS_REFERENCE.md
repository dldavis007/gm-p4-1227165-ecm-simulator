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
| Reference/RPM/dwell/spark | Status `$3FFA` at `$CAC6`; period `$3FC0` at `$CB5A`; RPM `$CDE6-$CE41`; spark output `$D20C-$D301` | U9 window `$3FC0`, `$3FC8`, `$3FDC`, `$3FE4`, `$3FE6`, `$3FE8`, `$3FEC`, `$3FF6`, `$3FFC` | U12 `REF/CYL/IGN/EST/BYPASS -> INJREF/IGNREF/ESTLOOP`; U9 inputs `INJREF/IGNREF/ESTLOOP`, output `IGN` | `src/scheduler_serial.inc.h`, `src/ecm_core.inc.h` | spark-reference and fuel/spark regressions | F0 register behavior; F1 pin/net connections; F2/F3 stimuli; F4 U9/U12 transformations |
| MAF/load/fuel chain | Analog MAF `$F7AC-$F7B6`; load producer `$D769-$D7A0`; downstream fuel/spark/diagnostic consumers | MAF `$00ED/$00EF`, airflow `$00EA-$00EB`, reference `$0095-$0096`, load `$0063`, history `$0061/$0062` | U10 AN10 analog MAF; physical transfer remains HAL | `src/fuel_air.inc.h`, `src/segment_f.inc.h`, `src/injector_service.inc.h` | MAF, load-producer, startup-fuel, fuel/spark and scheduler regressions | F0 MAF/reference-derived load; F1 connection; F2/F3 transfer |
| Batch injection | Service gate `$F67B-$F768`; pulse write `$F9D2-$F9E4`; factory register exercise `$FE4F-$FE9F` | sampled `$00A0` bit 6; `$3FD0` synchronous PW; `$3FCE` EFI delay | U9 `INJS/INJA` -> U12 driver; U12 `INJOUT` -> Q1; U11 `INJ` -> `INJLIMP`; sense/limit loops | `src/injector_service.inc.h`, `src/ecm_core.inc.h`, `src/factory_test_execution.inc.h` | fuel/spark, startup-fuel and factory-test regressions | F0 command/service; F1 connections; F2/F3 plant; F4 phase, polarity and current control |
| IAC control and motor | 50-ms producer plus 6.25-ms service; `$D370-$D3DB` key-off homing | IAC target/position/phase RAM | `IACA`, `IACB`, `IACEN`, stepper mechanics | `src/ecm_core.inc.h`, `src/ac_control.inc.h`, `src/ignition_shutdown.inc.h` | IAC/A-C, closed-loop/IAC and shutdown regressions | F0 software; F1 connection; F2/F3 motor/engine plant |
| Segment 1 output staging | `$EDA3-$EF03` | `$3FCC`, `$3FD2`, `$3FD4`, `$3FD6`, `$3FD8`, `$4004` | Output drivers and physical polarity | `src/output_handlers.inc.h`, `src/major_loop.inc.h` | `tests/output_stage_regression.inc.h` | F0 raw writes; F1 where schematic-proven; F3/F4 polarity/load |
| Vehicle speed | Major Segment 2 `$E07F` | VSS/status RAM | `VSS` conditioned input | `src/major_loop.inc.h`, `src/scheduler_serial.inc.h` | Scheduler and transmission regressions | F0 software; F1 connection; F2/F3 pulse calibration |
| TCC and transmission | Segment E and output staging | Selector/TCC state | External brake series-power boundary and TCC driver | `src/ecm_core.inc.h`, `simulation/transmission_drive.inc.h` | `tests/tcc_transmission_regression.inc.h` | F0 ECM logic; F1 connection; F3 plant ratios/slip |
| Coolant processing | Segment 6 and listing-resolved exits | Coolant variables and tables | CTS analog conditioning | `src/coolant_control.inc.h` | Coolant and listing-coolant regressions | F0 software; F1 connection; F2/F3/F4 analog transfer |
| U10 A/D acquisition | Common routine `$F1BE-$F1DF`; normal selectors `$10/$20/$40/$50/$60/$70/$80/$A0`; factory sweep `$FDB5-$FDC5` | temporary `$0064`; channel-specific RAM; factory `$017B-$0186` | U10 AN0-AN10 = MAP2/VOLT/O2/MAP/CTS/TPS/PUMPVOLT/DIAG/MAT/ESC/VMAF | HAL `hw_adc(selector)` plus subsystem consumers and factory test | sensor, diagnostic and factory-test regressions | F0 selector/storage behavior; F1 channel nets; F2 raw samples; F3 transfer curves; `$B0` F4 |
| ESC, knock attack/recovery and Error 43 | Knock attack `$D0D1-$D157`; recovery/failure `$EB3A-$EBB1`; Error 43 `$E6DF-$E6EC`; factory AN9 sample `$FDB5-$FDC5` | U9 counter `$3FCA`, prior `$00A1/$00A3`, retard `$00A5`, fault state; factory AN9 `$0184` | CAL29 is analog ESC/U10-AN9; separately CAL32 and KNOCK# reach U12 pin 15, whose pin-14 KNOCK output reaches U9 pin 42 | `src/ecm_core.inc.h`, `src/diagnostic_qualification.inc.h`, `src/factory_test_execution.inc.h` | fuel/spark, diagnostic, factory-test and frozen-profile regressions | F0 software; F1 separate paths; F2 counter stimulus; pin-42-to-`$3FCA` strong inference; U12/U9 conversion F4 |
| Diagnostics qualification | Segment D `$EF04`; `$E4F7-$E75C` | Diagnostic flags, timers and history | Diagnostic inputs and lamp output | `src/diagnostics.inc.h`, `src/diagnostic_qualification.inc.h`, `src/diagnostic_flash.inc.h` | Diagnostic regressions | F0 translated blocks; hardware inputs/outputs F1-F4 by signal |
| ALDL 160-baud transmit | IRQ/serial path | Serial state and transmit data | ALDL transceiver, shared line | `src/scheduler_serial.inc.h` | Existing scheduler/serial coverage | Partial F0; F1 connection; F3/F4 electrical timing/transceiver |
| ALDL 8192-baud protocol | IRQ front near `$C9F4`, `$FA58..$FC71`, lifecycle `$CB5F..$CBD9` | SCI RAM, device `$80`, Modes 0–4 | SCI registers and ALDL transceiver | `src/sci_8192.inc.h`, `src/mode4_lifecycle.inc.h` | Step-115/116 regressions | F0 software protocol/lifecycle; F1 connection; F3/F4 physical SCI/line |
| Ignition shutdown | `$D6D1-$D769`, `LF447` | Key-off timers, BLM cells, IAC reset state | Keep-alive power and SWI endpoint | `src/ignition_shutdown.inc.h` | `tests/ignition_shutdown_regression.inc.h` | F0 software; F4 physical powerdown/reset consequences |
| Factory test | `$C8B2-$C8DF`, `$CA6F-$CA73`, `$FC72-$FEA3` | `$0031/$0032/$0047-$004A`, `$0172-$018C`, MPU and I/O windows | FMD/SPI, A/D inputs, COP, output drivers and SWI/reset | `src/factory_test_control.inc.h`, `src/factory_test_execution.inc.h` | Step-117/118 regressions | F0 emitted software/raw writes; F1/F4 electrical/device semantics |
| Vector and exceptional entry | `$FFF0-$FFFE`; targets `$6000/$C9F4/$F27B/$C800` | Eight emitted vector words | Processor vector assignment, reset/COP/clock consequences, external `$6000` code | `src/vector_boundaries.inc.h` | `tests/vector_boundaries_regression.inc.h` | F0 table/routes; F4 processor/external-code consequences |
| MEMCAL cylinder selection | `$F682-$F68B` compares `$002F & $18` with `LC225`; `$E6A1-$E6AA` latches Error 41; `$CDE6-$CDF5` separately uses `LC009` for RPM normalization | CAL56 network; PROM `LC009=$00`, `LC225=$00`; MPU-derived `$002F` | `CAL56 -> U12 pin 11 CYL`, separate from U12 pin 29 `REF`; U12/MPU encoding undocumented | Existing listing-backed cylinder/RPM behavior only; do not synthesize U12 or equate CAL56 with either PROM byte | Scheduler, startup-fuel, injector and diagnostic regressions | F0 comparisons; F1 connection; strong end-to-end inference; F4 encoding |
| MEMCAL oscillator configuration | No direct processor read identified; no established relation to U9 timing registers | CAL42-CAL34 91 kOhm external-terminal relationship | `CAL42 -> U11 pin 18 OSC`; CAL34 destination unresolved | Implicit custom-device boundary; do not use as the U9/processor timebase | Steps 129, 130 and 134 audits | F1 connection; F4 transfer unknown |
| MEMCAL MAP/VIGN configuration | No normal-operation A/D channel 0/3 request found; factory scan `$FDB5-$FDC5` observes all channels | CAL61 common with J4 64; CAL59-to-CAL55/57 24 kOhm | MAP#/MAP2# -> U10 AN3/AN0; `CAL61 + MAP -> U11 pin 28`; `CAL59 -> 100 Ohm -> VIGN` | Keep MAP channels and U11 configuration separate from `$0063` load | Steps 129-132 audits; factory-test regression | F1 connections; F0 absence/scan evidence; F4 U11 consequence |
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
