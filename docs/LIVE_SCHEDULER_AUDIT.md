# Live scheduler and ROM audit

The C model reaches the translated running spark chain through the normal
6.25 ms IRQ's even dispatch. The ROM takes the stall-saver branch after RPM
filtering and before dwell. Startup qualification and stopped operation branch
to LD306. The no-reference timeout writes $FFFF to $3FC0 and enters the
modeled SWI boundary. The counter comparison is before the increment.

## Reachability

Run `python3 tools/audit_scheduler.py` with GCC and gcov to reproduce the
per-function inventory. The script compares the GCC call graph rooted at
`irq_6p25ms` with coverage from 1,024 IRQs across stopped, 800 and 2,400 RPM
inputs. Coverage starts after reset. Only functions actually executed on the
scheduler path receive live-translation credit.

| Classification of definitions in src/ | Count |
|---|---:|
| Observed on the IRQ path | 164 |
| Static IRQ path, unobserved in this scenario | 38 |
| Outside the IRQ call graph | 104 |

The observed set includes both spark lookups, advance sum, knock attack and
major-segment recovery, Mode-4 blend, LD1F6 output, EST finish, stall-saver
and LD306. Outside the IRQ graph are the independent SCI message handlers,
the standalone C42D base-fuel lookup, reset entries and host-facing HAL
setters. Some have legitimate separate entry points. These counts describe C
functions. They cannot establish what fraction of ROM instructions executes
or whether every branch of an observed function is correct.

## Independent ROM runs

The reference is the 16 KiB assembled `evidence/firmware/bua-hac.bin`,
mapped at $C000, SHA-256
`302cd1d2cc3ecdf7cf097085c426b9f609dfeb7a6ac8131f268cde10fbb12fc2`.
The comparison uses the Python HC11 instruction emulator at the following pin.

```sh
git clone https://github.com/anarkiwi/m68hc11 ../m68hc11
git -C ../m68hc11 checkout 9856198c1a8c5ce5e3c2428a4c332808307998a4
PYTHONPATH=../m68hc11 python3 tools/compare_spark_rom.py
PYTHONPATH=../m68hc11 python3 tools/compare_spark_rom.py --full-irq
PYTHONPATH=../m68hc11 python3 tools/compare_spark_rom.py --drive-irq
PYTHONPATH=../m68hc11 python3 tools/compare_spark_rom.py --all --coverage
```

- Running spark. Both implementations retain their own state for 16 ticks
  from LCD4B through the return from the LD365 FMD read. RAM below $0500
  (excluding the CPU stack window $01C0–$01FF), the entire MPU backing,
  each ordered MPU byte write, and the count of FMD reads match each tick.
  Inputs exercise all four Mode-4 spark commands, knock-counter changes,
  EST bypass, stall-saver and long reference periods.
- Branch and arithmetic checks. The startup Error 34 test covers MAF delta
  84, the equality boundary 85 and ignition-off inhibition. The timeout test
  checks the two bytes written before the SWI. The LF250 comparison runs 45
  old-value, input and coefficient combinations, including zero.
- Full IRQ. Both sides begin with the C reset RAM, MPU and I/O backing. The
  ROM runs from the IRQ vector to the wait loop, including the intervening
  serial IRQs when armed. The harness supplies timer IRQ status, immediate
  SPI completion and host ADC/FMD values. The stopped scenario matches
  all ordered MPU byte writes on 32/32 ticks. The second scenario supplies
  a valid VATS capture and a stopped, 300 RPM, then 1,100 RPM reference
  signal. A third, 512-tick scenario shifts into drive and supplies a 40 mph
  vehicle-speed input. On both driving traces the ROM and C first flag the
  engine running at tick 116. All three traces agree on non-stack RAM through
  $04FF and all ordered MPU writes on every tick. Both driving inputs supply
  reference captures and VATS periods through modeled MPU registers. The
  emulator adapter returns $FFFF for FDIV overflow per M68HC11RM.
- Cold power-on. The ROM starts at its reset vector with zero-filled memory.
  At $CCC6 its non-stack RAM, MPU and modeled I/O match the C power-on
  lifecycle entry. The historical `ecm_reset()` fixture used to seed the
  full IRQ replays differs from cold startup at 29 addresses. Reset checks
  final register state; intermediate reset write order is unverified.
- Targeted replays. Major-segment dispatches and routine slices use equal
  sensor inputs and RAM preloads and compare non-stack RAM, MPU state and
  ordered MPU writes at their endpoints. Cases include acceleration and PE,
  warm O2 feedback, BLM cell learning, idle PID and homing, shutdown, warm
  TCC lock/unlock, EGR, purge, outputs, MAT faults, code flashing, Mode 4 and
  factory control. The BLM threshold case retains state for 64 additional
  services. SCI runs receive and transmit IRQs for Modes 0–4 and checks
  every response byte against the C packet handler, plus bad-checksum
  rejection. Intermediate per-byte SCI scratch is unverified.

