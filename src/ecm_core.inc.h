/*
 * BUA / ECM 1227165 - PC harness, current integration step 110
 * C89-compatible.
 *
 * Step 110 completes listing-exact Major Segment-1 raw output staging at
 * $EDA3..$EF03 and adds a whole-image executable-gap audit. Frozen Step-104,
 * Step-105, and Step-109 signatures use explicit PC-only replay switches.
 *
 * Modular OnlineGDB-compatible build: main.c includes the logically grouped
 * source, simulation, and regression fragments that form this one translation
 * unit. Step 104 freezes the first deterministic
 * normal-operation baseline with a canonical per-IRQ RAM/MPU/I/O/statistics
 * signature and an explicit completeness boundary audit. Step 103 connects
 * the exact LDA73..LDB35
 * 4x4 BLM cell selector, its hysteresis/reinitialization side effects, the
 * unconditional O2-ready front end, and Segment-5 BLM-store qualification.
 * The integrated warm-restart drive now enters closed loop and learns a live
 * selected cell. Step 102 scheduler-connects the source-ordered
 * LE436 asynchronous acceleration-enrichment dispatch after normal LF901,
 * including the raw L3FF2 write and L3FFC CSR trigger at the HAL boundary.
 * The cranking path retains its literal LF92A-to-LDDC2 bypass of LE436.
 * Step 101 scheduler-connects the calibrated LD7B1 cranking-fuel branch and
 * its immediate LD7A0 handoff to LD86B.
 * Step 100 connects the remaining normal
 * Segment-3 and Segment-7 producers and restores LF3B6's leading H.U. hook.
 * Step 99 adds the first coordinated
 * start-to-drive behavioral simulation. Steps 93-98 establish the
 * source-shaped even/odd scheduler, MAF/load/TPS/transient/fuel/DFCO path,
 * 50-ms tail, and LF67B injector-service bookkeeping. Step 99 adds the
 * common LCC21 TPS producer and drives the resulting core through catch,
 * idle, acceleration, cruise, lift/DFCO, deceleration, and returned idle.
 * Step 91 closed the exact Segment-5 A/C/IAC
 * continuation LE005..LE07B and the
 * normal 9340 Segment-1 software output staging is represented explicitly
 * at the HAL boundary (AIR/AE/TCC/CCP/fan/EGR) without inventing electrical
 * polarity. Step 110 later completes its diagnostic and Mode-4 raw writes.
 * Step 90 major-loop producer wiring and all prior regressions remain intact.
 * The freeze audit is supplied separately as STEP104_BASELINE_FREEZE.txt.
 */
/*
 * BUA / ECM 1227165 - OnlineGDB PC harness, step 73
 * C89-compatible.
 *
 * Adds in step 73:
 *   - translates the LE25E..LE285 TCC kickdown/force-upshift tail
 *   - preserves the calibrated 3200-RPM and >60-MPH forced-lock decisions
 *   - preserves the exact <=60-MPH unlock boundary and zero lock-delay calibration
 *   - verifies that the final candidate sign bit, not a brake input, controls the software TCC status gate
 *
 * Adds in step 72:
 *   - translates the LE1FB..LE227 road-speed/TPS coast selector used by the TCC state machine
 *   - preserves the L0045 bit-4 32/37-MPH hysteresis and the LC28E OD-active TPS hysteresis
 *   - verifies the exact strict-less-than TPS qualification at the coast threshold
 *   - keeps this as executable behavior only; no new brake-switch software input is invented
 *
 * Adds in step 70:
 *   - separates the ECM TCC command from physical clutch energization at the vehicle wiring boundary
 *   - preserves the source fact that FMD byte 1 is complemented into L0037 while TCC bit 5 is retained
 *   - models the brake switch as an external series-power interruption, not an invented ECM software input
 *   - adds a PC-only locked-coast/brake-release RPM signature for the future integrated drive-cycle test
 *
 * Adds in step 69:
 *   - translates the dormant pass-by TCC timer/forced-lock path around LCF1A..LCF7F
 *   - proves LC29C=$FFFF makes the executable BHI delay-complete test unreachable
 *   - translates the OD-active N/V branch that feeds the ordinary lock/unlock state machine
 *   - keeps the unresolved brake-switch semantic explicit; no synthetic brake input is added
 *
 * Adds in step 68:
 *   - reconstructs the executable N/V-ratio gear-state classifier around LE122..LE17A
 *   - verifies the 30..35 fourth-gear N/V window and its LC28A confirmation counter
 *   - connects inferred fourth-gear state to the step-67 TCC lock path
 *   - reconstructs the final TCC-delay/downshift-inhibit gate at LE28F..LE2B1
 *
 * Adds in step 67:
 *   - begins the warm automatic 700R4 TCC lock/unlock reconstruction
 *   - translates the calibrated fourth-gear lock/unlock threshold tables
 *   - preserves coolant qualification, TCC status bit, and $D000/$DFFF MPU output boundary
 *   - deliberately leaves brake-switch semantics unresolved rather than inventing an ECM input
 *
 * Adds in step 64:
 *   - traces the power-steering-pressure anticipation path beside the A/C follower logic
 *   - proves that the 9340 calibration sets both power-steering anticipation additions to zero
 *   - distinguishes executable feature capability from the behavior actually enabled by this calibration
 *   - regression-compares baseline 9340 behavior with a hypothetical nonzero calibration without changing the algorithm
 *
 * Adds in step 63:
 *   - follows the complete A/C load cycle through anticipation, compressor load, unload, and anticipation removal
 *   - carries the source load-controlled/load-removal flags through the integrated 6.25-ms motor service
 *   - tracks every physical follower-removal step rather than only the first step of each 50-ms window
 *   - verifies that coordinated anticipation removal suppresses the unload flare before normal PID takeover
 *
 * Adds in step 62:
 *   - integrates the source-derived P/N A/C anticipation demand into the multirate idle simulation
 *   - compares identical PC compressor-load disturbances with and without A/C anticipation enabled
 *   - preserves 50-ms control scheduling and 6.25-ms physical IAC motor service
 *   - demonstrates reduced RPM sag from anticipatory IAC opening without changing the engine plant between cases
 *
 * Adds in step 61:
 *   - applies a temporary PC-plant idle load without changing reconstructed ECM control logic
 *   - exercises underspeed/high-response IAC behavior through the 50-ms regulator and 6.25-ms motor executor
 *   - demonstrates persistent L0101 multi-step consumption during a sufficiently large transient disturbance
 *   - verifies recovery into the calibrated P/N idle deadband after the disturbance is removed
 *
 * Adds in step 60:
 *   - connects the executable-proven 50-ms IAC regulator to the 6.25-ms LF4DF/LF5B0 motor service
 *   - preserves a pending packed L0101 request across the eight motor-service passes between regulator updates
 *   - uses the Step-40 motor executor for one-step-at-a-time position, phase, direction, and request bookkeeping
 *   - updates the PC engine plant at 6.25 ms while keeping regulator/RPM-rate state at the source 50-ms cadence
 *
 * Adds in step 59:
 *   - decodes LD633..LD6B6 fractional IAC quantization and L0103 residual feedback
 *   - uses LC678/LC679 low integral gains inside the RPM-error deadband and LC67A exact-zero clear
 *   - preserves the source rule that direct outputs clear L0103 while fractional outputs retain quantization residual
 *   - corrects the closed-loop PID cadence to executable-proven 50 ms (L0000 & 7 == 0)
 *
 * Adds in step 58:
 *   - integrates the LD472..LD6C6 IAC regulator state around the earlier PID translation
 *   - adds source-shaped RPM-rate, deadband, PID-disable, and fractional-step persistence
 *   - corrects the earlier Step-37 assumption that L0103 is a conventional always-active integral term
 *   - demonstrates that L0103 is cleared for |linear algorithm output| >= LC66C=8 and is used only in the small-output branch
 *
 * Adds in step 57:
 *   - closes the PC idle plant around the reconstructed Step-36 target and Step-37 IAC PID
 *   - lets commanded IAC motion influence simulated RPM instead of prescribing the post-catch RPM
 *   - keeps coolant warm-up and engine air-response dynamics explicitly on the simulator/plant side
 *   - corrects LF6D5 startup-enrichment gating to test only the high byte L000E, as the source does
 *
 * Adds in step 56:
 *   - drives the Step-55 reference/injector model from a changing crank/catch/idle RPM trajectory
 *   - converts instantaneous RPM to the MPU-style 65.536-kHz distributor-reference period
 *   - demonstrates sub-threshold cranking reset, ten-reference catch qualification, and high-idle warm-down
 *   - advances startup enrichment from physically timed batch services while RPM changes continuously
 *
 * Adds in step 55:
 *   - replaces synthetic injector-event stepping with a reference-driven physical batch scheduler
 *   - generates one normal 9340 double-fire batch service for every four 90-degree DRPs
 *   - feeds those physical services directly into the Step-49/48 startup-enrichment event path
 *   - verifies startup-delay and decay timing from DRPs/revolutions rather than manually injected events
 *
 * Adds in step 54:
 *   - resolves the 8-cylinder distributor-reference geometry from L3FC0 and the factory test
 *   - establishes L3FC0 as two DRP intervals = 180 crank degrees, hence one DRP every 90 degrees
 *   - cross-checks 1227165 hardware evidence for one common injector driver / all-8 batch firing
 *   - corrects the Step-51 double-fire timing estimator to one batch service per crank revolution
 *
 * Adds in step 53:
 *   - traces the L3FFC MPU control-word initialization and runtime bit manipulation
 *   - establishes normal startup 000A -> FB1A and ERR51 alternate 0002 -> FB12
 *   - identifies executable roles for low-byte bits 2, 3, and 4 where source usage proves them
 *   - deliberately leaves double-fire/bank/cadence selection MPU-internal because no control bit is proven
 *
 * Adds in step 52:
 *   - maps the production-facing MPU timer/fuel/spark registers used by the 6801
 *   - proves ordinary runtime writes synchronous fuel PW to L3FD0 but keeps EFI delay L3FCE at zero
 *   - separates explicit spark scheduling (L3FDC/L3FF6) from MPU-internal synchronous-fuel scheduling
 *   - preserves the factory-test nonzero EFI-delay case as evidence that L3FCE really is a delay register
 *
 * Adds in step 51:
 *   - traces injector-service qualification to MPU status L3FFA high-byte bit 6
 *   - separates the MPU injector-service indication from the independent DRP/ref bit 3
 *   - preserves the 6.25-ms polling ceiling: at most one software injector service per IRQ
 *   - adds a clearly marked double-fire timing estimator without treating it as executable proof
 *
 * Adds in step 50:
 *   - translates the single-fire event toggle and alternating zero-PW behavior
 *   - connects single-fire to the doubled injector-flow calibration used by base-PW math
 *   - confirms the actual 9340 LC014=$B4 calibration leaves single-fire selection disabled
 *   - corrects Step-41 LF521: LC630=32 is an unconditional startup-position addition
 *
 * Adds in step 49:
 *   - integrates cranking fuel, reference qualification, and immediate crank-to-run handoff
 *   - carries the Step-48 startup AFR accumulator into the Step-31 MAF/base-PW running path
 *   - advances startup enrichment on injector events through initial delay and periodic decay
 *   - demonstrates convergence from enriched post-catch fueling toward steady-state running fuel
 *
 * Adds in step 48:
 *   - translates startup-enrichment initialization and injector-event decay
 *   - preserves the 16-bit L000E accumulator, L0010 initial delay, and L00CC repeat counter
 *   - separates decaying startup enrichment from the continuously refreshed L00CD coolant AFR term
 *
 * Adds in step 47:
 *   - integrates AFR source selection in executable order: Mode 4 -> PE -> normal LDA2D
 *   - preserves PE TPS/load hysteresis, PE-state clearing/re-setting, and single-fire clearing
 *   - translates Mode-4 reciprocal AFR override and confirms DFCO fuel-zeroing is downstream
 *
 * Adds in step 46:
 *   - connects the DFCO state flag to LF92A zero-fuel output behavior
 *   - translates the one-pulse LC605 stall-saver async fuel addition
 *   - translates the LC603/LC604 post-DFCO follower support and LC60A DFCO follower offset
 *
 * Adds in step 45:
 *   - translates LD86B..LD96D decel-enlean and DFCO qualification/state behavior
 *   - preserves 1200/900-RPM and 30/40-load hysteresis plus the 20-count entry timer
 *   - models the LC608 re-entry timer and the executable TPS path that can explicitly clear it
 *
 * Adds in step 44:
 *   - translates the crank-to-run qualification and fuel-path handoff
 *   - preserves the second-reference latch plus LC01A=8 startup counter behavior
 *   - translates the L0118 cold-start reference counter and its 32-second/TPS reset gates
 *   - proves that LC35A=0 makes ENGINE RUNNING select the normal fuel path immediately
 *
 * Adds in step 43:
 *   - translates the calibrated LD7B1..LD7F0 cranking pulse-width construction
 *   - preserves coolant lookup, DRP multiplier, and TPS multiplier/un-flood behavior
 *   - documents that LC35B is all zero in the 9340 image, disabling the later RPM blend path
 *   - keeps the handoff to LF92A separate from the already-translated ordinary fuel output
 *
 * Adds in step 42:
 *   - translates the LD6D1 ignition-off timer/housekeeping path
 *   - models LEA80 re-arming of the next IAC reset while the engine is running
 *   - connects key-off to the LD393 IAC homing state machine
 *   - preserves the 8-count housekeeping threshold and LC012=$0385 software-powerdown threshold
 *
 * Adds in step 41:
 *   - translates the LD37x..LD3D2 IAC motor-reset/homing sequence
 *   - preserves the 255-step hard-stop seek followed by reopening to LC62F=144
 *   - translates LF521..LF53A startup warm-park positioning and battery inhibit
 *   - connects the coolant-derived warm-park position L010C to the startup command
 *
 * Adds in step 40:
 *   - translates LF5DF..LF644 IAC motor-command execution and position bookkeeping
 *   - preserves packed direction/magnitude command semantics and the 0,1,3,2 phase table
 *   - updates present position, running counter, throttle-follower accumulator, and fan-progress counter
 *   - translates LF644..LF678 base-bias error construction with signed +/-127 saturation
 *
 * Adds in step 39:
 *   - translates the LE8E3..LE9C5 throttle-follower command construction
 *   - preserves the 1.5-step/TPS slope, 75-step cap, P/N scaling, and moving-drive +5 steps
 *   - models A/C anticipation/default validation and immediate load-removal flagging
 *   - translates the LF573..LF5AD trajectory-decay timing arithmetic
 *   - isolates the fan-anticipation walk and its LC676 PID-inhibit completion
 *
 * Adds in step 38:
 *   - translates LD4DD..LD53B Neutral/Drive transition delay and minimum-IAC learning
 *   - preserves the 320-ms Neutral->Drive PID inhibit and deadband-timer reset
 *   - translates the drive-only learned minimum position walk at the selected 100-ms phase
 *   - isolates the executable high-derivative branch enabled by LC656/LC657
 *
 * Adds in step 37:
 *   - translates the normal low-gain IAC PID feedback path at LD59A..LD6AA
 *   - models P/N versus Drive deadbands and integral gains
 *   - preserves proportional/derivative sign, integral accumulation, and +/-127 clamp
 *   - translates the C66B/C66C output quantizer and C686 coolant multiplier
 *   - deliberately leaves rare high-gain/transition branches isolated for the next step
 *
 * Adds in step 36:
 *   - translates LD41F..LD450 IAC commanded-idle-speed construction
 *   - uses the C637 coolant target-RPM table with exact 16-count interpolation
 *   - applies the +50 RPM Park/Neutral offset and optional +300 RPM hot-spark offset
 *   - shows that A/C/fan calibrations in this image are step-position anticipation, not RPM-target additions here
 *   - establishes the normal warm P/N target from this code path without yet translating the full IAC PID
 *
 * Adds in step 35:
 *   - translates LED7B..LED9D BLM-store qualification
 *   - translates LDDC2..LDE30 50-ms BLM learning/update path
 *   - preserves integrator deadband, rich/lean agreement, +/-2 update, and 108..160 clamps
 *   - keeps LC5E0=28 as literal loop-count threshold without asserting the source comment timing
 *
 * Adds in step 34:
 *   - translates LECF6..LED7B closed-loop entry qualification
 *   - selects cold/warm/hot engine-run timers from coolant at L005F
 *   - translates minimum-coolant, O2-ready, 10-second timeout, and mode gates
 *   - uses the Step-33 listing evidence to mark CMPA #$80 (128) verified
 *
 * Adds in step 33:
 *   - investigates the anomalous source instruction `CMPA #1280` at LDBEE
 *   - adopts 128 as the strongly supported intended decel-integrator threshold
 *   - preserves the source-text anomaly; listing bytes verify CMPA #$80 (128)
 *   - completes the LDBE8..LDBFA decel closed-loop integrator-reset behavior
 *
 * Adds in step 32:
 *   - translates the unambiguous LDB9B..LDBD4 closed-loop reset/qualification gates
 *   - preserves resets for open loop, PE/DFCO, BLM-cell change, low-PW, and AE state
 *   - proves LC016=$62 enables BLM-cell and AE-related integrator-reset options
 *   - proves LC017=$00 disables the optional CCP-change integrator reset in this image
 *   - isolates the source's anomalous 8-bit `CMPA #1280` decel branch instead of guessing
 *
 * Adds in step 31:
 *   - connects analog MAF -> L00EA -> AFR -> calculated base PW -> BLM/C-L
 *   - continues through ordinary LF92A injector-output conditioning
 *   - adds deterministic sensor-to-injector regressions without scheduler claims
 *   - removes the synthetic-airflow dependency from the integrated fuel test
 *
 * Adds in step 30:
 *   - translates LF76A..LF87F MAF input conversion into L00EA airflow
 *   - proves LC014=$B4 selects the analog-MAF branch and airflow filtering
 *   - translates the six-piece MAF transfer curve into Q8.8 grams/second
 *   - translates the 4-sample/sliding airflow filter and min/max airflow limits
 *   - retains the ERR33/34 default-airflow fallback calculation
 *
 * Added in step 29:
 *   - translates the injector-event state machine at LF71C..LF768
 *   - preserves the single-fire zero-PW flip-flop and L003D b0 alternation
 *   - latches the last injector base PW and accumulates pending delivered fuel
 *   - advances/saturates the running injector-event counter
 *   - preserves the two-inject transient-fuel limiter lifetime and its b7 hold gate
 *
 * Added in step 28:
 *   - translates LFA1E TPS-triggered asynchronous accel-enrichment qualification
 *   - translates LE436..LE4EE async-pulse generation and coolant scaling
 *   - applies Step-27 transient-fuel headroom limiting to async pulses
 *   - preserves min/max async PW and twice the battery injector offset
 *
 * Added in step 27:
 *   - translates LF95E/LF9E5 transient-fuel limiting
 *   - unpacks LC331 high/low nibble factors for trigger/after-inject phases
 *   - translates the fixed LC5F8/LC5F9 alternate limiter entry at $F9FC
 *
 * Added in step 26:
 *   - translates LF901 differential-load accel enrichment application/decay
 *   - translates the ordinary LF92A..LF9E4 fuel-output path
 *   - preserves VATS/ignition/DFCO/high-RPM fuel cut gates
 *   - applies low-PW linearization plus twice the battery injector offset
 *   - preserves the minimum/default PW and 0x7FFF hardware-output cap
 *
 * Adds in step 24:
 *   - translates LDB35..LDD27 active closed-loop O2 control
 *   - translates fast O2 rich/lean hysteresis and slow-O2 error generation
 *   - translates airflow/idle error reduction, proportional width/step tables
 *   - translates integrator delay and 40..180 short-term integrator limits
 *   - connects the resulting L00C6 correction conceptually to Step 23
 *
 * Added in step 23:
 *   - translates LDD7A..LDDB9 BLM and closed-loop pulse-width correction
 *   - adds literal LF472 factor-in behavior (8x16, unity at factor 128)
 *   - preserves L00C6 additive correction, proportional half-step, and
 *     the L003D-b0 alternating round-up behavior
 *   - adds an integrated base-PW -> BLM -> closed-loop L00C2 regression
 *
 * Added in step 22:
 *   - begins the fuel/air core at LDD2D with base injector pulse-width generation
 *   - proves LC016=$62 selects the calculated-BPW path, not the C42D table path
 *   - adds an instruction-equivalent LF2F0 16x16 fixed-point helper
 *   - translates the running calculated-BPW chain using airflow, ref period, AFR,
 *     and the double/single-fire injector-flow constants
 *   - retains the C42D/LF27C table path as the disabled alternate calibration path
 *
 * Adds in step 20:
 *   - translates LD1F6..LD25x spark-angle to timer-count conversion
 *   - proves the saved reference period is the LF266 multiplicand at LD1F6
 *   - preserves LC01D fixed spark-time correction
 *   - translates the ref-period/16 advance slew limit and +/-period bound
 *   - updates the MPU spark fall/dwell delta words and current fire offset
 *   - adds eight regression cases for timing conversion and limiting
 *
 * Retained from step 19:
 *   - translates LD192..LD1EE Mode-4 spark modification and spark blending
 *   - preserves the unusual literal ADCA #0111 Mode-4 add sequence
 *   - models LF266 as rounded (8-bit * 16-bit) / 256 fixed-point multiply
 *   - corrects Step-16 startup-spark delay scaling using the literal LF266 result
 *   - records advance/retard in L0034 b0 and returns the low-byte magnitude
 *   - adds eight regression cases for blend, diagnostics, and Mode-4 branches
 *
 * Retained from step 18:
 *   - translates the BUA knock-control qualification and attack path
 *     around LD0D1..LD157
 *   - translates the C1BE RPM-dependent knock attack-rate table
 *   - translates the C1C8 WOT maximum-retard table
 *   - translates Segment A LEB3A..LEB59 knock-retard recovery
 *   - hooks the recovery into the actual major-loop Segment A cadence
 *   - adds isolated regression tests for qualification, attack, limiting,
 *     forced-failure retard, and recovery
 *
 * Retained from step 16:
 *   - startup spark initialization and injector-event decay state machine
 *
 * Retained from step 15:
 *   - translates LCFF4..LD02B highway-mode qualification/timer behavior
 *     for the actual BUA LC017=$00 calibration
 *   - translates the C172 highway spark table and LF2B9 interpolation
 *   - translates LD04D/LD054 hot-restart retard qualification/expiry
 *   - adds isolated regression tests for both spark contributions
 *
 * Retained from step 14:
 *   - original C02E main spark calibration and LF27C interpolation
 *   - C0FF coolant spark correction table and 20-degree bias handling
 *   - main + coolant partial spark calculations and regressions
 *
 * Retained from step 12:
 *   - translates the normal-running LCEB3..LCF1A dwell calculation
 *   - preserves load-change and reference-period dynamic-dwell behavior
 *   - adds battery-voltage dwell compensation and reference-period limiting
 *   - corrects the even-loop RPM path so it recalculates while running even
 *     when no new reference pulse arrived during that particular 12.5-ms loop
 *
 * Retained from step 11:
 *   - translates BUA reference-pulse qualification / engine-running startup state
 *   - preserves the second-reference-valid latch and 8-loop startup qualification
 *   - models the 23-loop missing-reference wait point without inventing SWI recovery
 *   - translates the exact L0056 special RPM transform
 *   - independent phase-continuous ECM distributor-reference pulse source
 *   - emulates the P4 65.536-kHz reference-period measurement for 8 cylinders
 *   - translates the LCDE6/LCDF6..LCE41 RPM calculation path
 *   - ECM RPM/25 and filtered RPM/12.5 now come from simulated reference pulses
 *   - drive-cycle compares commanded, ECM-derived, and cluster tach RPM
 *
 * Retained from earlier steps:
 *   - phase-continuous VSS and HEI frequency changes
 *   - true acceleration/deceleration drive-cycle ramps
 *   - event-driven simulated VSS and HEI tach electrical pulse trains
 *   - cluster speed and RPM are now measured from pulse periods
 *   - pulse scaling is isolated in explicit simulator calibration constants
 *   - independent 1986 Corvette cluster-input model
 *   - direct VSS and HEI tach inputs are kept separate from ECM serial
 *   - fuel/coolant/oil/battery are modeled as direct cluster inputs
 *   - no unproven MPG/range semantics are assigned
 *   - independent PC-side pulse-width receiver
 *   - receiver observes only serial line transitions and transition times
 *   - reconstructs 8-bit values MSB-first after one framing pulse per byte
 *   - end-to-end comparison: ECM source -> LF880 -> waveform -> receiver
 *
 * The serial logic is translated from instructions first.  Names such as
 * "display" and field meanings remain provisional where they originate only
 * from disassembly comments.
 *
 * Important: the code at $5800-$581E is external to the BUA image supplied,
 * so this file models those entry points only as hooks.  The normal 160-baud
 * display data is separate and is present in the BUA source itself.
 */
