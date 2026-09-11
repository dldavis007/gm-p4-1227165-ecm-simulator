# Idle-air control

## Purpose and evidence boundary

This chapter integrates the established Idle Air Control (IAC) target, feedback, command-quantization, motor-service, anticipation, startup, and key-off homing behavior for the supplied BUA/9340 image.

The evidence boundary remains explicit:

- listing-backed target construction, state, gains, quantization, command words, scheduler cadence, position bookkeeping, and homing are F0;
- visible IAC driver nets and motor connection are F1;
- raw host-side load/disturbance inputs and observable software commands are F2;
- throttle-airflow, bypass-airflow, engine torque response, stepper mechanics, friction, and RPM plant response are F3;
- undocumented driver electrical polarity/current behavior remains F4.

The firmware control law can therefore be described and simulated without claiming undocumented motor-driver or engine-airflow physics.

## Overall control architecture

The evidence-supported normal idle path is multirate:

1. firmware constructs a commanded idle-speed target;
2. the closed-loop IAC regulator executes at a source-proven 50-ms cadence;
3. its output is converted to a signed/directional packed movement request;
4. the IAC motor service runs every ordinary 6.25-ms IRQ and consumes at most one physical step per service opportunity;
5. present IAC position, phase, direction, and related follower state are updated as each requested step is consumed;
6. the engine responds to actual bypass-air motion through the external plant;
7. the next 50-ms regulator update sees the resulting RPM/error state.

This distinction between a 50-ms command producer and a 6.25-ms motor executor is central to the reconstructed behavior. Large regulator commands persist across the intervening motor-service passes rather than moving the valve instantaneously.

## Commanded idle-speed construction

The translated `$D41F-$D450` path constructs the commanded idle speed from the coolant target-RPM table and operating-mode additions.

The established behavior includes:

- coolant-indexed target RPM using the `C637` table with exact 16-count interpolation;
- a +50 RPM Park/Neutral target addition;
- an optional +300 RPM hot-spark-related target addition where the source condition enables it;
- A/C and fan handling in this calibration primarily through step-position anticipation/load compensation rather than simple additions to the target-RPM value at this point in the algorithm.

This separates the desired-speed calculation from the actuator-side anticipatory additions described below.

## Neutral/Drive state and minimum-position learning

The IAC control path includes transmission-state-specific behavior rather than one universal idle gain/state.

The translated `$D4DD-$D53B` logic establishes:

- a Neutral-to-Drive transition delay that inhibits normal PID response for the calibrated interval;
- reset of the relevant deadband/timing state during the transition;
- a drive-only learned minimum-IAC-position walk on its selected slower scheduler phase;
- separately translated higher-response/high-derivative behavior where its calibration enables it.

This allows the controller to preserve learned floor/transition behavior while avoiding an immediate ordinary PID reaction to a mode shift.

## Closed-loop feedback law

The normal IAC feedback path was reconstructed from `$D59A-$D6AA` and later surrounding logic.

The control law uses RPM error and RPM-rate/derivative information, with Park/Neutral versus Drive deadbands and gains. Source arithmetic preserves sign, fixed-width wrap/saturation semantics, and the final bounded signed command behavior.

Important reconstructed points are:

- proportional and derivative terms retain their source signs/order;
- the small-error region uses calibrated low integral behavior;
- the historical interpretation of `$0103` as a conventional always-active integral term was corrected: it functions as fractional/quantization residual state in the small-output branch and is cleared by sufficiently large direct outputs;
- exact-zero behavior can clear the residual according to calibration;
- the final command is bounded before motor execution.

Accordingly, the implementation should not be simplified to an ordinary textbook PID with a continuously accumulating integral term.

## 50-ms regulator cadence

The executable scheduler proves the closed-loop regulator cadence is 50 ms, selected by the scheduler condition corresponding to `L0000 & 7 == 0`.

The 6.25-ms IRQ continues to service the motor between regulator updates. Thus one 50-ms regulator output can represent several requested motor steps that are physically consumed one at a time over subsequent 6.25-ms services.