### Executed ROM instructions

`--coverage` counts unique executed instruction addresses against instruction
starts in the assembled listing. It verifies the listed instruction bytes
against the image and groups them into contiguous regions at routine entries,
dispatch targets and selected functional boundaries. Shared tails count once
at their ROM address. Repeated ticks do not increase the count.

| Replays added in command order | New instructions | Cumulative |
|---|---:|---:|
| Running spark and arithmetic, stopped, crank-to-idle | 3,000 | 45.47% |
| 512-tick road input | 237 | 49.06% |
| Mode 4 timeout | 25 | 49.44% |
| Stopped engine with high airflow | 32 | 49.92% |
| Independent reset vector | 213 | 53.15% |
| Diagnostic qualification and retained-code logging | 185 | 55.96% |
| Targeted segments, IAC motion, warm fuel, spark/reference slices, factory and SCI | 1,879 | 84.43% |

The suite executes all 275 instruction starts in the $E4F7 qualification routine.
Run `--faults` to check the pre-latch and latch passes of nine faults, counter
decay and inhibit paths, and single-pass Error 41 and 43 inputs. A separate
52-service Segment D replay checks Error 21 persistence, retained checksum,
and the active-low lamp command. Other input combinations remain untested.

The IAC request routine at $E87A reaches 204 of its 211 instructions.
The seven remaining instructions handle saturation. In this image the steering
addends are zero, throttle demand is capped at 75, learned A/C adds at most 32,
and moving in drive adds five. The maximum request before subtracting the
current follower position is 112, below the positive 127-step clamp. Tests
also cover negative deltas, fan and cold anticipation, and Mode 4. A retained
throttle-opening and release trace compares non-stack RAM, MPU state and
ordered MPU writes after each of 97 motor and 49 request services.

In the fuel region starting at $DA2D the suite executes 370 of 377 instructions.
For 128 successive 12.5 ms fuel services, the replay supplies a lean and then
rich ADC input, and sets the slow O2 sample produced outside the routine.
The integrator moves from 128 to 130 and then to 127. Non-stack RAM, MPU
state, and ordered writes including the injector pulse word at $3FD0 match
after every service. Separate cases check feedback inhibits, slow O2
deadband, proportional-step suppression, and invalid retained BLM recovery.

The $CDE6 region reaches 497 of 517 instructions. Running-reference cases
exercise high RPM, dwell clipping, hot restart, highway spark, Error 43
knock feedback and power enrichment. The even spark path calls LFA1E and
services the TCC pass-by counter. With the stock $FFFF qualification delay,
the counter wraps and cannot start a hold. A separate four-service replay
preloads an active hold and checks voltage inhibition, the $DFFF write to
$3FD6, hold expiry and the next qualifying pass.

The fuel transient region at $D86B reaches 163 of 164 instructions. A retained
24-service replay compares non-stack RAM, MPU state and ordered writes on
every pass. Fuel stays on during all 20 qualification passes, cuts for two
passes, and resumes with stall saver active when engine speed falls to 900 RPM.
Separate cases cover the Mode 4 AFR override, including a zero divisor.
The remaining instruction at $D915 selects single fire on cutoff entry
when calibration bit 1 at $C014 is set. The stock image clears this bit.

Segment 1 at $EDA3 reaches 143 of 153 instructions. Separate dispatches
exercise each Mode 4 output override, the first and repeated high-voltage
conditions, and stopped diagnostic outputs. Each compares non-stack RAM,
MPU state, ordered MPU writes and the fan latch at $4004. The ROM complements
purge duty for both normal and Mode 4 commands. A Mode 4 duty of $55 produces
$D2AB for purge and $D157 for EGR. The ten unexecuted instructions select
alternate A/C and shift-lamp outputs disabled by the stock $C017 and $C014.

All 84 instructions in the factory output region at $FDED execute. Starting
at factory service entry $FCA2, 96 consecutive passes retain state through
the minor-counter rollover. Reference periods of 327, 328, 655 and 656
exercise both sides of the fuel and dwell thresholds. A 97th pass simulates
ignition restoration and an FMD mode change, then compares the asynchronous
pulse, checksum and shutdown outputs. Each pass compares non-stack RAM,
MPU state, ordered MPU writes, and final I/O registers $4002, $4004, $4006,
$400B and $400C. Serial IRQ dispatch before $FCA2 is outside this trace.