#include <stdio.h>
#include <string.h>
#include <limits.h>
typedef unsigned char  bua_u8;
typedef signed char    bua_s8;
typedef unsigned short bua_u16;
typedef signed short   bua_s16;
typedef unsigned long  bua_u32;
#if UCHAR_MAX != 255
#error Requires 8-bit unsigned char
#endif
#if USHRT_MAX != 65535
#error Requires 16-bit unsigned short
#endif
typedef struct {
    bua_u8 low[0x0500u];
    bua_u8 mpu[0x0040u];
    bua_u8 io4000[0x0010u];
    bua_u8 io5000;
} BuaMemory;
typedef struct {
    bua_u32 irq_ticks;
    bua_u32 air_fuel_loops;
    bua_u32 spark_vss_loops;
    bua_u32 slow_o2_filter_calls;
    bua_u32 common_maf_calls;
    bua_u32 common_tps_calls;
    bua_u32 odd_load_producer_calls;
    bua_u32 odd_transient_front_calls;
    bua_u32 odd_50ms_tail_calls;
    bua_u32 transient_filter_updates;
    bua_u32 scheduler_blm_select_calls;
    bua_u32 scheduler_blm_cell_changes;
    bua_u32 scheduler_blm_reinitializations;
    bua_u32 scheduler_blm_store_qualifications;
    bua_u32 scheduler_blm_updates;
    bua_u32 physical_injector_services;
    bua_u32 common_injector_bookkeeping;
    bua_u32 running_injector_bookkeeping;
    bua_u32 odd_fuel_chain_calls;
    bua_u32 odd_crank_fuel_calls;
    bua_u32 odd_normal_fuel_calls;
    bua_u32 odd_mpu_fuel_writes;
    bua_u32 scheduler_async_calls;
    bua_u32 scheduler_async_hw_pulses;
    bua_u32 async_csr_triggers;
    bua_u32 iac_minor_services;
    bua_u32 iac_steps_consumed;
    bua_u32 major_segment_calls[16];
    bua_u32 diagnostic_segment_calls;
    bua_u32 ignition_shutdown_calls;
    bua_u32 iac_shutdown_homing_calls;
    bua_u32 soft_powerdown_events;
    bua_u32 one_second_events;
    bua_u32 vss_capture_changes;
    bua_u32 hu_5806_calls;
    bua_u32 display_bytes_loaded;
    bua_u32 serial_cells;
    bua_u32 serial_short_low_cells;
    bua_u32 serial_long_low_cells;
    bua_u32 serial_manager_calls;
    bua_u32 receiver_pulses;
    bua_u32 receiver_frames;
    bua_u32 receiver_bytes;
    bua_u32 receiver_bad_widths;
    bua_u32 ref_pulses;
    bua_u32 rpm_calculations;
    bua_u32 engine_run_transitions;
    bua_u32 reference_wait_events;
    bua_u32 dwell_calculations;
    bua_u32 main_spark_lookups;
    bua_u32 coolant_spark_lookups;
    bua_u32 highway_spark_lookups;
    bua_u32 hot_restart_evaluations;
    bua_u32 startup_spark_initializations;
    bua_u32 startup_spark_inject_updates;
    bua_u32 startup_spark_decay_events;
    bua_u32 knock_attack_evaluations;
    bua_u32 knock_recovery_evaluations;
    bua_u32 knock_recovery_changes;
    bua_u32 partial_spark_calculations;
    bua_u32 final_spark_calculations;
    bua_u32 spark_mode4_evaluations;
    bua_u32 spark_blend_evaluations;
    bua_u32 spark_timing_calculations;
    bua_u32 spark_timing_slew_limits;
    bua_u32 est_output_evaluations;
    bua_u32 est_enable_events;
    bua_u32 est_disable_events;
    bua_u32 est_error42_events;
    bua_u32 est_mode4_bypass_events;
    bua_u32 stall_saver_entries;
    bua_u32 stall_saver_exits;
    bua_u32 low_rpm_spark_resets;
    bua_u32 fuel_base_pw_calculations;
    bua_u32 fuel_base_pw_table_lookups;
    bua_u32 lf2f0_calls;
    bua_u32 lf472_calls;
    bua_u32 fuel_blm_corrections;
    bua_u32 fuel_closed_loop_corrections;
    bua_u32 fuel_o2_fast_evaluations;
    bua_u32 fuel_o2_control_evaluations;
    bua_u32 fuel_o2_integrator_updates;
    bua_u32 tps_ae_triggers;
    bua_u32 tps_ae_cancels;
    bua_u32 async_ae_pulses;
    bua_u32 injector_event_updates;
} BuaStats;
static BuaMemory mem;
static BuaStats stats;
/* PC regression switch: replay old frozen baselines without newly wired D. */
static bua_u8 sim_legacy_segment_d_freeze = 0u;
/* PC regression switch for pre-Step-110 software-only Segment-1 staging. */
static bua_u8 sim_legacy_segment1_output_freeze = 0u;
/* PC regression switch for signatures frozen before key-off integration. */
static bua_u8 sim_legacy_ignition_shutdown_freeze = 0u;
/* HAL-visible endpoint corresponding to the source's software-interrupt loop. */
static bua_u8 sim_soft_powerdown_latched = 0u;
static bua_u8 sim_iac_motor_on = 1u;
static bua_u8 sim_timer8;
static bua_u8 sim_maf_adc = 128u;
static bua_u8 sim_o2_adc = 128u;
static bua_u8 sim_tps_adc = 35u;
static bua_u8 sim_diag_adc = 200u;
/* PC-only VSS signal generator.  Capture register emulates P4 L3FC2. */
static bua_u32 sim_vss_abs_ticks;
static bua_u32 sim_vss_next_edge;
static bua_u16 sim_vss_capture_reg;
static bua_u16 sim_vss_period_ticks;
static bua_u8 sim_serial_line_high;
typedef struct {
    bua_u8 have_low_start;
    bua_u32 low_start_time;
    bua_u8 pulse_in_frame;
    bua_u8 assembling;
    bua_u8 bytes[16];
    bua_u16 frame_widths[16];
    bua_u8 byte_count;
} BuaSerialReceiver;
static BuaSerialReceiver rx;
static bua_u8 rx_enabled;
/* ---------------------------------------------------------------------- */
/* PC-only 1986 Corvette cluster input model.                              */
/*                                                                        */
/* These are SIGNAL SOURCES, not a claimed reproduction of the cluster    */
/* firmware.  The uploaded schematic establishes separate direct inputs   */
/* for VSS, HEI tach, fuel, coolant, oil pressure/temp and battery, plus   */
/* the ECM serial-data input.  Unknown electrical transfer functions are   */
/* deliberately left outside this model for now.                           */
/* ---------------------------------------------------------------------- */
/*
 * Pulse calibration constants are intentionally isolated here.
 * They are PC/HARDWARE-SIMULATOR parameters, not yet claims about the
 * exact 1986 Corvette electrical scaling.  When we verify the factory
 * VSS pulses/mile and HEI tach pulses/revolution, only these constants
 * need to change.
 */
#define SIM_VSS_PULSES_PER_MILE  4000ul
#define SIM_TACH_PULSES_PER_REV     4ul
typedef struct {
    unsigned int fuel_sender_raw;    /* arbitrary PC stimulus units */
    unsigned int coolant_raw;
    unsigned int oil_pressure_raw;
    unsigned int oil_temp_raw;
    unsigned int battery_mv;
} DashDirectInputs;
typedef struct {
    bua_u32 period_us;
    bua_u32 high_us;
    bua_u32 next_edge_us;
    bua_u8 level;
    bua_u32 rising_edges;
    bua_u32 falling_edges;
} SimPulseSource;
typedef struct {
    unsigned int speed_mph;
    unsigned int rpm;
    bua_u32 vss_last_rise_us;
    bua_u32 vss_period_us;
    bua_u32 tach_last_rise_us;
    bua_u32 tach_period_us;
    bua_u32 vss_rising_edges;
    bua_u32 tach_rising_edges;
    bua_u8 vss_valid;
    bua_u8 tach_valid;
    unsigned int fuel_sender_raw;
    unsigned int coolant_raw;
    unsigned int oil_pressure_raw;
    unsigned int oil_temp_raw;
    unsigned int battery_mv;
    bua_u8 serial_bytes[16];
    bua_u8 serial_count;
} DashModel;
static DashDirectInputs dash_in;
static DashModel dash;
static SimPulseSource sim_dash_vss;
static SimPulseSource sim_dash_tach;
static bua_u32 sim_vehicle_time_us;
/* PC-only P4 distributor-reference model.
 * For the 8-cylinder BUA calibration (LC009 == 0), the executable RPM math
 * is consistent with four reference pulses per crankshaft revolution and a
 * 65.536-kHz period counter.  Keep this separate from the cluster HEI tach
 * generator so neither decoder can accidentally share computed RPM. */
#define SIM_ECM_REF_PULSES_PER_REV 4ul
#define SIM_P4_REF_TIMER_HZ       65536ul
typedef struct {
    double phase;
    double frequency_hz;
    bua_u32 absolute_timer_ticks;
    bua_u32 last_edge_ticks;
    bua_u16 period_reg;
    bua_u8 period_valid;
    bua_u8 pulse_since_irq;
    bua_u8 injector_ref_phase;
    bua_u8 injector_services_pending;
} SimEcmReference;
static SimEcmReference sim_ecm_ref;
static bua_u32 round_div_u32(bua_u32 numerator, bua_u32 denominator)
{
    if (denominator == 0ul)
        return 0ul;
    return (numerator + denominator / 2ul) / denominator;
}
static bua_u32 scale_ratio_u32(bua_u32 value, bua_u32 multiplier,
                               bua_u32 divisor)
{
    double scaled;
    if (divisor == 0ul)
        return 0ul;
    /* PC-only waveform scheduling helper.  Double is intentional here to
     * avoid 32-bit overflow in value*multiplier at very low VSS rates.
     * No ECM algorithm uses this helper. */
    scaled = ((double)value * (double)multiplier) / (double)divisor;
    if (scaled < 0.5)
        return 0ul;
    return (bua_u32)(scaled + 0.5);
}
static void pulse_source_set_period(SimPulseSource *p, bua_u32 period_us)
{
    bua_u32 old_interval;
    bua_u32 new_interval;
    bua_u32 remaining;
    bua_u32 scaled_remaining;
    /*
     * Step 9 preserves waveform phase when frequency changes.  The old
     * Step-8 helper restarted the waveform at every command change, which
     * produced artificial zero-speed/zero-RPM samples at profile boundaries.
     *
     * A square-wave source has one edge every half-period.  Preserve the
     * fraction of the current half-cycle that remains, then scale that
     * fraction to the new half-period.  This is a PC signal-generator model;
     * the production hardware implementation can use an integer phase
     * accumulator or timer-compare rescheduling to obtain the same behavior.
     */
    if (period_us == 0ul) {
        p->period_us = 0ul;
        p->high_us = 0ul;
        p->next_edge_us = 0ul;
        p->level = 0u;
        return;
    }
    new_interval = period_us / 2ul;
    if (new_interval == 0ul)
        new_interval = 1ul;
    if (p->period_us == 0ul || p->next_edge_us <= sim_vehicle_time_us) {
        /* Starting from rest: wait one half-cycle before the first edge. */
        p->level = 0u;
        p->next_edge_us = sim_vehicle_time_us + new_interval;
    } else {
        old_interval = p->period_us / 2ul;
        if (old_interval == 0ul)
            old_interval = 1ul;
        remaining = p->next_edge_us - sim_vehicle_time_us;
        if (remaining > old_interval)
            remaining = old_interval;
        scaled_remaining = scale_ratio_u32(remaining, new_interval,
                                           old_interval);
        if (scaled_remaining == 0ul)
            scaled_remaining = 1ul;
        p->next_edge_us = sim_vehicle_time_us + scaled_remaining;
    }
    p->period_us = period_us;
    p->high_us = new_interval;
}
static void sim_set_dash_vss_mph(unsigned int mph)
{
    bua_u32 denominator;
    bua_u32 period;
    if (mph == 0u) {
        pulse_source_set_period(&sim_dash_vss, 0ul);
        return;
    }
    denominator = (bua_u32)mph * SIM_VSS_PULSES_PER_MILE;
    period = round_div_u32(3600000000ul, denominator);
    pulse_source_set_period(&sim_dash_vss, period);
}
static void sim_set_dash_tach_rpm(unsigned int rpm)
{
    bua_u32 denominator;
    bua_u32 period;
    if (rpm == 0u) {
        pulse_source_set_period(&sim_dash_tach, 0ul);
        return;
    }
    denominator = (bua_u32)rpm * SIM_TACH_PULSES_PER_REV;
    period = round_div_u32(60000000ul, denominator);
    pulse_source_set_period(&sim_dash_tach, period);
}
static void dash_vss_edge(bua_u8 high, bua_u32 when_us)
{
    if (high != 0u) {
        ++dash.vss_rising_edges;
        if (dash.vss_valid != 0u) {
            dash.vss_period_us = when_us - dash.vss_last_rise_us;
            if (dash.vss_period_us != 0ul)
                dash.speed_mph = (unsigned int)round_div_u32(
                    3600000000ul,
                    dash.vss_period_us * SIM_VSS_PULSES_PER_MILE);
        }
        dash.vss_last_rise_us = when_us;
        dash.vss_valid = 1u;
    }
}
static void dash_tach_edge(bua_u8 high, bua_u32 when_us)
{
    if (high != 0u) {
        ++dash.tach_rising_edges;
        if (dash.tach_valid != 0u) {
            dash.tach_period_us = when_us - dash.tach_last_rise_us;
            if (dash.tach_period_us != 0ul)
                dash.rpm = (unsigned int)round_div_u32(
                    60000000ul,
                    dash.tach_period_us * SIM_TACH_PULSES_PER_REV);
        }
        dash.tach_last_rise_us = when_us;
        dash.tach_valid = 1u;
    }
}
static void pulse_source_advance(SimPulseSource *p, bua_u32 end_us,
                                 void (*edge_fn)(bua_u8, bua_u32))
{
    bua_u32 low_us;
    if (p->period_us == 0ul)
        return;
    low_us = p->period_us - p->high_us;
    if (low_us == 0ul)
        low_us = 1ul;
    while (p->next_edge_us < end_us) {
        p->level = (bua_u8)(p->level == 0u);
        if (p->level != 0u) {
            ++p->rising_edges;
            edge_fn(1u, p->next_edge_us);
            p->next_edge_us += p->high_us;
        } else {
            ++p->falling_edges;
            edge_fn(0u, p->next_edge_us);
            p->next_edge_us += low_us;
        }
    }
}
static void sim_advance_dash_signals_6p25ms(void)
{
    bua_u32 end_us;
    end_us = sim_vehicle_time_us + 6250ul;
    pulse_source_advance(&sim_dash_vss, end_us, dash_vss_edge);
    pulse_source_advance(&sim_dash_tach, end_us, dash_tach_edge);
    sim_vehicle_time_us = end_us;
}
static void dash_set_test_inputs(void)
{
    dash_in.fuel_sender_raw = 128u;
    dash_in.coolant_raw = 96u;
    dash_in.oil_pressure_raw = 160u;
    dash_in.oil_temp_raw = 88u;
    dash_in.battery_mv = 14200u;
}
static void dash_update_direct_inputs(void)
{
    /* Analog sender transfer functions remain intentionally unscaled. */
    dash.fuel_sender_raw = dash_in.fuel_sender_raw;
    dash.coolant_raw = dash_in.coolant_raw;
    dash.oil_pressure_raw = dash_in.oil_pressure_raw;
    dash.oil_temp_raw = dash_in.oil_temp_raw;
    dash.battery_mv = dash_in.battery_mv;
}
static void dash_accept_serial_receiver(void)
{
    bua_u8 i;
    dash.serial_count = rx.byte_count;
    if (dash.serial_count > 16u)
        dash.serial_count = 16u;
    for (i = 0u; i < dash.serial_count; ++i)
        dash.serial_bytes[i] = rx.bytes[i];
}
#define RAM8(a) (mem.low[(bua_u16)(a)])
#define MINOR_COUNT          RAM8(0x0000u)
#define IGN_MODE_WORD        RAM8(0x0033u)
#define ENGINE_MODE_WORD     RAM8(0x0034u)
#define MINOR_MODE_WORD2      RAM8(0x0035u)
#define ALDL_MODE_WORD       RAM8(0x003Cu)
#define SERIAL_MODE_WORD     RAM8(0x0046u)
#define COOLANT              RAM8(0x005Bu)
#define RPM_SPECIAL          RAM8(0x0056u) /* translated LCE1B result */
#define RPM_DIV25            RAM8(0x0057u)
#define RPM_DIV12P5          RAM8(0x0058u) /* filtered */
#define REF_PERIOD_ADDR      0x0095u
#define REF_PERIOD_OLD_ADDR  0x0097u
#define DYNAMIC_DWELL_ADDR   0x0099u
#define DWELL_COUNTS_ADDR     0x009Bu
#define STARTUP_COUNTER      RAM8(0x0093u)
#define COLD_START_REF_COUNT RAM8(0x0118u)
#define CRANK_RPM_FILTER_ADDR 0x0127u
#define CRANK_FF_COUNTER     RAM8(0x0129u)
#define NO_REF_MINOR_COUNT   RAM8(0x009Fu)
#define SPARK_MODE_WORD      RAM8(0x0036u)
#define LOAD_25MS_OLD         RAM8(0x0062u)
#define FILTERED_LOAD         RAM8(0x0063u)
#define BATTERY_AD            RAM8(0x007Eu)
#define MODE_WORD1            RAM8(0x0001u)
#define STARTUP_COOLANT       RAM8(0x005Fu)
#define HIGHWAY_SPARK_TIMER   RAM8(0x00A6u)
#define STARTUP_SPARK_REPEAT   RAM8(0x0012u)
#define STARTUP_SPARK_ADDR      0x0013u
#define STARTUP_SPARK_DELAY_ADDR 0x0015u
#define SPARK_AUX_MODE_WORD    RAM8(0x003Bu)
#define MODE_WORD2             RAM8(0x0002u)
#define MODE_WORD3             RAM8(0x0003u)
#define DIAG_MODE_WORD2        RAM8(0x0040u)
#define AF_MODE_WORD           RAM8(0x0043u)
#define KNOCK_RETARD           RAM8(0x00A5u)
#define SPARK_INPUT_MODE_WORD   RAM8(0x0030u)
#define DIAG_MODE_WORD3         RAM8(0x0041u)
#define FINAL_SPARK_ADV_ADDR      0x0115u
#define SPARK_REL_REF_ADDR        0x009Du
#define SPARK_BLEND_MULT          RAM8(0x012Au)
#define ALDL_FUNC_MOD_ENABLE      RAM8(0x0158u)
#define ALDL_SPARK_COMMAND        RAM8(0x015Bu)
#define ALDL_MODE4_WORD4          RAM8(0x0154u)
#define SPARK_FEEDBACK_COUNT      RAM8(0x00B4u)
#define OLD_SPARK_PERIOD_ADDR       0x00B2u
#define NEW_ERROR_WORD2           RAM8(0x004Eu)
#define ENGINE_RUN_TIME_ADDR        0x001Au
#define TRANSIENT_LOAD_ADDR         0x00E5u
#define ACCEL_FUEL_ADDR             0x00D9u
#define ACCEL_LOAD_FACTOR          RAM8(0x00E2u)
#define LOWRPM_AUX_VALUE           RAM8(0x0104u)
#define AIRFLOW_WORD_ADDR            0x00EAu
#define TOTAL_AFR_ADDR               0x00CEu
#define INJECT_MODE_WORD            RAM8(0x0039u)
#define AIR_MODE_WORD               RAM8(0x003Du)
#define AF_MODE_WORD2               RAM8(0x0044u)
#define CLOSED_LOOP_PW_CORR         RAM8(0x00C6u)
#define CLOSED_LOOP_INT_TIMER       RAM8(0x00C4u)
#define CLOSED_LOOP_DURATION        RAM8(0x00C7u)
#define FAST_O2_PREVIOUS            RAM8(0x00C8u)
#define CLOSED_LOOP_INTEGRATOR      RAM8(0x00C9u)
#define O2_LEAN_OFFSET              RAM8(0x00D1u)
#define O2_LOOKUP_AIRFLOW           RAM8(0x00D2u)
#define O2_FILTERED_FAST            RAM8(0x006Fu)
#define O2_FILTERED_SLOW            RAM8(0x0071u)
#define TPS_LOAD_AXIS               RAM8(0x0082u)
#define O2_ERROR_LIMITED            RAM8(0x046Bu)
#define O2_PROPORTIONAL_STEP        RAM8(0x046Cu)
#define REF_SEEN_BIT         0x04u
#define SECOND_REF_VALID_BIT 0x08u
#define ENGINE_RUNNING_BIT   0x80u
/* VSS RAM from the original BUA layout. */
#define VSS_MPH_HI           RAM8(0x0065u) /* integer/high byte of 8.8 MPH */
#define VSS_MPH_LO           RAM8(0x0066u)
#define VSS_SCALED           RAM8(0x0067u)
#define VSS_NO_EDGE_COUNT    RAM8(0x0068u)
#define VSS_TIME_OLD         0x0069u
#define VSS_TIME_MID         0x006Bu
#define VSS_TIME_NEW         0x006Du
#define VSS_ERROR_COUNT      RAM8(0x00ABu)
#define DISPLAY_DISTANCE_COUNT RAM8(0x011Eu) /* source: run total distance */
#define BLM                  RAM8(0x00C0u)
#define ALCL_BIT_COUNT       RAM8(0x0189u)
#define ALCL_TABLE_INDEX     RAM8(0x018Au)
#define ALCL_XMIT_BYTE       RAM8(0x018Bu)
#define TIMER_SNAPSHOT       RAM8(0x018Cu)
/* Calibrations used by the translated VSS routine. */
#define CAL_VSS_FILTER_COEF  255u  /* LC284 = 0.996 */
#define CAL_VSS_ERR_TIME     2u    /* LC1F7 */
#define CAL_CMD_SPEED_PARAM  6u    /* LC64E */
#define CAL_STARTUP_REF_PERIOD 3277u /* LC018: startup RPM threshold */
#define CAL_STARTUP_LOOPS       8u   /* LC01A: 12.5-ms qualifying loops */
#define CAL_NO_REF_WAIT_LOOPS  23u   /* LCDC1 literal compare */
#define CAL_RPM_FILTER_COEF   128u   /* LC62E */
#define CAL_DWELL_LOAD_DELTA    32u   /* LC01B */
#define CAL_DWELL_BATT_REF      124u   /* LCEF9 literal: 12.4-V A/D value */
#define CAL_DWELL_BATT_GAIN       4u   /* LCF02 literal */
#define CAL_DWELL_REF_MARGIN     39u   /* LCF05 literal */
/* Step 22 base-fuel calibrations.  LC016 bit 6 is set in this image, so
 * the executable selects the calculated base-pulse-width path at LDD2D. */
