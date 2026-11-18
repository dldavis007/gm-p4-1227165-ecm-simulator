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
| Observed on the IRQ path | 146 |
| Static IRQ path, unobserved in this scenario | 37 |
| Outside the IRQ call graph | 105 |

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
  signal. It matches all ordered MPU writes on 256/256 ticks. Both set the
  running flag on tick 116. On every tick of both scenarios the fuel words
  $C2 and $D9, FMD byte selector and serial interrupt flags also match.
- Both full IRQ scenarios compare every byte below $0500 except the CPU
  stack window $01C0–$01FF, including the ADC receive scratch at $64.
  LF651 base bias, warm park, the startup motor trajectory, commanded idle,
  COP2 and Error 43 now agree across these traces. The emulator adapter
  corrects FDIV overflow to return $FFFF per M68HC11RM. This matters on
  drive tick 99 ($0999 divided by $06FE). Other idle PID operating regions,
  physical bus timing and warm transmission behavior need separate traces.

## C regression and calibration coverage

`make test` compares 51 compiled calibration tables with fixed addresses
in the assembled ROM, including strided MAF scalars. Standalone scalar
constants and other images are outside this table check. The eighth injector
offset byte is $01 at $C3C3. The historical drive, transmission and Segment-D
signatures changed with live spark and startup execution. Their current values
are F56CA262, C8414C20 and B12D3084 respectively. These are C regression
signatures. The ROM comparison is independent.

Simulation boundaries include the host period-valid flag, invalid capture
sentinels in RPM/dwell and the SWI wait. Reference capture and ASLD now wrap
to 16 bits. FMD transactions use the processor-facing byte boundary. SPI bus
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