The idle-speed PID region at $D59A reaches 152 of 153 instructions. The
remaining instruction at $D684 caps a rounded product at 127. With the stock
quantizer constant of 8 and an eight-bit remainder, this product rounds to
at most 8. The replay starts at $D370, compares non-stack RAM, MPU state
and ordered MPU writes, and stops at $D6CE before major-loop dispatch.
An 18-pass trace retains the RPM history and integrator while speed dips
below and rises above an 800 RPM setpoint. Additional cases exercise signed
overflow, fractional RPM, derivative gain, the drive and neutral deadbands,
throttle and gear transitions, and the learned warm motor-position limits.

Fifteen Segment 5 dispatches exercise A/C timer expiry, cutouts, and IAC
learning through the fan and closed-loop paths. Together with the existing
suite they execute all 120 instructions at $DF9D. In park/neutral the ROM
uses $C66A (128) to scale measured position travel and $C664 (255) to
scale the signed change applied to the accumulator at $0105.

The injector replay enters at $F67B and stops at the airflow service ($F76A).
It checks all 256 startup coolant inputs, then retains state across 64 ticks
with 48 reported injections. It compares non-stack RAM, MPU state and the
empty MPU write trace after each service. These inputs execute 103 of 104
instructions. The underflow clamp at $F6CF is unreachable here because the
test at $F695 requires enrichment of at least 256 and the decay amount is
one byte. This replay supplies the event indication independently of its
upstream physical generation.

The Mode 4 sweep checks all 256 command bytes through entry, a changed
command while active, and exit. A 32-tick full IRQ replay checks the serial
timeout clearing $0151 on rollover and the Mode 4 latch clearing one tick
later. Another 32-tick IRQ replay checks the stopped-engine MAF timer at
$00B0 advancing from 95 through 97, then setting the high-MAF latch at $003F.
The $CB5F region includes the following scheduler code and background RAM
refresh loop. Its total is 138 of 173 instructions.

Six Segment E replays retain state across 72 dispatches apiece, comparing
non-stack RAM, MPU state and ordered writes after every dispatch. At 25 mph
they check the N/V ratio byte directly around the 30, 35 and 41 thresholds.
The replays also exercise fourth-gear confirmation, the coolant threshold,
and clutch command changes. A separate case exercises the pass-by hold.
The $E10F region now reaches 189 of 205 instruction starts.

Of 6,598 listing instructions, 1,027 remain unexecuted. The largest remaining
regions by unexecuted instructions begin at $E75D (58), $EF04 (56) and $C800 (56). A hit records execution under the scenario inputs.
The comparison boundaries above determine what behavior each replay verifies.

## C regression and calibration coverage

`make test` compares 51 compiled calibration tables with fixed addresses
in the assembled ROM, including strided MAF scalars. Standalone scalar
constants and other images are outside this table check. The eighth injector
offset byte is $01 at $C3C3. The historical drive, transmission and Segment-D
signatures changed with live spark and startup execution. Their current values
are 02BA4EA7, 5636155D and B12D3084 respectively. These are C regression
signatures. The ROM comparison is independent.

The running RPM and dwell paths follow ROM arithmetic for $0000 and $FFFF
reference captures. Reference capture and ASLD wrap to 16 bits. The SWI
wait remains a simulator boundary. FMD transactions use the processor-facing byte boundary. SPI bus
timing, stack traffic and physical ignition timing remain unverified.

### First divergent stores in the original drive comparison

| First tick | RAM | ROM store(s) on that tick | Cause |
|---:|---|---|---|
| 1 | $64 | $F1CE | ADC transfer scratch and the order of MAF and TPS reads |
| 1 | $F3, $010F | $F5B0, $F678 | Park/neutral state and signed IAC base bias |
| 2 | $30 | LF21A toggle before the LF1E5 selector update | COP2 bit 1 |
| 12 | $A4 | $EB96 | Error 43 sample counter |
| 17 | $0109, $010C | $ECE6, $ECDE | Warm-park interpolation rounding |
| 99, 109 | $0127, $0128 | $D814 (word) | Emulator FDIV overflow quotient |
| 117 | $2C, $0101, $0104, $010A | $F604, $F5EC, $F5AD, $F5F6 | Startup IAC target and motor service |
| 122 | $F2, $FF, $0102 | $D470, $D6CC, $D450 | Idle-speed command and RPM history |
| 172 | $02 | $EB8D | Error 43 test-in-progress bit |

To inspect a regression, add `--trace-tick N` to the IRQ scenario. It prints the ROM writes on that tick for RAM addresses still unequal at the end of the tick.