#define CAL_AF_MODE3                 0x62u  /* LC016 */
#define CAL_BASE_PW_CALC_BIT         0x40u  /* LC016 b6 */
#define CAL_BASE_TABLE_MULT           735u  /* LC3CD */
#define CAL_INJ_FLOW_DOUBLE           441u  /* LC3CF */
#define CAL_INJ_FLOW_SINGLE           882u  /* LC3D1 */
#define CAL_CRANK_REF_LIMIT             0u  /* LC357 */
#define CAL_LOAD_SCALE                  80u  /* LC69A */
#define CAL_CRANK_RPM_COEF            128u  /* LC359 */
#define CAL_CRANK_FF_SKIP_COUNT         0u  /* LC35A */
#define CAL_CRANK_PW_SCALE           6554u  /* LC36C */
#define CAL_CRANK_COOLANT_MAX          208u  /* 115 C clamp */
#define CAL_COLD_START_RUN_TIME          32u  /* LC37C */
#define CAL_COLD_START_TPS_LIMIT        197u  /* LC37E */
#define INJECT_SINGLE_FIRE_BIT       0x80u  /* L0039 b7 */
#define CAL_BASE_FUEL_ROWS             17u
#define CAL_BASE_FUEL_COLS             17u
#define CLOSED_LOOP_NEUTRAL            128u  /* L00C6 neutral */
#define AF_RICH_LEAN_BIT              0x40u  /* L0044 b6 */
#define AIR_INJECT_TOGGLE_BIT         0x01u  /* L003D b0; used for odd-step rounding */
/* Step 24 closed-loop O2 control calibrations. */
#define AF_CLOSED_LOOP_BIT             0x80u  /* L0044 b7 */
#define AF_SLOW_O2_RICH_BIT            0x20u  /* L0044 b5 */
#define CAL_O2_READY_HIGH               158u  /* LC559 */
#define CAL_O2_READY_LOW                 45u  /* LC55A */
#define CAL_O2_FAST_WINDOW               12u  /* LC561 */
#define CAL_O2_AIR_DIV_WINDOW            23u  /* LC562 */
#define CAL_O2_MIN_INT_ERROR               4u  /* LC563 */
#define CAL_O2_POS_ERROR_GAIN            224u  /* LC564 */
#define CAL_O2_IDLE_TPS                     3u  /* LC55E */
#define CAL_O2_IDLE_INT_DELAY              16u  /* LC55F */
#define CAL_O2_IDLE_GAIN                  160u  /* LC560 */
#define CAL_O2_COOL_INT_OFFSET             96u  /* LC566 */
#define CAL_O2_INT_MIN                     40u  /* LC55C */
#define CAL_O2_INT_MAX                    180u  /* LC55D */
#define AIR_DIVERTER_ACTIVE_BIT          0x02u  /* L003E b1 in this path */
static const bua_u8 cal_o2_fast_threshold[9] =
    {130u,138u,142u,142u,138u,134u,122u,120u,118u};
static const bua_u8 cal_o2_int_delay_airflow[9] =
    {32u,28u,24u,22u,20u,18u,16u,16u,12u};
static const bua_u8 cal_o2_error_reduction[9] =
    {200u,224u,236u,244u,248u,255u,255u,255u,255u};
static const bua_u8 cal_o2_prop_width_rpm[6] =
    {20u,12u,8u,4u,3u,2u};
static const bua_u8 cal_o2_prop_step_error[12] =
    {3u,3u,3u,2u,2u,2u,3u,4u,7u,10u,12u,16u};
static const bua_u8 cal_o2_prop_width_error[12] =
    {4u,4u,4u,4u,4u,8u,12u,24u,32u,64u,128u,240u};
static const bua_u8 cal_o2_int_delay_error[12] =
    {255u,255u,255u,255u,128u,68u,64u,64u,64u,64u,64u,64u};
static const bua_u8 cal_o2_slow_upper[9] =
    {140u,148u,152u,152u,148u,144u,132u,128u,125u};
static const bua_u8 cal_o2_slow_lower[9] =
    {120u,128u,132u,132u,128u,124u,112u,109u,106u};
/* LCD05 LC58B and LC565. */
#define CAL_O2_COLD_FILTER_COEF          154u
static const bua_u8 cal_o2_slow_filter_coef[9] =
    {16u,24u,32u,36u,40u,44u,44u,44u,44u};
/* Step 25 AFR / open-loop / power-enrichment calibrations. */
#define CAL_STOICH_CODE                  445u
#define CAL_OPEN_LOOP_AFR_BIAS           221u
#define CAL_PE_LOAD_ENABLE                50u
#define CAL_PE_LOAD_HYST                  10u
#define CAL_PE_TPS_HYST                   16u
#define CAL_PE_TPS_THRESHOLD             180u
#define AF_POWER_ENRICH_BIT              0x20u
static const bua_u8 cal_open_loop_load_afr[17] =
    {0u,0u,0u,0u,0u,3u,8u,8u,13u,18u,23u,23u,23u,23u,23u,23u,23u};
static const bua_u8 cal_pe_coolant_change[9] =
    {140u,125u,112u,86u,58u,58u,58u,58u,58u};
static const bua_u8 cal_pe_rpm_change[17] =
    {128u,128u,128u,128u,128u,128u,128u,128u,128u,128u,128u,128u,128u,128u,128u,128u,128u};
/* Step 48 startup/open-loop AFR producer calibrations. */
#define STEP48_STARTUP_COOL_MAX          208u
#define STEP48_NVRAM_VALID_BIT          0x08u
static const bua_u8 cal_startup_enrich_step48[14] =
    {235u,215u,194u,171u,115u,36u,36u,36u,36u,26u,26u,26u,26u,26u};
static const bua_u8 cal_startup_delay_step48[14] =
    {175u,158u,140u,140u,140u,127u,120u,100u,80u,73u,67u,67u,67u,67u};
static const bua_u8 cal_startup_repeat_step48[14] =
    {23u,23u,23u,23u,23u,21u,20u,16u,13u,12u,11u,11u,11u,11u};
static const bua_u8 cal_startup_decay_step48[14] =
    {252u,252u,252u,252u,252u,252u,252u,151u,151u,151u,151u,151u,151u,151u};
static const bua_u8 cal_open_loop_coolant_step48[14] =
    {89u,68u,64u,53u,49u,35u,25u,25u,25u,25u,25u,25u,25u,25u};
/* Step 26 LF901/LF92A fuel-output calibrations. */
#define CAL_MIN_BASE_PW                    111u  /* LC3A1 */
#define CAL_DEFAULT_BASE_PW                111u  /* LC3A3 */
#define CAL_FUEL_CUTOFF_SPEED_HI           255u  /* LC3A5 */
#define CAL_FUEL_CUTOFF_REF_HI              98u  /* LC3A6 */
#define CAL_FUEL_CUTOFF_SPEED_REENABLE     254u  /* LC3A8 */
#define CAL_FUEL_CUTOFF_REF_REENABLE       103u  /* LC3A9 */
#define FUEL_VATS_OK_BIT                  0x20u  /* L0003 b5 */
#define FUEL_IGN_OFF_BIT                  0x10u  /* L0033 b4 */
#define FUEL_DFCO_ZERO_BIT                0x02u  /* L0043 b1 */
#define FUEL_HI_CUTOFF_BIT                0x40u  /* L0043 b6 */
#define FUEL_ENGINE_RUNNING_BIT           0x80u  /* L0034 b7 */
#define AE_FIRST_TIME_BIT                 0x02u  /* L0038 b1 */
#define AE_INJECT_SINCE_BIT               0x04u  /* L0038 b2 */
#define AE_ACTIVE_BIT                     0x08u  /* L0038 b3 */
#define FUEL_LIMIT_TRIGGER_STATE_BIT      0x10u  /* L0038 b4 */
#define FUEL_LIMIT_AE_MASK                0xC0u  /* L0038 b7/b6 */
#define FUEL_LIMIT_DECEL_MASK             0xA0u  /* L003E b7/b5 */
#define CAL_DECEL_LIMIT_TRIGGER           128u   /* LC5F8 */
#define CAL_DECEL_LIMIT_AFTER             128u   /* LC5F9 */
#define SINGLE_FIRE_BIT                   0x80u  /* L0039 b7 */
#define SINGLE_FIRE_ZERO_PW_BIT           0x40u  /* L0039 b6 */
#define FORCE_LOW_PW_BIT                  0x02u  /* L0033 b1 */
static const bua_u8 cal_small_pw_offset[14] =
    {18u,15u,13u,10u,8u,5u,3u,0u,0u,0u,0u,0u,0u,0u};
/* LC331, coolant-bucketed fuel-limit control byte. */
static const bua_u8 cal_fuel_limit_coolant[8] =
    {217u,217u,217u,217u,217u,169u,169u,169u};
/* Step 28 TPS-triggered asynchronous acceleration enrichment. */
#define CAL_TPS_AE_POS_THRESHOLD           10u  /* LC30E */
#define CAL_TPS_AE_NEG_THRESHOLD           10u  /* LC30F */
#define CAL_ASYNC_MIN_PW                  111u  /* LC310 */
#define CAL_ASYNC_MAX_PW                  364u  /* LC312 */
#define TPS_AE_INHIBIT_MASK              0x24u  /* L003F b5/b2 */
#define TPS_AE_ACTIVE_BIT                0x80u  /* L0038 b7 */
#define TPS_AE_LIMIT_BIT                 0x40u  /* L0038 b6 */
#define TPS_AE_PHASE_BIT                 0x20u  /* L0038 b5 */
/* Step 29 LF71C..LF768 injector-event state. */
#define SINGLE_FIRE_TOGGLE_ENABLE_BIT    0x10u  /* L0039 b4 */
#define INJECT_ROUND_TOGGLE_BIT          0x01u  /* L003D b0 */
#define INJECT_EVENT_STATE_BITS          0x14u  /* L0038 b4/b2 */
#define DECEL_LIMIT_ACTIVE_BIT           0x20u  /* L003E b5 */
#define DECEL_STATE_HOLD_BIT             0x80u  /* L003E b7 */
#define CAL_FUEL_LIMIT_INJECTS              2u  /* LC314 */
static const bua_u8 cal_async_bpwpct[8] =
    {32u,32u,32u,32u,32u,32u,32u,32u};
static const bua_u8 cal_async_pulse_count[9] =
    {12u,12u,12u,12u,12u,12u,12u,12u,12u};
static const bua_u8 cal_async_cool_factor[9] =
    {128u,128u,128u,128u,128u,96u,64u,64u,64u};
/* Segment 7, C5BB/C31D/C327/C613 coolant producers.  The supplied source's
 * C5BB label is damaged as LF2C6, but the table bytes and address use agree. */
static const bua_u8 cal_o2_lean_offset_c5bb[14] =
    {0u,0u,0u,0u,0u,16u,16u,16u,16u,16u,16u,0u,0u,0u};
/* Step 30 LF76A..LF87F MAF / airflow calibrations. */
#define CAL_AF_MODE1                    0xB4u  /* LC014 */
#define AF_ANALOG_MAF_BIT               0x04u  /* LC014 b2 */
#define AF_USE_FLOW_FILTER_BIT          0x80u  /* LC014 b7 */
#define CAL_MAF_SLIDING_FILTER            32u  /* LC6E7 */
#define CAL_MAF_MIN_DIGITAL_PERIOD      2048u  /* LC6E8 */
#define CAL_MAF_MIN_FLOW_Q8_8            768u  /* LC6EA = 3.0 g/s */
#define CAL_MAF_DEFAULT_TPS_LIMIT        167u  /* LC213 */
#define CAL_MAF_DEFAULT_IAC_SCALE         21u  /* LC214 */
#define CAL_MAF_DEFAULT_OFFSET_Q8_8     1024u  /* LC215 = 4.0 g/s */
/* Step 32 LDB9B..LDBD4 closed-loop reset/qualification gates. */
#define CAL_AF_MODE4                    0x00u  /* LC017 */
#define CAL_CL_RESET_ON_BLM_CHANGE_BIT  0x20u  /* LC016 b5 */
#define CAL_CL_RESET_DURING_AE_BIT       0x02u  /* LC016 b1 */
#define AF_BLM_CELL_CHANGE_BIT           0x04u  /* L0043 b2 */
#define CL_AE_RESET_MASK                 0x88u  /* L0038 b7/b3 */
#define CAL_CL_RESET_ON_CCP_CHANGE_BIT   0x08u  /* LC017 b3 */
#define DIAG_INT_FORCED_NEUTRAL_BIT      0x04u  /* L0041 b2 */
#define ALDL_DIAG_MODE_BIT               0x20u  /* L0035 b5 */
#define CAL_DECEL_CL_LOAD_MAX              40u  /* LC5F1 */
#define CAL_DECEL_CL_RPM_MIN               28u  /* LC5F2, RPM/25 = 700 RPM */
#define CAL_DECEL_INT_NEUTRAL              128u  /* intended operand for source CMPA #1280 */
static const bua_u8 cal_maf_scalar[6] =
    {23u,48u,83u,135u,207u,255u};
static const bua_u8 cal_maf_table1[9] =
    {0u,36u,50u,69u,93u,121u,155u,197u,248u};
static const bua_u8 cal_maf_table2[9] =
    {119u,133u,147u,163u,182u,198u,217u,237u,254u};
static const bua_u8 cal_maf_table3[9] =
    {147u,158u,170u,182u,195u,209u,223u,238u,253u};
static const bua_u8 cal_maf_table4[9] =
    {156u,166u,176u,187u,198u,210u,222u,237u,255u};
static const bua_u8 cal_maf_table5[9] =
    {166u,176u,186u,196u,207u,219u,230u,242u,255u};
static const bua_u8 cal_maf_table6[17] =
    {207u,213u,219u,226u,233u,240u,248u,255u,255u,255u,255u,255u,255u,255u,255u,255u,255u};
static const bua_u8 cal_maf_max_flow[17] =
    {23u,23u,30u,48u,68u,89u,111u,141u,170u,200u,220u,236u,245u,247u,247u,247u,255u};
static const bua_u8 cal_maf_default_rpm[9] =
    {30u,90u,110u,135u,150u,175u,210u,225u,230u};
/* Step 20 spark summation / limit calibrations. */
#define CAL_INITIAL_REF_LEAD_RAW       17u    /* LC01C, about 6 degrees */
#define CAL_MAX_SPARK_REL_REF_RAW     119u    /* LC01E, about 42 degrees */
#define CAL_MIN_SPARK_REL_REF_RAW   65525u    /* LC020 = -11 raw */
#define CAL_DIAG_FIXED_SPARK_RAW       57u    /* LC022 = 20 degrees */
#define CAL_ALDL_ADDED_SPARK_RAW       23u    /* LC024 = about 8 degrees */
#define CAL_ERR43_TEST_SPARK_RAW       57u    /* LC22B = about 10 degrees */
#define CAL_ERR43_TEST_MIN_COOLANT    173u    /* LC22C */
#define SPARK_INPUT_TEST_ENABLE_BIT   0x04u    /* L0030 b2 */
#define MINOR_DIAG_SWITCH_BIT         0x10u    /* L0035 b4 */
#define MINOR_ALDL_SPARK_BIT          0x20u    /* L0035 b5 */
#define MODE2_ERR43_TEST_BIT          0x01u    /* L0002 b0 */
#define MODE2_ERR43_FAIL_BIT          0x80u    /* L0002 b7 */
#define DIAG_FIXED_SPARK_BIT          0x80u    /* L0041 b7 */
/* Step 19 LD192..LD1EE Mode-4 / spark-blend control bits. */
#define MINOR_MODE4_BIT               0x08u    /* L0035 b3 */
#define ALDL_SPARK_ENABLE_BIT          0x08u    /* L0158 b3 */
#define ALDL_SPARK_MODIFY_BIT          0x10u    /* L0158 b4 */
#define ALDL_SPARK_RETARD_BIT          0x20u    /* L0158 b5 */
#define SPARK_ADV_RETARD_FLAG          0x01u    /* L0034 b0 */
/* Step 20 LD1F6..LD25x spark timing/output arithmetic. */
#define CAL_SPARK_TIME_CORR_COUNTS       13u    /* LC01D, about 198 usec */
#define MPU_BASE_ADDR                0x3FC0u
#define MPU_SPARK_OLD_DWELL_ADDR     0x3FDCu    /* previous dwell-period counts */
#define MPU_SPARK_DWELL_DELTA_ADDR   0x3FE6u    /* dwell counter update */
#define MPU_SPARK_FALL_DELTA_ADDR    0x3FE8u    /* current fire - previous fire */
#define MPU_SPARK_FIRE_OFFSET_ADDR   0x3FF6u    /* time from reference to fire */
/* Step 21: remaining EST output / bypass / low-RPM spark state. */
#define MPU_SPARK_PERIOD_ADDR        0x3FC8u
#define MPU_NEXT_DWELL_ADDR          0x3FE4u
#define MPU_LAST_REF_ADDR            0x3FECu
#define MPU_CPU_CONTROL_ADDR         0x3FFCu
#define EST_ENABLE_BIT               0x04u
#define CPU_EST_BYPASS_BIT           0x10u
#define SPARK_STALL_ACTIVE_BIT       0x02u
#define SPARK_STALL_ERROR_SKIP_BIT   0x01u
#define SPARK_FEEDBACK_BYPASS_BIT    0x40u
#define IGN_FIRST_REF_BIT            0x08u
#define IGN_REF_STATE_BIT            0x10u
#define IGN_ERR42_LATCH_BIT          0x40u
#define MODE4_EST_BYPASS_BIT         0x08u
#define NEW_ERROR42_BIT              0x20u
#define CAL_ERR42_FEEDBACK_LIMIT      2u
#define CAL_STALL_RPM_LOW            32u
#define CAL_STALL_RPM_MID            36u
#define CAL_STALL_RPM_AUX            44u
#define CAL_STALL_RPM_DISABLE        44u
#define CAL_TRANSIENT_LOAD_INIT     128u
/* C02E main spark table header and data.  Executable code calls LF27C with
 * A=L0056 and B=load.  Both axes use 16-count spacing.  The 17th row is
 * required because LF27C interpolates between row 15 and row 16 when A is
 * in the final 240/255 interval. */
