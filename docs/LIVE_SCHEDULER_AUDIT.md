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
| Observed on the IRQ path | 161 |
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
| Independent reset vector | 213 | 52.29% |
| Diagnostic qualification and retained-code logging | 185 | 55.09% |
| Targeted segments, IAC motion, warm fuel, spark/reference slices and SCI | 1,250 | 74.04% |

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

Of 6,598 listing instructions, 1,713 remain unexecuted. The largest remaining
regions by unexecuted instructions begin at $D86B (81), $EDA3 (77) and $FDED (75). A hit records execution under the scenario inputs.
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
