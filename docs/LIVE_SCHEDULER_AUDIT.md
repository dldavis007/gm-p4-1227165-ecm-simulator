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
| Observed on the IRQ path | 131 |
| Static IRQ path, unobserved in this scenario | 37 |
| Outside the IRQ call graph | 117 |

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
  SPI completion and host ADC/FMD values. This stopped-engine scenario
  diverges on all 32 ticks. Tick 1 differs in 19 non-stack RAM bytes. The ROM
  writes $FB02 to $3FFC before writing zero to $3FD0. C performs only the
  latter write. The command exits unsuccessfully while differences remain.
  Some differences can reflect peripheral or harness assumptions. Separate
  those from translation gaps before assigning an instruction-level cause.

## C regression and calibration coverage

`make test` compares the 48 compiled `cal_*` byte tables with fixed addresses
in the assembled ROM, including strided MAF scalars. Standalone scalar
constants and other images are outside this table check. The eighth injector
offset byte is $01 at $C3C3. The historical drive, transmission and Segment-D
signatures changed with live spark and startup execution. Their current values
are 6B024BFD, 09D73D61 and 316BC768 respectively. These are C regression
signatures. The ROM comparison is independent.

Simulation boundaries include the host period-valid flag, invalid capture
sentinels in RPM/dwell and the SWI wait. Reference capture and ASLD now wrap
to 16 bits. FMD transactions use the processor-facing byte boundary. SPI bus
timing, stack traffic and physical ignition timing remain unverified.