#define CAL_MAIN_SPARK_MIN_RPM_ARG   0u
#define CAL_MAIN_SPARK_MIN_LOAD     32u
#define CAL_MAIN_SPARK_COLS         12u
#define CAL_MAIN_SPARK_ROWS         17u
/* C42D base injector pulse-width table.  The 9340/16059335 image contains
 * 17 RPM rows (0..6400 in 400-RPM steps) and 17 load columns (0..256 in
 * 16-count steps).  Every RPM row is identical in this calibration. */
#define BASE_FUEL_ROW {0u,8u,15u,23u,31u,38u,46u,54u,61u,69u,77u,84u,92u,100u,107u,115u,123u}
static const bua_u8 cal_base_fuel[CAL_BASE_FUEL_ROWS][CAL_BASE_FUEL_COLS] = {
    BASE_FUEL_ROW, BASE_FUEL_ROW, BASE_FUEL_ROW, BASE_FUEL_ROW, BASE_FUEL_ROW,
    BASE_FUEL_ROW, BASE_FUEL_ROW, BASE_FUEL_ROW, BASE_FUEL_ROW, BASE_FUEL_ROW,
    BASE_FUEL_ROW, BASE_FUEL_ROW, BASE_FUEL_ROW, BASE_FUEL_ROW, BASE_FUEL_ROW,
    BASE_FUEL_ROW, BASE_FUEL_ROW
};
#undef BASE_FUEL_ROW
static const bua_u8 cal_main_spark[CAL_MAIN_SPARK_ROWS][CAL_MAIN_SPARK_COLS] = {
    { 57u, 57u, 57u, 57u, 57u, 57u, 57u, 57u, 57u, 57u, 57u, 57u },
    { 57u, 57u, 57u, 57u, 57u, 57u, 57u, 57u, 57u, 57u, 57u, 57u },
    { 77u, 77u, 77u, 77u, 77u, 70u, 65u, 65u, 63u, 59u, 59u, 59u },
    { 80u, 80u, 80u, 80u, 80u, 73u, 65u, 65u, 65u, 59u, 59u, 59u },
    { 85u, 85u, 85u, 85u, 85u, 78u, 70u, 70u, 70u, 60u, 60u, 60u },
    { 88u, 88u, 88u, 88u, 88u, 81u, 73u, 73u, 73u, 65u, 65u, 65u },
    {102u,102u,101u, 98u, 96u, 94u, 91u, 88u, 77u, 70u, 70u, 70u },
    {106u,106u,105u,102u,100u, 98u, 93u, 91u, 81u, 73u, 73u, 73u },
    {109u,109u,108u,104u,102u, 99u, 95u, 93u, 85u, 75u, 75u, 75u },
    {112u,112u,111u,105u,102u,102u, 98u, 94u, 88u, 78u, 78u, 78u },
    {116u,116u,114u,108u,105u,102u,100u, 97u, 92u, 81u, 81u, 81u },
    {120u,120u,116u,115u,108u,108u,100u, 97u, 94u, 85u, 85u, 85u },
    {134u,134u,125u,122u,108u,108u,100u, 97u, 94u, 85u, 85u, 85u },
    {134u,134u,129u,129u,114u,108u,101u, 97u, 94u, 85u, 85u, 85u },
    {134u,134u,129u,129u,129u,117u,106u, 97u, 94u, 85u, 85u, 85u },
    {134u,134u,129u,129u,129u,117u,106u,106u, 94u, 87u, 87u, 87u },
    {134u,134u,129u,129u,129u,117u,106u,106u, 94u, 92u, 92u, 92u }
};
/* C0FD/C0FE/C0FF coolant spark correction calibration.  The table values
 * include a +20-degree bias represented by raw value 57.  Executable code
 * later subtracts LC0FD from the summed spark terms, so the signed coolant
 * correction in raw spark units is (lookup - 57).
 *
 * C0FE is zero in BUA, therefore the lookup uses filtered load directly,
 * limited to 160 by LCFD4.  Coolant is limited to 208 before LF27C. */
#define CAL_COOL_SPARK_BIAS_RAW       57u
#define CAL_COOL_SPARK_LOAD_SELECTOR   0u
#define CAL_COOL_SPARK_MIN_COOL       32u
#define CAL_COOL_SPARK_MIN_LOAD       32u
#define CAL_COOL_SPARK_COLS            9u
#define CAL_COOL_SPARK_ROWS           12u
static const bua_u8 cal_coolant_spark[CAL_COOL_SPARK_ROWS][CAL_COOL_SPARK_COLS] = {
    {81u,81u,81u,81u,81u,81u,81u,85u,85u},
    {81u,81u,81u,81u,81u,81u,81u,85u,85u},
    {68u,68u,68u,68u,68u,68u,68u,68u,68u},
    {57u,57u,57u,57u,57u,57u,57u,71u,71u},
    {57u,57u,57u,57u,57u,57u,57u,71u,71u},
    {57u,57u,57u,57u,57u,57u,57u,71u,71u},
    {57u,57u,57u,57u,57u,57u,57u,57u,57u},
    {57u,57u,57u,57u,57u,57u,57u,57u,57u},
    {57u,57u,57u,57u,57u,57u,57u,57u,57u},
    {57u,57u,57u,57u,57u,57u,57u,57u,57u},
    {57u,57u,57u,57u,57u,57u,57u,57u,57u},
    {57u,57u,57u,51u,46u,40u,40u,40u,40u}
};
/* Step 15 highway-mode and hot-restart calibrations.
 * BUA LC017 is $00, so its optional EGR/fan and 4th-gear N/V gating
 * branches are disabled by calibration.  The executable path that remains
 * requires coolant > LC16E, load <= LC16F, L0056 >= LC170, and a timer
 * greater than LC171.  The timer increments only when L0000 is zero. */
#define CAL_HIGHWAY_MIN_COOLANT       133u  /* LC16E */
#define CAL_HIGHWAY_MAX_LOAD          100u  /* LC16F */
#define CAL_HIGHWAY_MIN_RPM_SPECIAL   254u  /* LC170, applied to L0056 */
#define CAL_HIGHWAY_DELAY             10u   /* LC171; enable when timer > 10 */
#define CAL_HIGHWAY_TABLE_MIN_LOAD     32u
#define CAL_HIGHWAY_TABLE_MAX_LOAD    144u
#define CAL_HIGHWAY_TABLE_POINTS        8u
static const bua_u8 cal_highway_spark[CAL_HIGHWAY_TABLE_POINTS] = {
    28u, 28u, 28u, 28u, 28u, 23u, 8u, 0u
};
#define CAL_HOT_RESTART_MIN_COOLANT   113u  /* LC02A */
#define CAL_HOT_RESTART_RAW            28u  /* LC02B */
#define CAL_HOT_RESTART_SECONDS        25u  /* LC684 */
#define MODE1_IAC_KICKDOWN_ENABLE    0x20u
/* Step 16 startup-spark calibration.  These four tables are addressed by
 * LF2C6 with 16-count coolant spacing.  The startup spark itself is stored
 * at L0013:L0014 as a 16-bit value whose high byte is later added to the
 * spark sum.  LF266 is a Q8-style 8x16 multiply; with C188=256 the
 * C18A table result is unchanged (for example 40 remains 40 injector events). */
#define CAL_STARTUP_SPARK_POINTS       14u
#define CAL_STARTUP_SPARK_MAX_COOL    208u
#define CAL_STARTUP_SPARK_DELAY_SCALE 256u  /* C188 */
#define STARTUP_SPARK_PHASE_BIT       0x80u /* L003B b7 */
#define SPARK_BLEND_BIT               0x20u /* L0036 b5 */
static const bua_u8 cal_startup_spark[CAL_STARTUP_SPARK_POINTS] = {
    28u,28u,28u,28u,28u,28u,28u,28u,28u,28u,28u,28u,28u,28u
};
static const bua_u8 cal_startup_spark_delay[CAL_STARTUP_SPARK_POINTS] = {
    100u,100u,100u,100u,100u,100u,100u,100u,100u,80u,40u,40u,40u,40u
};
static const bua_u8 cal_startup_spark_repeat[CAL_STARTUP_SPARK_POINTS] = {
    1u,1u,1u,1u,1u,1u,1u,1u,1u,1u,1u,1u,1u,1u
};
static const bua_u8 cal_startup_spark_decay[CAL_STARTUP_SPARK_POINTS] = {
    128u,128u,128u,128u,128u,128u,128u,128u,128u,128u,128u,128u,128u,255u
};
/* Step 17 knock-control calibration.  The executable uses L0056/4 for the
 * C1BE/C1C3 tables.  KNOCK_RETARD (L00A5) is in twice the raw spark-table
 * scale used for final subtraction: LD157 stores the value, then LSRA before
 * subtracting it from L009D. */
#define CAL_KNOCK_MIN_RPM12P5        52u   /* LC1B9 */
#define CAL_KNOCK_MIN_VSS             2u   /* LC1BA */
#define CAL_KNOCK_MAX_NORMAL        114u   /* LC1BB */
#define CAL_KNOCK_MIN_COOLANT       142u   /* LC1BC */
#define CAL_KNOCK_DELTA_COOLANT      53u   /* LC1BD */
#define CAL_KNOCK_FAIL_RETARD        45u   /* LC22A */
#define CAL_KNOCK_MIN_BATTERY        93u   /* LD116 literal */
#define KNOCK_DIFF_COOL_LATCH      0x80u   /* L0003 b7 */
#define KNOCK_ENABLED_FLAG         0x80u   /* L0040 b7 */
#define KNOCK_FAIL_MODE_BIT        0x80u   /* L0002 b7 */
#define POWER_ENRICH_ACTIVE        0x20u   /* L0043 b5 */
#define KNOCK_INHIBIT_AUX_BIT      0x20u   /* L003B b5 */
#define CAL_KNOCK_RATE_POINTS         5u
#define CAL_KNOCK_WOT_POINTS          8u
static const bua_u8 cal_knock_attack_rate[CAL_KNOCK_RATE_POINTS] = {
    7u, 8u, 13u, 16u, 16u
};
static const bua_u8 cal_knock_recovery_rate[CAL_KNOCK_RATE_POINTS] = {
    20u, 20u, 26u, 31u, 31u
};
static const bua_u8 cal_knock_wot_limit[CAL_KNOCK_WOT_POINTS] = {
    108u, 77u, 65u, 77u, 51u, 34u, 34u, 34u
};
static bua_u16 ram16be_get(bua_u16 a)
{
    return (bua_u16)(((bua_u16)RAM8(a) << 8) |
                     (bua_u16)RAM8((bua_u16)(a + 1u)));
}
static void ram16be_set(bua_u16 a, bua_u16 v)
{
    RAM8(a) = (bua_u8)(v >> 8);
    RAM8((bua_u16)(a + 1u)) = (bua_u8)v;
}
static bua_u16 mpu16be_get(bua_u16 a)
{
    bua_u16 off;
    off = (bua_u16)(a - (bua_u16)MPU_BASE_ADDR);
    return (bua_u16)(((bua_u16)mem.mpu[off] << 8) |
                     (bua_u16)mem.mpu[(bua_u16)(off + 1u)]);
}
static void mpu16be_set(bua_u16 a, bua_u16 v)
{
    bua_u16 off;
    off = (bua_u16)(a - (bua_u16)MPU_BASE_ADDR);
    mem.mpu[off] = (bua_u8)(v >> 8);
    mem.mpu[(bua_u16)(off + 1u)] = (bua_u8)v;
}
static bua_u8 hw_adc(bua_u8 channel)
{
    switch (channel & 0xF0u) {
        case 0x10u: return 128u;
        case 0x20u: return sim_o2_adc;
        case 0x40u: return 120u;
        case 0x50u: return sim_tps_adc;
        case 0x70u: return sim_diag_adc;
        case 0xA0u: return sim_maf_adc;
        default:    return 0u;
    }
}
static bua_u8 hw_timer8(void)
{
    sim_timer8 = (bua_u8)(sim_timer8 + 205u);
    return sim_timer8;
}
/*
 * Equivalent of the arithmetic performed by LF250 for the usages in this
 * port: old is 8.8 fixed point, new_value is an 8-bit whole-number sample,
 * and coef is scaled by 256.
 *
 * result = new_value * coef + round(old * (256 - coef) / 256)
 *
 * All arithmetic is intentionally integer-only.
 */
static bua_u16 bua_lag_filter_8_8(bua_u16 old_value,
                                  bua_u8 new_value,
                                  bua_u8 coef)
{
    bua_u32 p1;
    bua_u32 p2;
    p1 = (bua_u32)new_value * (bua_u32)coef;
    p2 = (bua_u32)old_value * (bua_u32)(256u - (bua_u16)coef);
    /* LF266 is a fixed-point 8 x 16 multiply returning product/256, rounded. */
    p2 = (p2 + 128ul) >> 8;
    return (bua_u16)(p1 + p2);
}
/*
 * HC11 FDIV-style fractional quotient needed by LF215.
 * For the normal VSS path numerator < denominator.
 */
static bua_u16 bua_fractional_divide(bua_u16 numerator, bua_u16 denominator)
{
    bua_u32 q;
    if (denominator == 0u)
        return 0xFFFFu;
    if (numerator >= denominator)
        return 0xFFFFu;
    q = ((bua_u32)numerator << 16) / (bua_u32)denominator;
    if (q > 0xFFFFul)
        q = 0xFFFFul;
    return (bua_u16)q;
}
/* ---------------------------------------------------------------------- */
/* Step 10: P4 distributor-reference input and original RPM arithmetic.     */
/* ---------------------------------------------------------------------- */
static void sim_set_ecm_reference_rpm(unsigned int rpm)
{
    sim_ecm_ref.frequency_hz = ((double)rpm *
                                (double)SIM_ECM_REF_PULSES_PER_REV) / 60.0;
}
static void sim_advance_ecm_reference_6p25ms(void)
{
    double pulses;
    double period_ticks_d;
    bua_u32 edge_tick;
    bua_u32 delta;
    sim_ecm_ref.pulse_since_irq = 0u;
    sim_ecm_ref.absolute_timer_ticks += 410ul; /* 6.25 ms * 65536 ~= 409.6 */
    if (sim_ecm_ref.frequency_hz <= 0.0)
        return;
    pulses = sim_ecm_ref.frequency_hz * 0.00625;
    sim_ecm_ref.phase += pulses;
    period_ticks_d = (double)SIM_P4_REF_TIMER_HZ / sim_ecm_ref.frequency_hz;
    while (sim_ecm_ref.phase >= 1.0) {
        /* Locate the edge within the current 6.25-ms interval from the
         * remaining phase.  PC-only double arithmetic schedules the input;
         * the ECM RPM calculation below remains integer/fixed-point. */
        double excess = sim_ecm_ref.phase - 1.0;
        double back_ticks = excess * period_ticks_d;
        if (back_ticks < 0.0)
            back_ticks = 0.0;
        if (back_ticks > 409.6)
            back_ticks = 409.6;
        edge_tick = sim_ecm_ref.absolute_timer_ticks - (bua_u32)(back_ticks + 0.5);
        if (sim_ecm_ref.last_edge_ticks != 0ul) {
            delta = edge_tick - sim_ecm_ref.last_edge_ticks;
            if (delta > 65535ul)
                delta = 65535ul;
            sim_ecm_ref.period_reg = (bua_u16)delta;
            sim_ecm_ref.period_valid = 1u;
            ram16be_set(REF_PERIOD_ADDR, sim_ecm_ref.period_reg);
        }
        sim_ecm_ref.last_edge_ticks = edge_tick;
        sim_ecm_ref.pulse_since_irq = 1u;
        sim_ecm_ref.injector_ref_phase =
            (bua_u8)(sim_ecm_ref.injector_ref_phase + 1u);
        if (sim_ecm_ref.injector_ref_phase >= 4u) {
            sim_ecm_ref.injector_ref_phase = 0u;
            if (sim_ecm_ref.injector_services_pending != 255u)
                ++sim_ecm_ref.injector_services_pending;
            ++stats.physical_injector_services;
        }
        MINOR_MODE_WORD2 |= REF_SEEN_BIT;
        ++stats.ref_pulses;
        sim_ecm_ref.phase -= 1.0;
    }
}
/* LCDE6 + LCDF6..LCE41, 8-cylinder path only (LC009 == 0).
 * The source doubles the measured reference period, performs LF215 FDIV
 * with numerator 307, then derives RPM/25 and filtered RPM/12.5. */