This source-derived cadence replaces earlier assumptions that the feedback regulator itself ran at the faster IRQ rate.

## Fractional-step quantization and residual state

The `$D633-$D6B6` path preserves sub-step control effort rather than discarding all small commands.

The established behavior includes:

- calibrated low integral/fractional gains inside the RPM-error deadband;
- `$0103` residual feedback used in the fractional-output branch;
- direct/large outputs clearing the residual;
- fractional outputs retaining the residual so repeated small corrections can eventually produce a physical step.

This is important at warm stable idle, where desired corrections can be smaller than one motor step per 50-ms regulator period.

## Packed movement command and motor executor

The IAC command is stored as a packed direction/magnitude request, with the motor executor consuming one step at a time.

The translated motor-command path preserves:

- direction and remaining magnitude semantics;
- present-position bookkeeping at `$002C`;
- the source phase sequence `0, 1, 3, 2`;
- running/movement counters and associated follower/progress state;
- one-step consumption during each 6.25-ms service opportunity.

This proves the software stepping sequence and position accounting. It does not establish external winding-current polarity or electrical drive strength.

## Throttle follower

The IAC system is not solely an idle-speed feedback loop. The translated throttle-follower path constructs additional position demand during throttle movement and vehicle operation.

Established behavior includes:

- approximately 1.5 steps per TPS-unit slope in the translated arithmetic;
- a 75-step cap;
- Park/Neutral scaling;
- an additional moving-drive contribution;
- trajectory/decay timing that returns follower demand rather than dropping it instantly.

The follower state therefore acts as a feed-forward/trajectory contribution layered with the idle feedback system.

## A/C load anticipation and learning

A/C is integrated into IAC as an anticipatory load-compensation path. The normal calibration does not simply wait for RPM to fall and let the PID recover.

The source-shaped behavior includes:

- A/C request/compressor-state transitions reflected in IAC control words;
- anticipatory opening before/around compressor loading;
- coordinated removal of that extra position when the load is removed;
- learned A/C step demand with calibrated minimum/maximum bounds and limited positive learning rate;
- Park/Neutral scaling of the learned change;
- a learning delay before the step estimate is accepted/updated.

The `LE005-$E07B` continuation currently preserves the learned-step bounds, timer, learned position, accumulator, and P/N gain behavior.

Integrated PC regressions have used an external compressor-load disturbance only on the plant side while retaining this firmware anticipation logic. That separation is appropriate: compressor torque is F3, while anticipatory IAC commands are F0.

## Fan and other anticipation features

The IAC path also contains fan-anticipation/follower handling. The translated logic walks the associated anticipation state and can inhibit the ordinary PID until the anticipatory movement is complete.

Power-steering-pressure anticipation capability was also traced. In this specific 9340 calibration, the relevant additions are zero, which distinguishes implemented firmware capability from enabled calibration behavior.

These features should remain represented as calibrated feed-forward/anticipation paths rather than being silently deleted because a particular calibration disables part of them.

## Base-bias and minimum-position contributions

The reconstructed motor/control path includes base-bias error construction and learned/minimum-position contributions in addition to the primary RPM feedback error.

The listing arithmetic includes signed saturation and source-specific combination order. These terms help establish why IAC movement can occur without a simple one-to-one relationship between instantaneous RPM error and requested steps.

## Startup positioning

Startup IAC behavior includes warm-park positioning before normal closed-loop operation.

The translated startup path preserves:

- coolant-derived warm-park position state;
- an unconditional startup-position addition established by the emitted calibration/code path;
- battery-related inhibit/qualification where present in the source;
- command initialization of IAC state before the normal regulator takes control.

This means startup valve position is not assumed to equal the last running closed-loop position.

## Key-off reset and homing

The key-off lifecycle includes a deliberate IAC reset/homing sequence rather than immediate power removal.

The listing-backed `$D370-$D3DB` state machine, now live in the shutdown path, establishes:

1. a reset request under key-off or other qualifying conditions;
2. a close phase that drives the modeled position toward the hard stop;
3. software initialization of the position counter to `$FF` at reset entry;
4. continued one-step physical consumption through the normal 6.25-ms motor service;
5. after the closed stop is reached, an open phase toward the calibrated park position `LC62F = 144`;
6. reset-complete state and, when ignition remains on, reinitialization through the startup IAC state.

The surrounding shutdown lifecycle also preserves the key-off housekeeping interval, later software-powerdown boundary, and retained-state work. Physical keep-alive power remains outside the firmware-exact layer.

The close/open phase bits and the park value are executable facts. The actual mechanical hard-stop force, lost-step behavior, motor current, and whether every physical valve reaches exactly the same airflow at 144 software steps remain plant/electrical questions.

## IAC service during shutdown

Homing is selected on the even 12.5-ms branch, but its movement request is still consumed by the 6.25-ms motor executor. This preserves the same producer/executor separation used during normal control.

Consequently, a key-off homing command can require many ordinary IRQ services to complete. A simulator should not jump the IAC position directly from its running value to zero or park simply because the homing state machine advanced.

## Simulation architecture

The correct PC model is layered:

`firmware target/control (F0, 50 ms)`

`-> packed movement request (F0)`

`-> one-step motor service (F0, 6.25 ms)`

`-> simulated valve/airflow/engine response (F3)`

`-> RPM/reference feedback presented back to firmware (F2/F0 boundary)`

A/C/fan/power-steering disturbances likewise belong on the plant side, while their anticipation logic remains firmware-side.

This architecture prevents host timestep or motor-model convenience from rewriting the controller's actual cadence and state machine.

## C implementation and regression correspondence

The current C port already separates these responsibilities across the live translation:

- `src/ecm_core.inc.h` — commanded idle construction, regulator arithmetic, quantization/residual behavior, throttle-follower logic, motor execution, startup IAC state, and related shared helpers;
- `src/ac_control.inc.h` — A/C/IAC continuation and learned anticipatory step behavior;
- `src/ignition_shutdown.inc.h` — live key-off IAC homing/reset and restart boundary;
- scheduler integration — 50-ms regulator scheduling and 6.25-ms motor service;
- PC simulation — engine/load response retained on the plant side.

Existing closed-loop/IAC, A/C, scheduler, startup, drive-cycle, and ignition-shutdown regressions exercise these paths. Step 144 is documentation/theory integration and does not justify changing C source or frozen behavioral baselines.

## Evidence-supported theory of operation

1. Firmware builds a coolant- and mode-dependent target idle speed.
2. Transition/minimum-position and anticipation states modify how the controller is allowed to respond.
3. The normal IAC feedback regulator executes every 50 ms and preserves source-specific proportional, derivative, deadband, and fractional residual behavior.
4. Its result becomes a packed direction/magnitude movement request.
5. The motor service executes every 6.25 ms and consumes one requested step while updating phase and position state.
6. Throttle follower, A/C, fan, and calibrated auxiliary anticipation paths provide feed-forward position demand in addition to feedback regulation.
7. Startup logic initializes the valve to calibrated warm-park/start state before ordinary closed-loop control.
8. Key-off logic performs a hard-stop close/reset followed by reopening to park position 144 before eventual software powerdown.
9. Physical airflow per step and resulting engine RPM are plant behavior, not firmware facts.

## Remaining evidence needs

- Trace the external IAC driver schematic to document exact IACA/IACB/IACEN pin/register/output relationships and electrical polarity without inference.
- Bench-measure physical step direction, winding sequence, current, and valve displacement for one commanded software step.
- Measure bypass airflow versus physical step position and hysteresis if a high-fidelity engine-idle plant is desired.
- Verify hard-stop lost-step behavior and actual reopened airflow at software park position 144 on production hardware.
- Preserve calibration-specific zero/nonzero anticipation terms when comparing other PROM images rather than generalizing 9340 behavior.