static void bua_calculate_rpm_from_reference(void)
{
    bua_u16 ref_period;
    bua_u16 doubled_period;
    bua_u16 q;
    bua_u16 d;
    bua_u8 rpm25;
    bua_u8 rpm12p5_new;
    bua_u16 filtered;
    ref_period = ram16be_get(REF_PERIOD_ADDR);
    if (ref_period == 0u || ref_period == 0xFFFFu) {
        RPM_SPECIAL = 0u;
        RPM_DIV25 = 0u;
        ram16be_set(0x0058u, 0u);
        return;
    }
    doubled_period = (bua_u16)(ref_period << 1);
    if (doubled_period == 0u || doubled_period < ref_period)
        q = 0xFFFFu;
    else
        q = bua_fractional_divide(307u, doubled_period);
    /* LCE1B..LCE28: round Q and store high byte as RPM/25. */
    d = q;
    if ((bua_u16)(d + 128u) < d)
        rpm25 = 255u;
    else {
        d = (bua_u16)(d + 128u);
        rpm25 = (bua_u8)(d >> 8);
    }
    RPM_DIV25 = rpm25;
    /* LCE28..LCE41: Q*2, rounded, saturated, then LF250 with LC62E=128.
     * The existing lag helper is the same provisional LF250 translation used
     * elsewhere in this port; bit-exact utility verification remains pending. */
    if ((q & 0x8000u) != 0u) {
        rpm12p5_new = 255u;
    } else {
        d = (bua_u16)(q << 1);
        if ((bua_u16)(d + 128u) < d)
            rpm12p5_new = 255u;
        else {
            d = (bua_u16)(d + 128u);
            rpm12p5_new = (bua_u8)(d >> 8);
        }
    }
    if (ram16be_get(0x0058u) == 0u)
        filtered = (bua_u16)((bua_u16)rpm12p5_new << 8);
    else
        filtered = bua_lag_filter_8_8(ram16be_get(0x0058u), rpm12p5_new, (bua_u8)CAL_RPM_FILTER_COEF);
    ram16be_set(0x0058u, filtered);
    /* LCE0D..LCE1B: exact L0056 transform.  Its semantic purpose remains
     * provisional; this preserves the instruction-level arithmetic. */
    d = q;
    if ((bua_u8)(d >> 8) > 96u) {
        bua_u16 before;
        before = d;
        d = (bua_u16)(d + 16512u);
        if (d < before)
            RPM_SPECIAL = 255u;
        else
            RPM_SPECIAL = (bua_u8)(d >> 8);
    } else {
        d = (bua_u16)(d << 1);
        if (d < 8064u)
            RPM_SPECIAL = 0u;
        else {
            d = (bua_u16)(d - 8064u);
            RPM_SPECIAL = (bua_u8)(d >> 8);
        }
    }
    ++stats.rpm_calculations;
}
/* ---------------------------------------------------------------------- */
/* Step 13: LF27C / C02E main spark lookup.                               */
/*                                                                        */
/* LF2D6 interpolates two unsigned bytes using an 8-bit fraction.  LF27C */
/* creates that fraction from the low byte of argument*16, so each axis  */
/* has 16 input counts per table interval.  The assembly rounds the      */
/* magnitude at bit 7 of the product's low byte.                          */
/* ---------------------------------------------------------------------- */
static bua_u8 bua_interp_u8_lf2d6(bua_u8 first, bua_u8 second,
                                   bua_u8 fraction)
{
    unsigned int diff;
    unsigned int product;
    unsigned int delta;
    if (first == second || fraction == 0u)
        return first;
    if (second > first) {
        diff = (unsigned int)second - (unsigned int)first;
        product = diff * (unsigned int)fraction;
        delta = (product >> 8) + ((product & 0x80u) != 0u ? 1u : 0u);
        return (bua_u8)((unsigned int)first + delta);
    }
    diff = (unsigned int)first - (unsigned int)second;
    product = diff * (unsigned int)fraction;
    delta = (product >> 8) + ((product & 0x80u) != 0u ? 1u : 0u);
    return (bua_u8)((unsigned int)first - delta);
}
static bua_u8 bua_lookup_main_spark_raw(bua_u8 rpm_arg, bua_u8 load_arg)
{
    bua_u8 row_off;
    bua_u8 col_off;
    bua_u8 row_index;
    bua_u8 col_index;
    bua_u8 row_frac;
    bua_u8 col_frac;
    bua_u8 q0;
    bua_u8 q1;
    bua_u8 c00;
    bua_u8 c01;
    bua_u8 c10;
    bua_u8 c11;
    /* C02E row minimum is zero in this calibration. */
    row_off = (bua_u8)(rpm_arg - (bua_u8)CAL_MAIN_SPARK_MIN_RPM_ARG);
    if (load_arg < (bua_u8)CAL_MAIN_SPARK_MIN_LOAD)
        col_off = 0u;
    else
        col_off = (bua_u8)(load_arg - (bua_u8)CAL_MAIN_SPARK_MIN_LOAD);
    row_index = (bua_u8)(row_off >> 4);
    col_index = (bua_u8)(col_off >> 4);
    row_frac = (bua_u8)(row_off << 4);
    col_frac = (bua_u8)(col_off << 4);
    /* Caller ranges in BUA keep row_index <=15 and col_index <=11. */
    if (row_index >= (bua_u8)(CAL_MAIN_SPARK_ROWS - 1u)) {
        row_index = (bua_u8)(CAL_MAIN_SPARK_ROWS - 2u);
        row_frac = 255u;
    }
    if (col_index >= (bua_u8)(CAL_MAIN_SPARK_COLS - 1u)) {
        col_index = (bua_u8)(CAL_MAIN_SPARK_COLS - 1u);
        col_frac = 0u;
    }
    c00 = cal_main_spark[row_index][col_index];
    c10 = cal_main_spark[(bua_u8)(row_index + 1u)][col_index];
    if (col_frac == 0u || col_index == (bua_u8)(CAL_MAIN_SPARK_COLS - 1u)) {
        q0 = c00;
        q1 = c10;
    } else {
        c01 = cal_main_spark[row_index][(bua_u8)(col_index + 1u)];
        c11 = cal_main_spark[(bua_u8)(row_index + 1u)][(bua_u8)(col_index + 1u)];
        q0 = bua_interp_u8_lf2d6(c00, c01, col_frac);
        q1 = bua_interp_u8_lf2d6(c10, c11, col_frac);
    }
    ++stats.main_spark_lookups;
    return bua_interp_u8_lf2d6(q0, q1, row_frac);
}
static double bua_spark_raw_to_degrees(bua_u8 raw)
{
    /* Calibration comment: table = spark_degrees * 256 / 90. */
    return ((double)raw * 90.0) / 256.0;
}
static double bua_spark_signed_raw_to_degrees(bua_s16 raw)
{
    return ((double)raw * 90.0) / 256.0;
}
static bua_u8 bua_lookup_coolant_spark_raw(bua_u8 coolant_arg, bua_u8 load_arg)
{
    bua_u8 row_off;
    bua_u8 col_off;
    bua_u8 row_index;
    bua_u8 col_index;
    bua_u8 row_frac;
    bua_u8 col_frac;
    bua_u8 q0;
    bua_u8 q1;
    bua_u8 c00;
    bua_u8 c01;
    bua_u8 c10;
    bua_u8 c11;
    if (coolant_arg > 208u)
        coolant_arg = 208u;
    /* C0FE=0 selects filtered load (LV8).  LCFD4 limits this path to 160. */
    if (load_arg > 160u)
        load_arg = 160u;
    if (coolant_arg < (bua_u8)CAL_COOL_SPARK_MIN_COOL)
        row_off = 0u;
    else
        row_off = (bua_u8)(coolant_arg - (bua_u8)CAL_COOL_SPARK_MIN_COOL);
    if (load_arg < (bua_u8)CAL_COOL_SPARK_MIN_LOAD)
        col_off = 0u;
    else
        col_off = (bua_u8)(load_arg - (bua_u8)CAL_COOL_SPARK_MIN_LOAD);
    row_index = (bua_u8)(row_off >> 4);
    col_index = (bua_u8)(col_off >> 4);
    row_frac = (bua_u8)(row_off << 4);
    col_frac = (bua_u8)(col_off << 4);
    /* A=208 is exactly row 11, so unlike C02E no 13th row is required. */
    if (row_index >= (bua_u8)(CAL_COOL_SPARK_ROWS - 1u)) {
        row_index = (bua_u8)(CAL_COOL_SPARK_ROWS - 1u);
        row_frac = 0u;
    }
    if (col_index >= (bua_u8)(CAL_COOL_SPARK_COLS - 1u)) {
        col_index = (bua_u8)(CAL_COOL_SPARK_COLS - 1u);
        col_frac = 0u;
    }
    c00 = cal_coolant_spark[row_index][col_index];
    if (row_frac == 0u || row_index == (bua_u8)(CAL_COOL_SPARK_ROWS - 1u)) {
        if (col_frac == 0u || col_index == (bua_u8)(CAL_COOL_SPARK_COLS - 1u))
            q0 = c00;
        else {
            c01 = cal_coolant_spark[row_index][(bua_u8)(col_index + 1u)];
            q0 = bua_interp_u8_lf2d6(c00, c01, col_frac);
        }
        ++stats.coolant_spark_lookups;
        return q0;
    }
    c10 = cal_coolant_spark[(bua_u8)(row_index + 1u)][col_index];
    if (col_frac == 0u || col_index == (bua_u8)(CAL_COOL_SPARK_COLS - 1u)) {
        q0 = c00;
        q1 = c10;
    } else {
        c01 = cal_coolant_spark[row_index][(bua_u8)(col_index + 1u)];
        c11 = cal_coolant_spark[(bua_u8)(row_index + 1u)][(bua_u8)(col_index + 1u)];
        q0 = bua_interp_u8_lf2d6(c00, c01, col_frac);
        q1 = bua_interp_u8_lf2d6(c10, c11, col_frac);
    }
    ++stats.coolant_spark_lookups;
    return bua_interp_u8_lf2d6(q0, q1, row_frac);
}
static bua_s16 bua_main_plus_coolant_raw(bua_u8 rpm_arg, bua_u8 load_arg,
                                          bua_u8 coolant_arg,
                                          bua_u8 *main_raw_out,
                                          bua_u8 *cool_raw_out)
{
    bua_u8 main_raw;
    bua_u8 cool_raw;
    bua_u8 main_load;
    main_load = load_arg;
    if (main_load > 208u)
        main_load = 208u;
    main_raw = bua_lookup_main_spark_raw(rpm_arg, main_load);
    cool_raw = bua_lookup_coolant_spark_raw(coolant_arg, load_arg);
    if (main_raw_out != 0)
        *main_raw_out = main_raw;
    if (cool_raw_out != 0)
        *cool_raw_out = cool_raw;
    ++stats.partial_spark_calculations;
    return (bua_s16)((bua_s16)main_raw + (bua_s16)cool_raw -
                     (bua_s16)CAL_COOL_SPARK_BIAS_RAW);
}
/* ---------------------------------------------------------------------- */
/* Step 15: highway-mode spark and hot-restart retard.                    */
/* ---------------------------------------------------------------------- */
static bua_u8 bua_lookup_highway_spark_raw(bua_u8 load_arg)
{
    bua_u8 off;
    bua_u8 index;
    bua_u8 frac;
    if (load_arg < (bua_u8)CAL_HIGHWAY_TABLE_MIN_LOAD)
        load_arg = (bua_u8)CAL_HIGHWAY_TABLE_MIN_LOAD;
    if (load_arg > (bua_u8)CAL_HIGHWAY_TABLE_MAX_LOAD)
        load_arg = (bua_u8)CAL_HIGHWAY_TABLE_MAX_LOAD;
    off = (bua_u8)(load_arg - (bua_u8)CAL_HIGHWAY_TABLE_MIN_LOAD);
    index = (bua_u8)(off >> 4);
    frac = (bua_u8)(off << 4);
    ++stats.highway_spark_lookups;
    if (index >= (bua_u8)(CAL_HIGHWAY_TABLE_POINTS - 1u))
        return cal_highway_spark[CAL_HIGHWAY_TABLE_POINTS - 1u];
    return bua_interp_u8_lf2d6(cal_highway_spark[index],
                                cal_highway_spark[(bua_u8)(index + 1u)],
                                frac);
}
/* Literal LCFF4..LD02B behavior for the BUA calibration LC017=$00.
 * Timer is passed explicitly so regression tests need not disturb ECM RAM. */
static bua_u8 bua_highway_spark_eval(bua_u8 load_arg, bua_u8 coolant_arg,
                                     bua_u8 rpm_special, bua_u8 minor_count,
                                     bua_u8 timer_in, bua_u8 *timer_out)
{
    bua_u8 timer;
    timer = timer_in;
    if (load_arg > (bua_u8)CAL_HIGHWAY_MAX_LOAD ||
        coolant_arg <= (bua_u8)CAL_HIGHWAY_MIN_COOLANT ||
        rpm_special < (bua_u8)CAL_HIGHWAY_MIN_RPM_SPECIAL) {
        timer = 0u;
        if (timer_out != 0)
            *timer_out = timer;
        return 0u;
    }
    if (timer > (bua_u8)CAL_HIGHWAY_DELAY) {
        if (timer_out != 0)
            *timer_out = timer;
        return bua_lookup_highway_spark_raw(load_arg);
    }
    if (minor_count == 0u)
        timer = (bua_u8)(timer + 1u);
    if (timer_out != 0)
        *timer_out = timer;
    return 0u;
}
/* LD04D/LD054 hot-restart decision.  The original uses the low byte of the
 * engine-run-seconds word (L001B) and sets L0001 b5 when the retard expires
 * or is not applicable.  Return value is the raw spark amount to subtract. */
static bua_u8 bua_hot_restart_retard_eval(bua_u8 mode_word1,
                                          bua_u8 startup_coolant,
                                          bua_u8 run_seconds_low,
                                          bua_u8 *mode_word1_out)
{
    bua_u8 result;
    bua_u8 mode;
    ++stats.hot_restart_evaluations;
    mode = mode_word1;
    result = 0u;
    if ((mode & MODE1_IAC_KICKDOWN_ENABLE) == 0u &&
        startup_coolant >= (bua_u8)CAL_HOT_RESTART_MIN_COOLANT &&
        run_seconds_low < (bua_u8)CAL_HOT_RESTART_SECONDS) {
        result = (bua_u8)CAL_HOT_RESTART_RAW;
    } else {
        mode |= MODE1_IAC_KICKDOWN_ENABLE;
    }
    if (mode_word1_out != 0)
        *mode_word1_out = mode;
    return result;
}
static bua_u16 bua_mul_u16_u16_lf2f0(bua_u16 multiplier,
                                      bua_u16 multiplicand,
                                      bua_u16 *upper_out);
static bua_u8 bua_lookup_spacing16(const bua_u8 *table, bua_u8 points,
                                   bua_u8 arg);
/* ---------------------------------------------------------------------- */
/* Step 25: AFR generation, open loop and power enrichment.               */
/* ---------------------------------------------------------------------- */
typedef struct BuaAfrResultTag {
    bua_u16 afr_code;
    bua_u16 factor;
    bua_u8 load_change;
    bua_u8 pe_active;
    bua_u8 single_fire_cleared;
} BuaAfrResult;
static bua_u8 bua_lookup_spacing32(const bua_u8 *table, bua_u8 points,
                                   bua_u8 arg)
{
    bua_u8 index;
    bua_u8 frac;
    unsigned int base;
    unsigned int next;
    unsigned int value;
    if (points == 0u)
        return 0u;
    if (points == 1u)
        return table[0];
    index = (bua_u8)(arg >> 5);
    frac = (bua_u8)(arg & 31u);
    if (index >= (bua_u8)(points - 1u))
        return table[points - 1u];
    base = (unsigned int)table[index];
    next = (unsigned int)table[(bua_u8)(index + 1u)];
    if (next >= base)
        value = base + (((next - base) * (unsigned int)frac + 16u) >> 5);
    else
        value = base - (((base - next) * (unsigned int)frac + 16u) >> 5);
    return (bua_u8)value;
}
static bua_u16 bua_afr_from_factor_lf2f0(bua_u16 factor)
{
    bua_u16 upper;
    return bua_mul_u16_u16_lf2f0(CAL_STOICH_CODE, factor, &upper);
}
static BuaAfrResult bua_afr_lda2d(bua_u8 startup_enrich_hi,
                                 bua_u8 af_mode_word2,
                                 bua_u8 coolant_afr_change,
                                 bua_u8 filtered_load)
{
    BuaAfrResult r;
    unsigned int factor;
    bua_u8 load_change;
    r.pe_active = 0u;
    r.single_fire_cleared = 0u;
    load_change = 0u;
    if ((af_mode_word2 & AF_CLOSED_LOOP_BIT) != 0u) {
        factor = 256u + (unsigned int)startup_enrich_hi;
    } else {
        load_change = bua_lookup_spacing16(cal_open_loop_load_afr, 17u,
                                           filtered_load);
        factor = (unsigned int)startup_enrich_hi +
                 (unsigned int)CAL_OPEN_LOOP_AFR_BIAS +
                 (unsigned int)coolant_afr_change +
                 (unsigned int)load_change;
        if (factor > 65535u)
            factor = 65535u;
    }
    r.factor = (bua_u16)factor;
    r.load_change = load_change;
    r.afr_code = bua_afr_from_factor_lf2f0(r.factor);
    return r;
}
static BuaAfrResult bua_afr_power_enrich_ld9d7(bua_u8 rpm_div25,
                                               bua_u8 filtered_load,
                                               bua_u8 tps_axis,
                                               bua_u8 coolant_arg,
                                               bua_u8 pe_was_active)
{
    BuaAfrResult r;
    unsigned int tps_threshold;
    unsigned int load_threshold;
    bua_u8 coolant_change;
    bua_u8 rpm_change;
    unsigned int factor;
    (void)rpm_div25;
    r.afr_code = 0u;
    r.factor = 0u;
    r.load_change = 0u;
    r.pe_active = 0u;
    r.single_fire_cleared = 0u;
    tps_threshold = (unsigned int)CAL_PE_TPS_THRESHOLD;
    if (pe_was_active != 0u) {
        if (tps_threshold >= (unsigned int)CAL_PE_TPS_HYST)
            tps_threshold -= (unsigned int)CAL_PE_TPS_HYST;
        else
            tps_threshold = 0u;
    }
    if ((unsigned int)tps_axis < tps_threshold)
        return r;
    load_threshold = (unsigned int)CAL_PE_LOAD_ENABLE;
    if (pe_was_active != 0u) {
        if (load_threshold >= (unsigned int)CAL_PE_LOAD_HYST)
            load_threshold -= (unsigned int)CAL_PE_LOAD_HYST;
        else
            load_threshold = 0u;
    }
    if ((unsigned int)filtered_load < load_threshold)
        return r;
    coolant_change = bua_lookup_spacing32(cal_pe_coolant_change, 9u,
                                          coolant_arg);
    rpm_change = cal_pe_rpm_change[0];
    factor = 128u + (unsigned int)rpm_change + (unsigned int)coolant_change;
    r.pe_active = 1u;
    r.single_fire_cleared = 1u;
    r.factor = (bua_u16)factor;
    r.afr_code = bua_afr_from_factor_lf2f0(r.factor);
    return r;
}
static double bua_afr_code_to_ratio(bua_u16 code)
{
    if (code == 0u)
        return 0.0;
    return 6553.6 / (double)code;
}
/* ---------------------------------------------------------------------- */
/* Step 47: integrated AFR selection / PE / Mode-4 interaction.           */
/*                                                                        */
/* Executable order at LD9A9..LDA5A is significant: Mode 4 AFR override  */
/* has first priority and bypasses both PE and normal AFR construction.   */
/* Otherwise LD9D7 tests PE; failure falls through LDA2D.  DFCO does not  */
/* participate in AFR selection here -- LF92A later forces fuel to zero.  */
/* ---------------------------------------------------------------------- */
#define STEP47_MODE4_ACTIVE_BIT           0x08u  /* L0035 b3 */
#define STEP47_MODE4_AFR_ENABLE_BIT       0x04u  /* L0158 b2 */
typedef struct BuaAfrSelectResultTag {
    BuaAfrResult afr;
    bua_u8 af_mode_word;
    bua_u8 single_fire;
    bua_u8 source;       /* 0 normal LDA2D, 1 PE, 2 Mode 4 */
} BuaAfrSelectResult;
static bua_u16 bua_mode4_afr_code_step47(bua_u8 aldl_afr_value)
{
    if (aldl_afr_value == 0u)
        return 0xFFFFu;
    return (bua_u16)(65535u / (unsigned int)aldl_afr_value);
}
static BuaAfrSelectResult bua_select_afr_step47(bua_u8 rpm_div25,
                                                 bua_u8 filtered_load,
                                                 bua_u8 tps_axis,
                                                 bua_u8 coolant_arg,
                                                 bua_u8 startup_enrich_hi,
                                                 bua_u8 af_mode_word2,
                                                 bua_u8 coolant_afr_change,
                                                 bua_u8 af_mode_word,
                                                 bua_u8 single_fire,
                                                 bua_u8 mode4_status,
                                                 bua_u8 mode4_enable,
                                                 bua_u8 mode4_afr_value)
{
    BuaAfrSelectResult r;
    BuaAfrResult pe;
    bua_u8 pe_was_active;
    r.af_mode_word = af_mode_word;
    r.single_fire = single_fire;
    r.source = 0u;
    if (((mode4_status & STEP47_MODE4_ACTIVE_BIT) != 0u) &&
        ((mode4_enable & STEP47_MODE4_AFR_ENABLE_BIT) != 0u)) {
        r.afr.afr_code = bua_mode4_afr_code_step47(mode4_afr_value);
        r.afr.factor = 0u;
        r.afr.load_change = 0u;
        r.afr.pe_active = (bua_u8)(((af_mode_word & AF_POWER_ENRICH_BIT) != 0u) ? 1u : 0u);
        r.afr.single_fire_cleared = 0u;
        r.source = 2u;
        return r;
    }
    pe_was_active = (bua_u8)(((af_mode_word & AF_POWER_ENRICH_BIT) != 0u) ? 1u : 0u);
    pe = bua_afr_power_enrich_ld9d7(rpm_div25, filtered_load, tps_axis,
                                    coolant_arg, pe_was_active);
    /* LD9ED clears PE before testing the current qualifiers. */
    r.af_mode_word = (bua_u8)(r.af_mode_word & (bua_u8)~AF_POWER_ENRICH_BIT);
    if (pe.pe_active != 0u) {
        r.af_mode_word = (bua_u8)(r.af_mode_word | AF_POWER_ENRICH_BIT);
        r.single_fire = 0u; /* BCLR L0039,$80 */
        r.afr = pe;
        r.source = 1u;
        return r;
    }
    r.afr = bua_afr_lda2d(startup_enrich_hi, af_mode_word2,
                          coolant_afr_change, filtered_load);
    r.source = 0u;
    return r;
}
/* ---------------------------------------------------------------------- */
/* Step 16: startup spark initialization and per-inject decay.             */
/* ---------------------------------------------------------------------- */
static bua_u8 bua_lookup_fixed16_table(const bua_u8 *table,
                                       bua_u8 points,
                                       bua_u8 argument)
{
    bua_u8 index;
    bua_u8 fraction;
    if (argument > (bua_u8)CAL_STARTUP_SPARK_MAX_COOL)
        argument = (bua_u8)CAL_STARTUP_SPARK_MAX_COOL;
    index = (bua_u8)(argument >> 4);
    fraction = (bua_u8)(argument << 4);
    if (index >= (bua_u8)(points - 1u))
        return table[(bua_u8)(points - 1u)];
    return bua_interp_u8_lf2d6(table[index], table[(bua_u8)(index + 1u)],
                               fraction);
}
/* Literal arithmetic result of LF266.  The routine multiplies an 8-bit
 * factor by a 16-bit value and returns the product scaled back by 256,
 * with the low partial product rounded from bit 7.  Algebraically this is
 * round((factor * value) / 256) for the unsigned ranges used here. */
static bua_u16 bua_mul_u8_u16_lf266(bua_u8 factor, bua_u16 value)
{
    bua_u32 product;
    product = (bua_u32)factor * (bua_u32)value;
    product += 128ul;
    return (bua_u16)(product >> 8);
}
/* ---------------------------------------------------------------------- */
/* Step 22: LF2F0 and base injector pulse width (LDD2D..LDD60).            */
/*                                                                        */
/* LF2F0 forms a 32-bit unsigned product.  The executable returns the     */
/* middle two bytes (product >> 8) in D and the upper two bytes           */
/* (product >> 16) in X.  If the top product byte is nonzero, D is        */
/* saturated to FFFF.  Several callers use X as well, so return it here.  */
/* ---------------------------------------------------------------------- */
static bua_u16 bua_mul_u16_u16_lf2f0(bua_u16 multiplier,
                                      bua_u16 multiplicand,
                                      bua_u16 *upper_out)
{
    bua_u32 product;
    bua_u16 middle;
    product = (bua_u32)multiplier * (bua_u32)multiplicand;
    if (upper_out != 0)
        *upper_out = (bua_u16)(product >> 16);
    middle = (bua_u16)(product >> 8);
    if ((product & 0xFF000000ul) != 0ul)
        middle = 0xFFFFu;
    ++stats.lf2f0_calls;
    return middle;
}
/* C42D uses the same 16-count fixed spacing on both axes as LF27C. */
static bua_u8 bua_lookup_base_fuel_c42d(bua_u8 rpm_arg, bua_u8 load_arg)
{
    bua_u8 row_index;
    bua_u8 col_index;
    bua_u8 row_frac;
    bua_u8 col_frac;
    bua_u8 q0;
    bua_u8 q1;
    row_index = (bua_u8)(rpm_arg >> 4);
    row_frac = (bua_u8)(rpm_arg << 4);
    col_index = (bua_u8)(load_arg >> 4);
    col_frac = (bua_u8)(load_arg << 4);
    if (row_index >= (bua_u8)(CAL_BASE_FUEL_ROWS - 1u)) {
        row_index = (bua_u8)(CAL_BASE_FUEL_ROWS - 1u);
        row_frac = 0u;
    }
    if (col_index >= (bua_u8)(CAL_BASE_FUEL_COLS - 1u)) {
        col_index = (bua_u8)(CAL_BASE_FUEL_COLS - 1u);
        col_frac = 0u;
    }
    if (col_frac == 0u || col_index == (bua_u8)(CAL_BASE_FUEL_COLS - 1u)) {
        q0 = cal_base_fuel[row_index][col_index];
        if (row_index == (bua_u8)(CAL_BASE_FUEL_ROWS - 1u))
            q1 = q0;
        else
            q1 = cal_base_fuel[(bua_u8)(row_index + 1u)][col_index];
    } else {
        q0 = bua_interp_u8_lf2d6(cal_base_fuel[row_index][col_index],
                                  cal_base_fuel[row_index][(bua_u8)(col_index + 1u)],
                                  col_frac);
        if (row_index == (bua_u8)(CAL_BASE_FUEL_ROWS - 1u))
            q1 = q0;
        else
            q1 = bua_interp_u8_lf2d6(cal_base_fuel[(bua_u8)(row_index + 1u)][col_index],
                                      cal_base_fuel[(bua_u8)(row_index + 1u)][(bua_u8)(col_index + 1u)],
                                      col_frac);
    }
    ++stats.fuel_base_pw_table_lookups;
    return bua_interp_u8_lf2d6(q0, q1, row_frac);
}
static bua_u16 bua_base_pw_table_path(bua_u8 rpm_arg, bua_u8 load_arg,
                                      bua_u16 total_afr)
{
    bua_u16 scale;
    bua_u8 table_raw;
    scale = bua_mul_u16_u16_lf2f0((bua_u16)CAL_BASE_TABLE_MULT,
                                   total_afr, 0);
    table_raw = bua_lookup_base_fuel_c42d(rpm_arg, load_arg);
    return bua_mul_u8_u16_lf266(table_raw, scale);
}
/* Literal calculated-BPW chain selected by LC016 b6 in this image.
 * L00EA is the original 16-bit airflow word.  Step 30 establishes from
 * the executable/calibration cross-check that its physical scaling is Q8.8
 * grams/second (g/s * 256).  The last ROLB/PSHX/PULA/PULB/ROLB/ROLA
 * sequence is reconstructed from the high and middle product words. */
static bua_u16 bua_base_pw_calculated_path(bua_u16 airflow_word,
                                           bua_u16 ref_period,
                                           bua_u16 total_afr,
                                           bua_u8 single_fire)
{
    bua_u16 d;
    bua_u16 upper;
    bua_u16 inj_flow;
    bua_u16 carry_from_middle_bit7;
    d = bua_mul_u16_u16_lf2f0(airflow_word, ref_period, &upper);
    d = (bua_u16)(d >> 1);
    d = bua_mul_u16_u16_lf2f0(d, total_afr, &upper);
    inj_flow = single_fire ? (bua_u16)CAL_INJ_FLOW_SINGLE
                           : (bua_u16)CAL_INJ_FLOW_DOUBLE;
    d = bua_mul_u16_u16_lf2f0(d, inj_flow, &upper);
    /* First ROLB exports bit 7 of the middle-product low byte as carry.
     * Loading X into D and rotating D left through that carry is exactly
     * equivalent to taking product >> 15 for the non-overflowing result. */
    carry_from_middle_bit7 = (d & 0x0080u) ? 1u : 0u;
    return (bua_u16)((bua_u16)(upper << 1) | carry_from_middle_bit7);
}
static bua_u16 bua_base_pw_ldd2d(void)
{
    bua_u16 ref_period;
    bua_u16 total_afr;
    bua_u16 result;
    bua_u8 single_fire;
    ref_period = ram16be_get(REF_PERIOD_ADDR);
    total_afr = ram16be_get(TOTAL_AFR_ADDR);
    single_fire = (INJECT_MODE_WORD & INJECT_SINGLE_FIRE_BIT) ? 1u : 0u;
    /* ASLA on LC016 tests original b6 via the sign flag.  LC016=$62 has
     * b6 set, therefore the actual image uses the calculated path. */
    if ((CAL_AF_MODE3 & CAL_BASE_PW_CALC_BIT) != 0u) {
        if ((ENGINE_MODE_WORD & ENGINE_RUNNING_BIT) == 0u)
            ref_period = (bua_u16)CAL_CRANK_REF_LIMIT;
        result = bua_base_pw_calculated_path(ram16be_get(AIRFLOW_WORD_ADDR),
                                             ref_period, total_afr, single_fire);
    } else {
        result = bua_base_pw_table_path(RPM_DIV25, FILTERED_LOAD, total_afr);
    }
    ++stats.fuel_base_pw_calculations;
    return result;
}
/* ---------------------------------------------------------------------- */
/* Step 43: calibrated cranking fuel construction, LD7B1..LD7F0.          */
/*                                                                        */
/* LF2C6 uses 16 raw argument counts per table interval.  The coolant     */
/* table is clamped at raw 208 (115 C) before lookup.  C37F is indexed by */
/* L0118*2; in this image every entry is 255, so it is essentially unity  */
/* under LF266.  C390 is the notable clear-flood table: it rises with TPS */
/* through 75%, then becomes zero at 87.5% and 100%.                      */
/*                                                                        */
/* The following C35B RPM multiplier table is all zero in this image.     */
/* Therefore the LD82E post-crank blend path is calibration-disabled; the */
/* executable cranking path proceeds from this constructed PW to LF92A.  */
/* ---------------------------------------------------------------------- */
static const bua_u8 cal_crank_rpm_mult[17] = {
    0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u
};
static const bua_u8 cal_crank_coolant_pw[14] = {
    230u,205u,77u,51u,33u,23u,20u,19u,18u,17u,14u,12u,16u,20u
};
static const bua_u8 cal_crank_drp_mult[17] = {
    255u,255u,255u,255u,255u,255u,255u,255u,255u,
    255u,255u,255u,255u,255u,255u,255u,255u
};
static const bua_u8 cal_crank_tps_mult[9] = {
    64u,81u,81u,89u,97u,106u,115u,0u,0u
};
static bua_u8 bua_lookup_fixed16_unclamped_step43(const bua_u8 *table,
                                                   bua_u8 points,
                                                   bua_u8 argument)
{
    bua_u8 index;
    bua_u8 fraction;
    index=(bua_u8)(argument>>4);
    fraction=(bua_u8)(argument<<4);
    if (index >= (bua_u8)(points-1u))
        return table[(bua_u8)(points-1u)];
    return bua_interp_u8_lf2d6(table[index],table[(bua_u8)(index+1u)],fraction);
}
static bua_u8 bua_crank_tps_mult_step43(bua_u8 tps_axis)
{
    /* C390 is a normal 9-point, 32-count-spacing LF2BF table. */
    return bua_lookup_spacing32(cal_crank_tps_mult,9u,tps_axis);
}
static bua_u16 bua_crank_pw_step43(bua_u8 coolant,
                                    bua_u8 tps_axis,
                                    bua_u8 drp_count)
{
    bua_u8 cool_arg;
    bua_u8 cool_raw;
    bua_u8 drp_raw;
    bua_u8 tps_raw;
    bua_u16 pw;
    cool_arg=coolant;
    if (cool_arg > (bua_u8)CAL_CRANK_COOLANT_MAX)
        cool_arg=(bua_u8)CAL_CRANK_COOLANT_MAX;
    cool_raw=bua_lookup_fixed16_unclamped_step43(cal_crank_coolant_pw,14u,cool_arg);
    pw=bua_mul_u8_u16_lf266(cool_raw,(bua_u16)CAL_CRANK_PW_SCALE);
    /* L0118 is doubled before C37F lookup. */
    drp_raw=bua_lookup_fixed16_unclamped_step43(cal_crank_drp_mult,17u,
                                                (bua_u8)(drp_count<<1));
    pw=bua_mul_u8_u16_lf266(drp_raw,pw);
    /* LF472 uses factor/128; C390 is calibrated as multiplier*64, and the
       subsequent ROLD doubles the result.  Saturate exactly at 16 bits. */
    tps_raw=bua_crank_tps_mult_step43(tps_axis);
    {
        bua_u32 scaled;
        scaled=((bua_u32)tps_raw*(bua_u32)pw)>>7;
        if (scaled > 32767ul)
            return 65535u;
        return (bua_u16)((bua_u16)scaled<<1);
    }
}
static int bua_crank_rpm_blend_enabled_step43(void)
{
    unsigned int i;
    for (i=0u;i<17u;++i) {
        if (cal_crank_rpm_mult[i] != 0u)
            return 1;
    }
    return 0;
}
/* ---------------------------------------------------------------------- */
/* Step 44: crank-to-run qualification and immediate fuel handoff.        */
/*                                                                        */
/* LCD4B..LCD8F declares ENGINE RUNNING only after:                       */
/*   - reference period is faster than LC018,                             */
/*   - a second-reference-valid latch has first been established, and     */
/*   - LC01A=8 further qualified reference events have accumulated.       */
/* The compare occurs before the increment, so a fresh state reaches      */
/* ENGINE RUNNING on the tenth qualifying reference event.                */
/*                                                                        */
/* LCADx also maintains L0118 during the cold-start interval.  It clears  */
/* after engine run time exceeds LC37C=32 or TPS exceeds LC37E=197;       */
/* otherwise it increments on the qualifying reference event and stops   */
/* at signed-positive 127 because the source refuses to store 0x80.       */
/*                                                                        */
/* At LD7A0, LC35A=0 means an asserted ENGINE RUNNING flag immediately    */
/* jumps to LD86B.  Therefore this calibration has no post-catch crank-   */
/* fuel dwell: crank fuel is used while not-running, normal running fuel  */
/* is selected on the first fuel-path evaluation after b7 becomes set.    */
/* ---------------------------------------------------------------------- */
typedef struct {
    bua_u8 second_ref_valid;
    bua_u8 startup_counter;
    bua_u8 engine_running;
} BuaRunQual44;
typedef enum {
    BUA_FUEL_PATH_CRANK44 = 0,
    BUA_FUEL_PATH_NORMAL44 = 1
} BuaFuelPath44;
static void bua_run_qual_ref_event_step44(BuaRunQual44 *q,
                                           bua_u16 ref_period)
{
    if (q->engine_running != 0u)
        return;
    if (ref_period >= (bua_u16)CAL_STARTUP_REF_PERIOD) {
        q->startup_counter=0u;
        return;
    }
    if (q->second_ref_valid == 0u) {
        q->second_ref_valid=1u;
        return;
    }
    if (q->startup_counter < (bua_u8)CAL_STARTUP_LOOPS) {
        q->startup_counter=(bua_u8)(q->startup_counter+1u);
        return;
    }
    q->engine_running=1u;
}
static bua_u8 bua_cold_start_ref_count_step44(bua_u8 old_count,
                                               bua_u16 engine_run_seconds,
                                               bua_u8 tps_axis)
{
    bua_u8 n;
    if (engine_run_seconds > (bua_u16)CAL_COLD_START_RUN_TIME)
        return 0u;
    if (tps_axis > (bua_u8)CAL_COLD_START_TPS_LIMIT)
        return 0u;
    n=(bua_u8)(old_count+1u);
    /* INCB followed by BPL: 0x7F -> 0x80 is not stored. */
    if ((n & 0x80u) != 0u)
        return old_count;
    return n;
}
static BuaFuelPath44 bua_select_fuel_path_core_step44(bua_u8 engine_mode_word,
                                                       bua_u8 ff_counter,
                                                       bua_u8 skip_count)
{
    /* LD7A0: CMPA LC35A / BCS LD7B1 / BRCLR ENGINE_RUNNING,LD7B1. */
    if (ff_counter < skip_count)
        return BUA_FUEL_PATH_CRANK44;
    if ((engine_mode_word & (bua_u8)ENGINE_RUNNING_BIT) == 0u)
        return BUA_FUEL_PATH_CRANK44;
    return BUA_FUEL_PATH_NORMAL44;
}
static BuaFuelPath44 bua_select_fuel_path_step44(bua_u8 engine_mode_word,
                                                  bua_u8 ff_counter)
{
    return bua_select_fuel_path_core_step44(engine_mode_word,ff_counter,
                                             (bua_u8)CAL_CRANK_FF_SKIP_COUNT);
}
static void bua_apply_run_transition_step44(BuaRunQual44 *q)
{
    if (q->second_ref_valid != 0u)
        IGN_MODE_WORD |= SECOND_REF_VALID_BIT;
    else
        IGN_MODE_WORD &= (bua_u8)~SECOND_REF_VALID_BIT;
    STARTUP_COUNTER=q->startup_counter;
    if (q->engine_running != 0u)
        ENGINE_MODE_WORD |= ENGINE_RUNNING_BIT;
    else
        ENGINE_MODE_WORD &= (bua_u8)~ENGINE_RUNNING_BIT;
}
/* ---------------------------------------------------------------------- */
/* Step 24: active closed-loop O2 control, LDB35..LDD27.                  */
/*                                                                        */
/* This translation intentionally starts after the broader closed-loop    */
/* qualification/reset logic.  It models the executable fast rich/lean    */
/* decision, slow-O2 error path, proportional timing/step generation, and */
/* integrator update which produces L00C6 for Step 23.                    */
/* ---------------------------------------------------------------------- */
typedef struct {
    bua_u8 fast_rich;
    bua_u8 slow_rich;
    bua_u8 limited_error;
    bua_u8 prop_width;
    bua_u8 prop_step;
    bua_u8 integrator;
    bua_u8 int_timer;
    bua_u8 closed_corr;
} BuaO2ControlResult;
static bua_u8 bua_lookup_spacing16(const bua_u8 *table, bua_u8 points,
                                    bua_u8 argument)
{
    bua_u8 index;
    bua_u8 fraction;
    index = (bua_u8)(argument >> 4);
    fraction = (bua_u8)(argument << 4);
    if (index >= (bua_u8)(points - 1u))
        return table[(bua_u8)(points - 1u)];
    return bua_interp_u8_lf2d6(table[index], table[(bua_u8)(index + 1u)],
                               fraction);
}
static bua_u8 bua_o2_airflow_argument(bua_u16 airflow_word)
{
    bua_u16 doubled;
    bua_u8 arg;
    doubled = (bua_u16)(airflow_word << 1);
    arg = (bua_u8)(doubled >> 8);
    if (arg > 128u)
        arg = 128u;
    return arg;
}
/* LDB35..LDB9B.  Inside the +/- LC561 window the original chooses rich
 * when the fast filtered O2 is rising, lean otherwise. */
static bua_u8 bua_o2_fast_rich_ldb35(bua_u16 airflow_word,
                                     bua_u8 filtered_o2,
                                     bua_u8 previous_o2,
                                     bua_u8 lean_offset,
                                     bua_u8 air_diverter_active,
                                     bua_u8 *ready_out)
{
    bua_u8 arg;
    bua_u8 threshold;
    bua_u8 upper;
    bua_u8 lower;
    bua_u8 rich;
    arg = bua_o2_airflow_argument(airflow_word);
    threshold = bua_lookup_spacing16(cal_o2_fast_threshold, 9u, arg);
    threshold = (bua_u8)(threshold - lean_offset);
    if (air_diverter_active == 0u)
        threshold = (bua_u8)(threshold - (bua_u8)CAL_O2_AIR_DIV_WINDOW);
    if (ready_out != (bua_u8 *)0) {
        *ready_out = (filtered_o2 > (bua_u8)CAL_O2_READY_HIGH ||
                      filtered_o2 < (bua_u8)CAL_O2_READY_LOW) ? 1u : 0u;
    }
    upper = (bua_u8)(threshold + (bua_u8)CAL_O2_FAST_WINDOW);
    lower = (bua_u8)(threshold - (bua_u8)CAL_O2_FAST_WINDOW);
    if (filtered_o2 > upper)
        rich = 1u;
    else if (filtered_o2 < lower)
        rich = 0u;
    else
        rich = (filtered_o2 > previous_o2) ? 1u : 0u;
    ++stats.fuel_o2_fast_evaluations;
    return rich;
}
static bua_u8 bua_o2_slow_error_ldc05(bua_u8 airflow_arg,
                                      bua_u8 slow_o2,
                                      bua_u8 lean_offset,
                                      bua_u8 air_diverter_active,
                                      bua_u8 idle_condition,
                                      bua_u8 *slow_rich_out)
{
    bua_u8 upper;
    bua_u8 lower;
    bua_u8 error;
    bua_u8 factor;
    bua_u16 product;
    upper = bua_lookup_spacing16(cal_o2_slow_upper, 9u, airflow_arg);
    upper = (bua_u8)(upper - lean_offset);
    if (air_diverter_active == 0u)
        upper = (bua_u8)(upper - (bua_u8)CAL_O2_AIR_DIV_WINDOW);
    if (slow_o2 > upper) {
        error = (bua_u8)(slow_o2 - upper);
        product = (bua_u16)((bua_u16)error * (bua_u16)CAL_O2_POS_ERROR_GAIN);
        error = (bua_u8)(product >> 8);
        *slow_rich_out = 1u;
    } else {
        lower = bua_lookup_spacing16(cal_o2_slow_lower, 9u, airflow_arg);
        lower = (bua_u8)(lower - lean_offset);
        if (air_diverter_active == 0u)
            lower = (bua_u8)(lower - (bua_u8)CAL_O2_AIR_DIV_WINDOW);
        if (lower > slow_o2) {
            error = (bua_u8)(lower - slow_o2);
            *slow_rich_out = 0u;
        } else {
            error = 0u;
            /* LDC2B jumps directly to LDC69 without changing b5.  For the
             * zero-error case direction is immaterial to the arithmetic. */
            *slow_rich_out = 0u;
        }
    }
    factor = bua_lookup_spacing16(cal_o2_error_reduction, 9u, airflow_arg);
    if (idle_condition != 0u) {
        product = (bua_u16)((bua_u16)factor * (bua_u16)CAL_O2_IDLE_GAIN);
        factor = (bua_u8)(product >> 8);
    }
    product = (bua_u16)((bua_u16)factor * (bua_u16)error);
    error = (bua_u8)(product >> 8);
    if (error > 88u)
        error = 88u;
    return error;
}
static BuaO2ControlResult bua_closed_loop_o2_core_step103(bua_u16 airflow_word,
                                                     bua_u8 fast_rich,
                                                     bua_u8 slow_o2,
                                                     bua_u8 lean_offset,
                                                     bua_u8 air_diverter_active,
                                                     bua_u8 tps_axis,
                                                     bua_u8 vss_scaled,
                                                     bua_u8 rpm_special,
                                                     bua_u8 coolant,
                                                     bua_u8 duration_counter,
                                                     bua_u8 integrator,
                                                     bua_u8 int_timer)
{
    BuaO2ControlResult r;
    bua_u8 arg;
    bua_u8 idle;
    bua_u8 width_from_error;
    bua_u8 width_rpm_arg;
    bua_u8 width_rpm;
    bua_u8 int_delay_error;
    bua_u8 int_delay_air;
    bua_u8 delay;
    bua_u16 product;
    bua_u8 prop_error;
    arg = bua_o2_airflow_argument(airflow_word);
    idle = (tps_axis <= (bua_u8)CAL_O2_IDLE_TPS && vss_scaled == 0u) ? 1u : 0u;
    r.fast_rich = fast_rich;
    r.limited_error = bua_o2_slow_error_ldc05(arg, slow_o2, lean_offset,
                                               air_diverter_active, idle,
                                               &r.slow_rich);
    width_from_error = bua_lookup_spacing16(cal_o2_prop_width_error, 12u,
                                             (bua_u8)(r.limited_error << 1));
    if (rpm_special > 160u)
        rpm_special = 160u;
    width_rpm_arg = (bua_u8)(rpm_special >> 1);
    width_rpm = bua_lookup_spacing16(cal_o2_prop_width_rpm, 6u, width_rpm_arg);
    if ((bua_u16)width_from_error + (bua_u16)width_rpm > 255u)
        r.prop_width = 255u;
    else
        r.prop_width = (bua_u8)(width_from_error + width_rpm);
    r.integrator = integrator;
    r.int_timer = int_timer;
    if (r.limited_error > (bua_u8)CAL_O2_MIN_INT_ERROR &&
        r.fast_rich == r.slow_rich) {
        int_delay_error = bua_lookup_spacing16(cal_o2_int_delay_error, 12u,
                                                (bua_u8)(r.limited_error << 1));
        /* LDCAC..LDCBC: add a coolant-dependent offset only while
         * -(coolant)-LC566 does not borrow.  Preserve the unsigned test. */
        {
            bua_u8 neg_cool;
            bua_u8 temp;
            neg_cool = (bua_u8)(0u - coolant);
            temp = (bua_u8)(neg_cool - (bua_u8)CAL_O2_COOL_INT_OFFSET);
            if (neg_cool >= (bua_u8)CAL_O2_COOL_INT_OFFSET) {
                if ((bua_u16)int_delay_error + (bua_u16)temp > 255u)
                    int_delay_error = 255u;
                else
                    int_delay_error = (bua_u8)(int_delay_error + temp);
            }
        }
        int_delay_air = bua_lookup_spacing16(cal_o2_int_delay_airflow, 9u, arg);
        product = (bua_u16)((bua_u16)int_delay_error * (bua_u16)int_delay_air);
        delay = (bua_u8)(product >> 8);
        if (idle != 0u) {
            if ((bua_u16)delay + (bua_u16)CAL_O2_IDLE_INT_DELAY > (bua_u16)255u)
                delay = 255u;
            else
                delay = (bua_u8)(delay + (bua_u8)CAL_O2_IDLE_INT_DELAY);
        }
        if (delay <= r.int_timer) {
            r.int_timer = 0u;
            if (r.slow_rich != 0u) {
                if (r.integrator > (bua_u8)CAL_O2_INT_MIN)
                    r.integrator = (bua_u8)(r.integrator - 1u);
            } else {
                if (r.integrator < (bua_u8)CAL_O2_INT_MAX)
                    r.integrator = (bua_u8)(r.integrator + 1u);
            }
            ++stats.fuel_o2_integrator_updates;
        } else {
            r.int_timer = (bua_u8)(r.int_timer + 1u);
        }
    } else {
        r.int_timer = 0u;
    }
    /* LDC9C/LDD01 suppresses proportional action once the same rich/lean
     * condition has persisted longer than the computed width. */
    if ((r.fast_rich != r.slow_rich ||
         r.limited_error <= (bua_u8)CAL_O2_MIN_INT_ERROR) &&
        duration_counter > r.prop_width) {
        r.prop_step = 0u;
    } else {
        prop_error = (bua_u8)(r.limited_error << 1);
        r.prop_step = bua_lookup_spacing16(cal_o2_prop_step_error, 12u,
                                            prop_error);
    }
    if (r.fast_rich != 0u) {
        if (r.integrator < r.prop_step)
            r.closed_corr = 0u;
        else
            r.closed_corr = (bua_u8)(r.integrator - r.prop_step);
    } else {
        if ((bua_u16)r.integrator + (bua_u16)r.prop_step > 255u)
            r.closed_corr = 255u;
        else
            r.closed_corr = (bua_u8)(r.integrator + r.prop_step);
    }
    ++stats.fuel_o2_control_evaluations;
    return r;
}
static BuaO2ControlResult bua_closed_loop_o2_step24(bua_u16 airflow_word,
                                                     bua_u8 filtered_fast_o2,
                                                     bua_u8 previous_fast_o2,
                                                     bua_u8 slow_o2,
                                                     bua_u8 lean_offset,
                                                     bua_u8 air_diverter_active,
                                                     bua_u8 tps_axis,
                                                     bua_u8 vss_scaled,
                                                     bua_u8 rpm_special,
                                                     bua_u8 coolant,
                                                     bua_u8 duration_counter,
                                                     bua_u8 integrator,
                                                     bua_u8 int_timer)
{
    bua_u8 ready;
    bua_u8 fast_rich;
    fast_rich=bua_o2_fast_rich_ldb35(airflow_word,filtered_fast_o2,
                                      previous_fast_o2,lean_offset,
                                      air_diverter_active,&ready);
    (void)ready;
    return bua_closed_loop_o2_core_step103(airflow_word,fast_rich,slow_o2,
                    lean_offset,air_diverter_active,tps_axis,vss_scaled,
                    rpm_special,coolant,duration_counter,integrator,int_timer);
}
/* ---------------------------------------------------------------------- */
/* Step 23: BLM + closed-loop fuel correction (LDD7A..LDDB9).             */
/*                                                                        */
/* LF472 is a different 8x16 helper from LF266.  Its ROL chain doubles    */
/* the 24-bit partial product before retaining the upper 16 bits, so the  */
/* effective scaling is factor * value / 128.  Factor 128 is therefore    */
/* unity, matching the source initialization and BLM comments.            */
/* ---------------------------------------------------------------------- */
static bua_u16 bua_factor_u8_u16_lf472(bua_u8 factor, bua_u16 value)
{
    bua_u32 product;
    bua_u32 result;
    product = (bua_u32)factor * (bua_u32)value;
    result = product >> 7;
    if (result > 65535ul)
        result = 65535ul;
    ++stats.lf472_calls;
    return (bua_u16)result;
}
/* Literal LDD7A..LDDB9 arithmetic.  The proportional-step byte is the
 * value left on the stack by LDD06/LDD12.  When it is odd, L003D b0
 * selects the alternating round-up used by the original code.  L0044 b6
 * controls the final half-step add/subtract.  Keep the final 16-bit
 * add/subtract wrap exactly as the processor would; the preceding
 * integrator correction itself saturates at zero on underflow. */
static bua_u16 bua_apply_blm_closed_loop_ldd7a(bua_u16 base_pw,
                                                bua_u8 blm,
                                                bua_u8 closed_corr,
                                                bua_u8 proportional_step,
                                                bua_u8 af_mode_word2,
                                                bua_u8 air_mode_word)
{
    bua_u16 corrected;
    bua_u16 magnitude;
    bua_u8 half_step;
    corrected = bua_factor_u8_u16_lf472(blm, base_pw);
    ++stats.fuel_blm_corrections;
    if (closed_corr >= (bua_u8)CLOSED_LOOP_NEUTRAL) {
        magnitude = (bua_u16)(closed_corr - (bua_u8)CLOSED_LOOP_NEUTRAL);
        corrected = (bua_u16)(corrected + magnitude);
    } else {
        magnitude = (bua_u16)((bua_u8)CLOSED_LOOP_NEUTRAL - closed_corr);
        if (corrected < magnitude)
            corrected = 0u;
        else
            corrected = (bua_u16)(corrected - magnitude);
    }
    half_step = (bua_u8)(proportional_step >> 1);
    if ((proportional_step & 1u) != 0u &&
        (air_mode_word & AIR_INJECT_TOGGLE_BIT) != 0u)
        half_step = (bua_u8)(half_step + 1u);
    if ((af_mode_word2 & AF_RICH_LEAN_BIT) != 0u)
        corrected = (bua_u16)(corrected + (bua_u16)half_step);
    else
        corrected = (bua_u16)(corrected - (bua_u16)half_step);
    ++stats.fuel_closed_loop_corrections;
    return corrected;
}
static bua_u16 bua_fuel_base_and_corrections_step23(bua_u8 proportional_step)
{
    bua_u16 base_pw;
    bua_u16 corrected;
    base_pw = bua_base_pw_ldd2d();
    corrected = bua_apply_blm_closed_loop_ldd7a(base_pw, BLM,
                                                 CLOSED_LOOP_PW_CORR,
                                                 proportional_step,
                                                 AF_MODE_WORD2,
                                                 AIR_MODE_WORD);
    ram16be_set(0x00C2u, corrected);
    return corrected;
}
static void bua_startup_spark_initialize(bua_u8 startup_coolant)
{
    bua_u8 spark_raw;
    bua_u8 delay_raw;
    bua_u16 delay_count;
    if (startup_coolant > (bua_u8)CAL_STARTUP_SPARK_MAX_COOL)
        startup_coolant = (bua_u8)CAL_STARTUP_SPARK_MAX_COOL;
    spark_raw = bua_lookup_fixed16_table(cal_startup_spark,
                                          (bua_u8)CAL_STARTUP_SPARK_POINTS,
                                          startup_coolant);
    ram16be_set(STARTUP_SPARK_ADDR, (bua_u16)((bua_u16)spark_raw << 8));
    delay_raw = bua_lookup_fixed16_table(cal_startup_spark_delay,
                                          (bua_u8)CAL_STARTUP_SPARK_POINTS,
                                          startup_coolant);
    /* LECxx calls LF266 with A=table value and X=C188 (256).
     * LF266 returns round(A * X / 256), so C188=256 leaves the table
     * value numerically unchanged: 40 means 40 qualifying injector events. */
    delay_count = bua_mul_u8_u16_lf266(delay_raw,
                                        (bua_u16)CAL_STARTUP_SPARK_DELAY_SCALE);
    ram16be_set(STARTUP_SPARK_DELAY_ADDR, delay_count);
    STARTUP_SPARK_REPEAT = 0u;
    SPARK_AUX_MODE_WORD &= (bua_u8)~STARTUP_SPARK_PHASE_BIT;
    ++stats.startup_spark_initializations;
}
/* Literal LF6D5..LF719 behavior for one qualifying injector occurrence.
 * The caller is responsible for only invoking this when the source path has
 * an injector event and ENGINE RUNNING is true (LF67B..LF695). */
static void bua_startup_spark_on_inject(bua_u8 startup_coolant)
{
    bua_u16 d;
    bua_u8 repeat;
    bua_u8 decay;
    ++stats.startup_spark_inject_updates;
    if ((SPARK_MODE_WORD & SPARK_BLEND_BIT) != 0u)
        return;
    d = ram16be_get(STARTUP_SPARK_DELAY_ADDR);
    if (d != 0u) {
        ram16be_set(STARTUP_SPARK_DELAY_ADDR, (bua_u16)(d - 1u));
        return;
    }
    if ((SPARK_AUX_MODE_WORD & STARTUP_SPARK_PHASE_BIT) != 0u &&
        STARTUP_SPARK_REPEAT != 0u) {
        STARTUP_SPARK_REPEAT = (bua_u8)(STARTUP_SPARK_REPEAT - 1u);
        return;
    }
    SPARK_AUX_MODE_WORD |= STARTUP_SPARK_PHASE_BIT;
    if (startup_coolant > (bua_u8)CAL_STARTUP_SPARK_MAX_COOL)
        startup_coolant = (bua_u8)CAL_STARTUP_SPARK_MAX_COOL;
    repeat = bua_lookup_fixed16_table(cal_startup_spark_repeat,
                                      (bua_u8)CAL_STARTUP_SPARK_POINTS,
                                      startup_coolant);
    STARTUP_SPARK_REPEAT = repeat;
    decay = bua_lookup_fixed16_table(cal_startup_spark_decay,
                                     (bua_u8)CAL_STARTUP_SPARK_POINTS,
                                     startup_coolant);
    d = ram16be_get(STARTUP_SPARK_ADDR);
    if (d < (bua_u16)decay)
        d = 0u;
    else
        d = (bua_u16)(d - (bua_u16)decay);
    ram16be_set(STARTUP_SPARK_ADDR, d);
    ++stats.startup_spark_decay_events;
}
static bua_u8 bua_startup_spark_sum_byte(void)
{
    /* LDAB L0013 in the summation consumes the high byte only. */
    return RAM8(0x0013u);
}
/* ---------------------------------------------------------------------- */
/* Step 17: knock qualification, attack, WOT limiting, and recovery.      */
/* ---------------------------------------------------------------------- */
static bua_u8 bua_lookup_knock_rate(const bua_u8 *table, bua_u8 rpm_special)
{
    bua_u8 arg;
    arg = (bua_u8)(rpm_special >> 2);
    return bua_lookup_fixed16_table(table, (bua_u8)CAL_KNOCK_RATE_POINTS, arg);
}
static bua_u8 bua_lookup_knock_wot_limit(bua_u8 rpm25)
{
    bua_u8 arg;
    /* LD13D path: L0057, LSRA, then LF2B9 with offset 16. */
    arg = (bua_u8)(rpm25 >> 1);
    if (arg < 16u)
        arg = 0u;
    else
        arg = (bua_u8)(arg - 16u);
    return bua_lookup_fixed16_table(cal_knock_wot_limit,
                                    (bua_u8)CAL_KNOCK_WOT_POINTS, arg);
}
/* Translate LD0D1..LD157 after the knock counter has already been reduced
 * to an 8-bit event count by the original L3FCA-L00A1 arithmetic. */
static bua_u8 bua_knock_attack_eval(bua_u8 knock_count)
{
    bua_u8 a;
    bua_u8 delta;
    bua_u8 rate;
    bua_u8 limit;
    bua_u16 product;
    bua_u16 doubled;
    bua_u16 sum;
    ++stats.knock_attack_evaluations;
    a = 0u;
    if ((MODE_WORD3 & KNOCK_DIFF_COOL_LATCH) == 0u) {
        if (COOLANT > STARTUP_COOLANT) {
            delta = (bua_u8)(COOLANT - STARTUP_COOLANT);
            if (delta > (bua_u8)CAL_KNOCK_DELTA_COOLANT) {
                MODE_WORD3 |= KNOCK_DIFF_COOL_LATCH;
            } else if (COOLANT < (bua_u8)CAL_KNOCK_MIN_COOLANT) {
                DIAG_MODE_WORD2 &= (bua_u8)~KNOCK_ENABLED_FLAG;
                KNOCK_RETARD = 0u;
                return 0u;
            }
        } else if (COOLANT < (bua_u8)CAL_KNOCK_MIN_COOLANT) {
            DIAG_MODE_WORD2 &= (bua_u8)~KNOCK_ENABLED_FLAG;
            KNOCK_RETARD = 0u;
            return 0u;
        }
    }
    DIAG_MODE_WORD2 |= KNOCK_ENABLED_FLAG;
    if (RPM_DIV12P5 < (bua_u8)CAL_KNOCK_MIN_RPM12P5 &&
        VSS_MPH_HI < (bua_u8)CAL_KNOCK_MIN_VSS) {
        KNOCK_RETARD = 0u;
        return 0u;
    }
    /* LD116 preloads A with the fixed fail-safe retard.  Several inhibit\/
     * fault conditions branch directly to LD157 and therefore retain it. */
    a = (bua_u8)CAL_KNOCK_FAIL_RETARD;
    if ((MODE_WORD2 & KNOCK_FAIL_MODE_BIT) != 0u ||
        BATTERY_AD < (bua_u8)CAL_KNOCK_MIN_BATTERY ||
        (SPARK_AUX_MODE_WORD & KNOCK_INHIBIT_AUX_BIT) != 0u) {
        KNOCK_RETARD = a;
        return a;
    }
    rate = bua_lookup_knock_rate(cal_knock_attack_rate, RPM_SPECIAL);
    product = (bua_u16)((bua_u16)rate * (bua_u16)knock_count);
    doubled = (bua_u16)(product << 1);
    /* MUL; ASLD; ADDA L00A5: only the high byte of the doubled product is
     * accumulated into the 8-bit retard value.  Carry saturates to 255. */
    sum = (bua_u16)KNOCK_RETARD + (bua_u16)(doubled >> 8);
    if (sum > 255u)
        a = 255u;
    else
        a = (bua_u8)sum;
    limit = (bua_u8)CAL_KNOCK_MAX_NORMAL;
    if ((AF_MODE_WORD & POWER_ENRICH_ACTIVE) != 0u)
        limit = bua_lookup_knock_wot_limit(RPM_DIV25);
    if (a > limit)
        a = limit;
    KNOCK_RETARD = a;
    return a;
}
/* LEB3A..LEB59.  Segment A calls this only when L0000 b4 is clear, which
 * makes the recovery path execute five times per second in the 160-count
 * scheduler.  MUL carry is bit 7 of the low product byte, so ADCA #0 rounds
 * product/256; the original then forces a minimum decrement of one. */
static void bua_knock_recovery_segment_a(void)
{
    bua_u8 rate;
    bua_u8 decrement;
    bua_u16 product;
    bua_u16 rounded;
    if ((MINOR_COUNT & 0x10u) != 0u)
        return;
    ++stats.knock_recovery_evaluations;
    rate = bua_lookup_knock_rate(cal_knock_recovery_rate, RPM_SPECIAL);
    product = (bua_u16)((bua_u16)rate * (bua_u16)KNOCK_RETARD);
    rounded = (bua_u16)((product + 128u) >> 8);
    if (rounded == 0u)
        decrement = 1u;
    else if (rounded > 255u)
        decrement = 255u;
    else
        decrement = (bua_u8)rounded;
    if (KNOCK_RETARD <= decrement) {
        if (KNOCK_RETARD != 0u)
            ++stats.knock_recovery_changes;
        KNOCK_RETARD = 0u;
    } else {
        KNOCK_RETARD = (bua_u8)(KNOCK_RETARD - decrement);
        ++stats.knock_recovery_changes;
    }
}
static bua_u8 bua_knock_spark_subtract_raw(void)
{
    /* LD157 immediately LSRA before subtracting from L009D. */
    return (bua_u8)(KNOCK_RETARD >> 1);
}
/* ---------------------------------------------------------------------- */
/* Step 18: literal spark summation and pre-Mode-4 limits.                 */
/* ---------------------------------------------------------------------- */
/* LD063..LD08B.  This is the extra spark used while the error-43 knock
 * diagnostic test is active.  The original clears L0002 b0 on the
 * disqualifying branches that pass through LD088. */
static bua_u8 bua_knock_test_spark_eval(bua_u8 hot_restart_raw)
{
    if ((MINOR_MODE_WORD2 & MINOR_DIAG_SWITCH_BIT) != 0u ||
        (SPARK_INPUT_MODE_WORD & SPARK_INPUT_TEST_ENABLE_BIT) == 0u ||
        hot_restart_raw != 0u ||
        (MODE_WORD2 & MODE2_ERR43_FAIL_BIT) != 0u) {
        MODE_WORD2 &= (bua_u8)~MODE2_ERR43_TEST_BIT;
        return 0u;
    }
    if ((MODE_WORD2 & MODE2_ERR43_TEST_BIT) == 0u)
        return 0u;
    if (COOLANT <= (bua_u8)CAL_ERR43_TEST_MIN_COOLANT)
        return 0u;
    return (bua_u8)CAL_ERR43_TEST_SPARK_RAW;
}
static bua_u8 bua_aldl_added_spark_eval(void)
{
    if ((MINOR_MODE_WORD2 & MINOR_ALDL_SPARK_BIT) != 0u)
        return (bua_u8)CAL_ALDL_ADDED_SPARK_RAW;
    return 0u;
}
/* LD096..LD0BF.  X is a 16-bit accumulator.  Stack order reconstructs:
 * ALDL + knock-test + highway + coolant + main + startup, then the code
 * subtracts the coolant-table bias and the hot-restart retard. */
static bua_s16 bua_spark_sum_ld096(bua_u8 main_raw,
                                    bua_u8 coolant_raw,
                                    bua_u8 highway_raw,
                                    bua_u8 hot_restart_raw,
                                    bua_u8 knock_test_raw,
                                    bua_u8 aldl_raw,
                                    bua_u8 startup_raw)
{
    bua_u16 x;
    x = 0u;
    x = (bua_u16)(x + (bua_u16)aldl_raw);
    x = (bua_u16)(x + (bua_u16)knock_test_raw);
    x = (bua_u16)(x + (bua_u16)highway_raw);
    x = (bua_u16)(x + (bua_u16)coolant_raw);
    x = (bua_u16)(x + (bua_u16)main_raw);
    x = (bua_u16)(x + (bua_u16)startup_raw);
    x = (bua_u16)(x - (bua_u16)CAL_COOL_SPARK_BIAS_RAW);
    x = (bua_u16)(x - (bua_u16)hot_restart_raw);
    ram16be_set(FINAL_SPARK_ADV_ADDR, x);
    ++stats.final_spark_calculations;
    return (bua_s16)x;
}
/* LD0BF..LD192.  This stops immediately before the Mode-4 / spark-blend
 * section.  It preserves the signed branch behavior of the original. */
static bua_s16 bua_spark_relative_to_reference_pre_mode4(void)
{
    bua_u16 d;
    bua_u16 rel;
    bua_s16 signed_rel;
    rel = ram16be_get(FINAL_SPARK_ADV_ADDR);
    rel = (bua_u16)(rel - (bua_u16)CAL_INITIAL_REF_LEAD_RAW);
    ram16be_set(SPARK_REL_REF_ADDR, rel);
    d = (bua_u16)((bua_u16)CAL_MAX_SPARK_REL_REF_RAW - rel);
    if ((bua_s16)d <= 0) {
        rel = (bua_u16)CAL_MAX_SPARK_REL_REF_RAW;
        ram16be_set(SPARK_REL_REF_ADDR, rel);
    }
    rel = ram16be_get(SPARK_REL_REF_ADDR);
    rel = (bua_u16)(rel - (bua_u16)bua_knock_spark_subtract_raw());
    ram16be_set(SPARK_REL_REF_ADDR, rel);
    if ((DIAG_MODE_WORD3 & DIAG_FIXED_SPARK_BIT) != 0u) {
        rel = (bua_u16)CAL_DIAG_FIXED_SPARK_RAW;
        ram16be_set(SPARK_REL_REF_ADDR, rel);
    }
    signed_rel = (bua_s16)ram16be_get(SPARK_REL_REF_ADDR);
    if (signed_rel < (bua_s16)-11) {
        rel = (bua_u16)CAL_MIN_SPARK_REL_REF_RAW;
        ram16be_set(SPARK_REL_REF_ADDR, rel);
    }
    return (bua_s16)ram16be_get(SPARK_REL_REF_ADDR);
}
/* ---------------------------------------------------------------------- */
/* Step 19: LD192..LD1EE Mode-4 modification, blend, and sign/magnitude.  */
/* ---------------------------------------------------------------------- */
/* Returns the low-byte spark magnitude that LD1EE pushes for the next
 * spark-output stage.  L009D itself is deliberately left in the exact
 * 16-bit representation produced by the source. */
static bua_u8 bua_spark_mode4_blend_ld192(void)
{
    bua_u16 d;
    bua_u8 a;
    bua_u8 b;
    bua_u8 carry;
    ++stats.spark_mode4_evaluations;
    if ((MINOR_MODE_WORD2 & MINOR_MODE4_BIT) != 0u &&
        (ALDL_FUNC_MOD_ENABLE & ALDL_SPARK_ENABLE_BIT) != 0u) {
        a = ALDL_FUNC_MOD_ENABLE;
        b = ALDL_SPARK_COMMAND;
        if ((a & ALDL_SPARK_MODIFY_BIT) == 0u) {
            if ((a & ALDL_SPARK_RETARD_BIT) != 0u) {
                /* LD1A8: LDAA #128 / NEGB.  The high byte is a negative
                 * marker; LD1E7 later NEGBs the low byte back to magnitude. */
                b = (bua_u8)(0u - b);
                d = (bua_u16)(0x8000u | (bua_u16)b);
            } else {
                d = 0u;
            }
        } else if ((a & ALDL_SPARK_RETARD_BIT) != 0u) {
            d = ram16be_get(SPARK_REL_REF_ADDR);
            d = (bua_u16)(d - (bua_u16)ALDL_SPARK_COMMAND);
        } else {
            /* Preserve the unusual literal ADDB L015B / ADCA #0111.
             * 0111 is decimal 111 in this source.  No semantic correction
             * is imposed here even though only the low byte reaches LD1EE. */
            d = ram16be_get(SPARK_REL_REF_ADDR);
            b = (bua_u8)d;
            a = (bua_u8)(d >> 8);
            {
                bua_u16 low_sum;
                bua_u16 high_sum;
                low_sum = (bua_u16)b + (bua_u16)ALDL_SPARK_COMMAND;
                b = (bua_u8)low_sum;
                carry = (low_sum > 255u) ? 1u : 0u;
                high_sum = (bua_u16)a + 111u + (bua_u16)carry;
                a = (bua_u8)high_sum;
            }
            d = (bua_u16)(((bua_u16)a << 8) | (bua_u16)b);
        }
        ram16be_set(SPARK_REL_REF_ADDR, d);
    } else {
        /* LD1C8: diagnostics and negative retard bypass the blend. */
        d = ram16be_get(SPARK_REL_REF_ADDR);
        if ((DIAG_MODE_WORD3 & DIAG_FIXED_SPARK_BIT) == 0u &&
            (bua_s16)d >= 0) {
            d = bua_mul_u8_u16_lf266(SPARK_BLEND_MULT, d);
            ram16be_set(SPARK_REL_REF_ADDR, d);
            ++stats.spark_blend_evaluations;
        }
    }
    /* LD1DB..LD1EE: high byte sign chooses advance/retard flag; for retard
     * only B is negated to obtain the magnitude pushed to the next stage. */
    d = ram16be_get(SPARK_REL_REF_ADDR);
    b = (bua_u8)d;
    if (((bua_u8)(d >> 8) & 0x80u) != 0u) {
        ENGINE_MODE_WORD |= SPARK_ADV_RETARD_FLAG;
        b = (bua_u8)(0u - b);
    } else {
        ENGINE_MODE_WORD &= (bua_u8)~SPARK_ADV_RETARD_FLAG;
    }
    return b;
}
/* ---------------------------------------------------------------------- */
/* Step 20: LD1F6..LD25x angle-to-time conversion and MPU spark updates.  */
/* ---------------------------------------------------------------------- */
typedef struct {
    bua_u16 angle_time_counts;
    bua_s16 requested_fire_offset;
    bua_s16 limited_fire_offset;
    bua_s16 fire_delta;
    bua_s16 dwell_delta;
} BuaSparkTimingResult;
/* Reproduce the source's 0-ref_period followed by four LSRD instructions
 * and ORAA #$F0.  For the normal positive reference periods this is the
 * signed -ceil(ref_period/16) slew step used by LD20E..LD22x. */
static bua_u16 bua_spark_negative_period_sixteenth(bua_u16 ref_period)
{
    bua_u16 d;
    d = (bua_u16)(0u - ref_period);
    d = (bua_u16)(d >> 1);
    d = (bua_u16)(d >> 1);
    d = (bua_u16)(d >> 1);
    d = (bua_u16)(d >> 1);
    d = (bua_u16)(d | 0xF000u);
    return d;
}
/* LD20E..LD243 limiting, separated so the two independent limits can be
 * tested directly: advance may move earlier by at most ref_period/16 per
 * execution, and the absolute offset is bounded to +/- one ref period. */
static bua_u16 bua_spark_limit_fire_offset(bua_u16 requested,
                                            bua_u16 old_fire,
                                            bua_u16 ref_period,
                                            bua_u8 *slew_limited)
{
    bua_u16 d;
    bua_u16 neg_step;
    bua_u16 neg_period;
    *slew_limited = 0u;
    neg_step = bua_spark_negative_period_sixteenth(ref_period);
    d = neg_step;
    d = (bua_u16)(d + old_fire);
    d = (bua_u16)(d - requested);
    if ((d & 0x8000u) == 0u) {
        requested = (bua_u16)(d + requested);
        *slew_limited = 1u;
    }
    neg_period = (bua_u16)(0u - ref_period);
    if ((requested & 0x8000u) != 0u) {
        /* Source computes (-ref_period) - requested and takes BCC LD23F.
         * No borrow therefore means requested is more negative than -period. */
        if (neg_period >= requested)
            requested = neg_period;
    } else {
        /* Positive/retard side: source clamps only if ref_period-requested
         * borrows, i.e. requested exceeds one full reference period. */
        if (requested > ref_period)
            requested = ref_period;
    }
    return requested;
}
/* LD1F6..LD25x.  At LD1F6 the low-byte spark magnitude has just been
 * popped into A while the original reference period is still on the stack.
 * LF266 therefore converts raw spark angle to timer counts as
 * round(ref_period * magnitude / 256).  Advance is made negative; retard
 * remains positive.  LC01D is then subtracted as the fixed time correction.
 *
 * The resulting reference-to-fire offset is slew/period limited and the
 * MPU-facing delta quantities are updated exactly with 16-bit wraparound:
 *   L3FE8 = new_fire - old_fire
 *   L3FE6 = fire_delta + old_dwell - new_dwell
 *   L3FDC = new_dwell
 *   L3FF6 = new_fire
 */
static BuaSparkTimingResult bua_spark_timing_output_ld1f6(bua_u8 magnitude)
{
    BuaSparkTimingResult r;
    bua_u16 ref_period;
    bua_u16 angle_counts;
    bua_u16 requested;
    bua_u16 limited;
    bua_u16 old_fire;
    bua_u16 old_dwell;
    bua_u16 new_dwell;
    bua_u16 delta;
    bua_u8 slew_limited;
    ++stats.spark_timing_calculations;
    ref_period = ram16be_get(REF_PERIOD_ADDR);
    angle_counts = bua_mul_u8_u16_lf266(magnitude, ref_period);
    r.angle_time_counts = angle_counts;
    if ((ENGINE_MODE_WORD & SPARK_ADV_RETARD_FLAG) == 0u)
        requested = (bua_u16)(0u - angle_counts);
    else
        requested = angle_counts;
    requested = (bua_u16)(requested - (bua_u16)CAL_SPARK_TIME_CORR_COUNTS);
    r.requested_fire_offset = (bua_s16)requested;
    old_fire = mpu16be_get((bua_u16)MPU_SPARK_FIRE_OFFSET_ADDR);
    limited = bua_spark_limit_fire_offset(requested, old_fire, ref_period,
                                           &slew_limited);
    if (slew_limited != 0u)
        ++stats.spark_timing_slew_limits;
    r.limited_fire_offset = (bua_s16)limited;
    delta = (bua_u16)(limited - old_fire);
    mpu16be_set((bua_u16)MPU_SPARK_FALL_DELTA_ADDR, delta);
    r.fire_delta = (bua_s16)delta;
    old_dwell = mpu16be_get((bua_u16)MPU_SPARK_OLD_DWELL_ADDR);
    new_dwell = ram16be_get(DWELL_COUNTS_ADDR);
    delta = (bua_u16)(delta + old_dwell);
    delta = (bua_u16)(delta - new_dwell);
    mpu16be_set((bua_u16)MPU_SPARK_DWELL_DELTA_ADDR, delta);
    r.dwell_delta = (bua_s16)delta;
    mpu16be_set((bua_u16)MPU_SPARK_OLD_DWELL_ADDR, new_dwell);
    mpu16be_set((bua_u16)MPU_SPARK_FIRE_OFFSET_ADDR, limited);
    return r;
}
/* ---------------------------------------------------------------------- */
/* Step 21: remaining EST output state and low-RPM/stall-saver handling.  */
/* ---------------------------------------------------------------------- */
typedef struct {
    bua_u8 est_enabled;
    bua_u8 error42_set;
    bua_u8 mode4_bypass;
    bua_u8 first_ref_consumed;
} BuaEstOutputResult;
static BuaEstOutputResult bua_est_output_finish_ld25x(void)
{
    BuaEstOutputResult r;
    bua_u16 cpu_cr;
    bua_u8 a;
    ++stats.est_output_evaluations;
    r.est_enabled = 0u;
    r.error42_set = 0u;
    r.mode4_bypass = 0u;
    r.first_ref_consumed = 0u;
    a = IGN_MODE_WORD;
    if ((a & IGN_FIRST_REF_BIT) != 0u) {
        if ((a & IGN_REF_STATE_BIT) == 0u &&
            (ENGINE_MODE_WORD & 0x40u) == 0u) {
            if ((a & 0x40u) != 0u) {
                IGN_MODE_WORD |= IGN_ERR42_LATCH_BIT;
                r.error42_set = 1u;
            } else {
                bua_u8 fb;
                fb = SPARK_FEEDBACK_COUNT;
                SPARK_FEEDBACK_COUNT = 0u;
                if (fb > CAL_ERR42_FEEDBACK_LIMIT) {
                    IGN_MODE_WORD |= IGN_ERR42_LATCH_BIT;
                    r.error42_set = 1u;
                } else {
                    ENGINE_MODE_WORD |= SPARK_FEEDBACK_BYPASS_BIT;
                }
            }
        }
        IGN_MODE_WORD &= (bua_u8)~IGN_FIRST_REF_BIT;
        mpu16be_set((bua_u16)MPU_NEXT_DWELL_ADDR,
                    mpu16be_get((bua_u16)MPU_LAST_REF_ADDR));
        r.first_ref_consumed = 1u;
    }
    cpu_cr = mpu16be_get((bua_u16)MPU_CPU_CONTROL_ADDR);
    cpu_cr = (bua_u16)(cpu_cr | CPU_EST_BYPASS_BIT);
    if ((SPARK_INPUT_MODE_WORD & EST_ENABLE_BIT) == 0u) {
        SPARK_BLEND_MULT = 0u;
        SPARK_MODE_WORD |= SPARK_BLEND_BIT;
        SPARK_INPUT_MODE_WORD |= EST_ENABLE_BIT;
        ++stats.est_enable_events;
    }
    if ((SPARK_MODE_WORD & SPARK_STALL_ACTIVE_BIT) != 0u ||
        (MODE_WORD1 & 0x80u) != 0u) {
        SPARK_INPUT_MODE_WORD &= (bua_u8)~EST_ENABLE_BIT;
        cpu_cr = (bua_u16)(cpu_cr & (bua_u16)~CPU_EST_BYPASS_BIT);
        ++stats.est_disable_events;
        if ((SPARK_MODE_WORD & SPARK_STALL_ERROR_SKIP_BIT) == 0u) {
            NEW_ERROR_WORD2 |= NEW_ERROR42_BIT;
            r.error42_set = 1u;
        } else if (SPARK_FEEDBACK_COUNT > CAL_ERR42_FEEDBACK_LIMIT) {
            MODE_WORD1 |= 0x80u;
        }
    }
    if ((MINOR_MODE_WORD2 & MINOR_MODE4_BIT) != 0u &&
        (ALDL_MODE4_WORD4 & MODE4_EST_BYPASS_BIT) != 0u) {
        cpu_cr = (bua_u16)(cpu_cr & (bua_u16)~CPU_EST_BYPASS_BIT);
        SPARK_INPUT_MODE_WORD &= (bua_u8)~EST_ENABLE_BIT;
        SPARK_FEEDBACK_COUNT = 0u;
        SPARK_MODE_WORD |= SPARK_STALL_ERROR_SKIP_BIT;
        ram16be_set(OLD_SPARK_PERIOD_ADDR,
                    mpu16be_get((bua_u16)MPU_SPARK_PERIOD_ADDR));
        r.mode4_bypass = 1u;
        ++stats.est_mode4_bypass_events;
    }
    mpu16be_set((bua_u16)MPU_CPU_CONTROL_ADDR, cpu_cr);
    r.est_enabled = ((SPARK_INPUT_MODE_WORD & EST_ENABLE_BIT) != 0u) ? 1u : 0u;
    if (r.error42_set != 0u) ++stats.est_error42_events;
    return r;
}
typedef struct {
    bua_u8 entered;
    bua_u8 exited;
    bua_u8 active;
} BuaStallSaverResult;
static BuaStallSaverResult bua_stall_saver_update(bua_u8 rpm12p5,
                                                   bua_u8 aux_l005a)
{
    BuaStallSaverResult r;
    bua_u8 enter;
    r.entered = 0u;
    r.exited = 0u;
    r.active = ((SPARK_MODE_WORD & SPARK_STALL_ACTIVE_BIT) != 0u) ? 1u : 0u;
    if ((MODE_WORD1 & 0x80u) != 0u) {
        SPARK_MODE_WORD &= (bua_u8)~(SPARK_STALL_ACTIVE_BIT | SPARK_STALL_ERROR_SKIP_BIT);
        r.exited = r.active;
        r.active = 0u;
        if (r.exited != 0u) ++stats.stall_saver_exits;
        return r;
    }
    enter = 0u;
    if (rpm12p5 < CAL_STALL_RPM_LOW)
        enter = 1u;
    else if (rpm12p5 < CAL_STALL_RPM_MID && aux_l005a > CAL_STALL_RPM_AUX)
        enter = 1u;
    if (enter != 0u) {
        if ((SPARK_MODE_WORD & SPARK_STALL_ACTIVE_BIT) == 0u) {
            SPARK_FEEDBACK_COUNT = 0u;
            mpu16be_set((bua_u16)MPU_CPU_CONTROL_ADDR,
                        (bua_u16)(mpu16be_get((bua_u16)MPU_CPU_CONTROL_ADDR) &
                                  (bua_u16)~CPU_EST_BYPASS_BIT));
            SPARK_INPUT_MODE_WORD &= (bua_u8)~EST_ENABLE_BIT;
            ram16be_set(OLD_SPARK_PERIOD_ADDR,
                        mpu16be_get((bua_u16)MPU_SPARK_PERIOD_ADDR));
            r.entered = 1u;
            ++stats.stall_saver_entries;
        }
        SPARK_MODE_WORD |= (SPARK_STALL_ACTIVE_BIT | SPARK_STALL_ERROR_SKIP_BIT);
        r.active = 1u;
        return r;
    }
    if ((SPARK_MODE_WORD & SPARK_STALL_ACTIVE_BIT) != 0u &&
        rpm12p5 > CAL_STALL_RPM_DISABLE) {
        SPARK_MODE_WORD &= (bua_u8)~(SPARK_STALL_ACTIVE_BIT | SPARK_STALL_ERROR_SKIP_BIT);
        SPARK_FEEDBACK_COUNT = (bua_u8)(SPARK_FEEDBACK_COUNT + 1u);
        mpu16be_set((bua_u16)MPU_SPARK_OLD_DWELL_ADDR, 0u);
        mpu16be_set((bua_u16)MPU_SPARK_DWELL_DELTA_ADDR, 0u);
        mpu16be_set((bua_u16)MPU_SPARK_FALL_DELTA_ADDR, 0u);
        mpu16be_set((bua_u16)MPU_SPARK_FIRE_OFFSET_ADDR, 0u);
        mpu16be_set((bua_u16)MPU_NEXT_DWELL_ADDR,
                    mpu16be_get((bua_u16)MPU_LAST_REF_ADDR));
        r.exited = 1u;
        r.active = 0u;
        ++stats.stall_saver_exits;
    }
    return r;
}
static void bua_low_rpm_spark_reset_ld306(void)
{
    ram16be_set(0x0468u, 0xFFFFu);
    ram16be_set(0x0056u, 0u);
    ram16be_set(0x0058u, 0u);
    if ((MODE_WORD1 & 0x08u) == 0u)
        ram16be_set(ENGINE_RUN_TIME_ADDR, 0u);
    ram16be_set(TRANSIENT_LOAD_ADDR, (bua_u16)((bua_u16)CAL_TRANSIENT_LOAD_INIT << 8));
    ram16be_set(ACCEL_FUEL_ADDR, 0u);
    ACCEL_LOAD_FACTOR = 0u;
    LOWRPM_AUX_VALUE = 0u;
    SPARK_INPUT_MODE_WORD &= (bua_u8)~EST_ENABLE_BIT;
    ++stats.low_rpm_spark_resets;
}
/* ---------------------------------------------------------------------- */
/* Step 12: LCEB3..LCF1A normal-running dwell calculation.                */
/*                                                                        */
/* This is a literal integer translation of the dwell arithmetic up to    */
/* the pass-by/TCC qualification code.  It does not yet model stall-saver */
/* hardware writes, spark-table lookup, or MPU output registers.          */
/* ---------------------------------------------------------------------- */
static void bua_compute_dwell_12p5ms(void)
{
    bua_u16 d;
    bua_u16 dynamic;
    bua_u16 ref_period;
    bua_u16 old_ref_period;
    bua_u16 base_dwell;
    bua_u16 dwell;
    bua_u8 load_delta;
    bua_u8 batt_deficit;
    ref_period = ram16be_get(REF_PERIOD_ADDR);
    if (ref_period == 0u || ref_period == 0xFFFFu)
        return;
    dynamic = ram16be_get(DYNAMIC_DWELL_ADDR);
    /* LCEB3..LCECB. A is intentionally $FF before the 8-bit load
     * subtraction. A sufficiently large positive load step therefore
     * creates $FFxx, which the following reference-period/8 limit clamps. */
    load_delta = (bua_u8)(FILTERED_LOAD - LOAD_25MS_OLD);
    if (FILTERED_LOAD >= LOAD_25MS_OLD &&
        load_delta >= (bua_u8)CAL_DWELL_LOAD_DELTA) {
        dynamic = (bua_u16)(0xFF00u | (bua_u16)load_delta);
    } else {
        old_ref_period = ram16be_get(REF_PERIOD_OLD_ADDR);
        d = (bua_u16)(old_ref_period - ref_period);
        d = (bua_u16)(d << 1);
        d = (bua_u16)(d - dynamic);
        if ((bua_s16)d >= 0) {
            d = (bua_u16)(d + dynamic);
            dynamic = d;
        }
    }
    /* LCECD..LCEDA: dynamic dwell cannot exceed reference period / 8. */
    d = (bua_u16)(ref_period >> 3);
    if (dynamic > d)
        dynamic = d;
    ram16be_set(DYNAMIC_DWELL_ADDR, dynamic);
    /* The previous reference period is updated here in the original. */
    ram16be_set(REF_PERIOD_OLD_ADDR, ref_period);
    /* LCEDA..LCEF9: piecewise base-dwell calculation, preserving 16-bit
     * wraparound on the underflow branches. */
    d = (bua_u16)(ref_period >> 1);
    if (d < 229u) {
        d = (bua_u16)(d - 229u);
        d = (bua_u16)(d + 308u);
    } else {
        d = (bua_u16)(d - 229u);
        d = (bua_u16)(d >> 1);
        if (d < 295u) {
            d = (bua_u16)(d - 295u);
            d = (bua_u16)(d + 1527u);
            d = (bua_u16)(d >> 2);
        } else {
            d = (bua_u16)(d - 295u);
            d = (bua_u16)(d + 382u);
        }
    }
    base_dwell = d;
    /* LCEF9..LCF02: only low battery adds dwell. */
    if (BATTERY_AD < (bua_u8)CAL_DWELL_BATT_REF)
        batt_deficit = (bua_u8)((bua_u8)CAL_DWELL_BATT_REF - BATTERY_AD);
    else
        batt_deficit = 0u;
    d = (bua_u16)((bua_u16)batt_deficit *
                  (bua_u16)CAL_DWELL_BATT_GAIN);
    d = (bua_u16)(d + base_dwell);
    d = (bua_u16)(d + dynamic);
    dwell = d;
    /* LCF04..LCF1A: dwell cannot consume the whole reference period; the
     * source leaves a literal 39-count margin. */
    d = (bua_u16)(ref_period - (bua_u16)CAL_DWELL_REF_MARGIN);
    if (d < dwell)
        dwell = d;
    ram16be_set(DWELL_COUNTS_ADDR, dwell);
    ++stats.dwell_calculations;
}
static void one_second_event(void)
{
    bua_u16 seconds;
    ALDL_MODE_WORD ^= 0x20u;
    if ((ENGINE_MODE_WORD & 0x80u) != 0u) {
        seconds = ram16be_get(0x001Au);
        ram16be_set(0x001Au, (bua_u16)(seconds + 1u));
    }
    ++stats.one_second_events;
}
/* Step 92: scheduler-facing LF4DF/LF5DF IAC motor service subset.
 * The source enters LF4DF on every ordinary 6.25-ms IRQ before the odd/even
 * fuel/spark split.  This wrapper deliberately implements only the already
 * proven LF5DF motor-consumption/bookkeeping portion.  Open-loop trajectory,
 * PID command production, Mode-4 forcing, and physical coil writes remain in
 * their translated helpers/HAL until the following reconciliation passes. */
static void bua_iac_motor_service_6p25ms(void)
{
    bua_u8 command;
    bua_u8 mag;
    bua_u8 position;
    bua_u8 running;
    bua_u8 control;
    ++stats.iac_minor_services;
    command = RAM8(0x0101u);
    mag = (bua_u8)(command & 0x7Fu);
    control = RAM8(0x00F2u);
    /* LF514..LF521 shuts the motor off outside the useful battery window
     * while running.  Keep the hardware-enable state explicit. */
    if ((ENGINE_MODE_WORD & ENGINE_RUNNING_BIT) != 0u &&
        (RAM8(0x007Eu) <= 0x5Au || RAM8(0x007Eu) > 0xABu)) {
        sim_iac_motor_on = 0u;
        return;
    }
    if (mag == 0u)
        return;
    /* LF5DF: an off motor is enabled first; the pending step survives until
     * the following 6.25-ms service. */
    if (sim_iac_motor_on == 0u) {
        sim_iac_motor_on = 1u;
        return;
    }
    RAM8(0x0101u) = (bua_u8)(command - 1u);
    ++stats.iac_steps_consumed;
    position = RAM8(0x002Cu);
    running = RAM8(0x010Au);
    if ((command & 0x80u) != 0u) {
        if (position != 0u)
            --position;
        --running;
        if ((control & 0x10u) != 0u && RAM8(0x0105u) != 0u)
            --RAM8(0x0105u);
    } else {
        if (position < 145u) {
            ++position;
            ++running;
            if ((control & 0x10u) != 0u)
                ++RAM8(0x0105u);
        }
    }
    RAM8(0x002Cu) = position;
    RAM8(0x010Au) = running;
    if ((control & 0x08u) != 0u)
        ++RAM8(0x010Eu);
}
/* Step-94 implementations follow the already-translated MAF/fuel helpers. */
