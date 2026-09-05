# BUA / GM 1227165 Step-113 retained-memory startup project

This retains the modular frozen Step-104 C89 PC harness, independent Step-105
transmission-aware driving scenario, and Step-106 listing correction.  Step
107 begins the listing-backed translation of Major Segment D diagnostics,
Step 108 translates its complete `LE4F7..LE75C` qualification chain, and Step
109 completes the field-service and flash-code routes and scheduler wiring.
Step 110 audits the remaining executable image and completes the listing-exact
Segment-1 MPU/I/O output staging at `$EDA3..$EF03`. Step 111 translates and
scheduler-wires the key-off front at `$D6D1..$D769`, the LF447 block-learn
commit, the IAC reset/park state machine at `$D370..$D3DB`, and a thin HAL
event at the software-powerdown endpoint.
Step 112 adds a listing-only, source-ordered model of `$C800..$C9F3`, including
the normal scheduler handoff and explicit boundaries for factory test, the
optional HUD ROM, SWI wait, MPU peripherals, and 8192-baud service. It does
not use `bua-hac.txt` as firmware evidence.
Step 113 translates the exact LF3A7 retained-error checksum and LF434 BLM/SAM
initialization, corrects the live LF434 default from historical decimal 120 to
the emitted `$80` (128), and preserves the older value only inside explicit
Step-104/105/109 signature replays.

The project targets the 1986 Corvette L98 GM P4 ECM, service number 1227165, using the
supplied 9340 / 16059335 PROM material.  `BUA` remains a source label, not an
independently proven calibration identity.

## Build

The project deliberately builds as one translation unit:

```text
gcc -std=c89 -Wall -Wextra -pedantic main.c -o build/bua_step113
```

The included implementation fragments must not be compiled separately.
`main.c` includes them in the frozen declaration order so all existing
file-local (`static`) linkage and arithmetic behavior remain unchanged.

- Windows compile: run `build_gcc.bat`
- Windows compile and run: run `build_run_gcc.bat`
- Linux/macOS compile and run: run `sh build_gcc.sh`
- Make: run `make test`

For OnlineGDB, import the complete project tree and compile only `main.c`.
Every source fragment is well below the previously observed approximately
11,000-line editor/save limit.  `main.c` itself contains `int main(void)`.

## Directory layout

| Path | Purpose |
| --- | --- |
| `main.c` | Readable include map and program entry point |
| `src/` | Translated ECM algorithms, scheduler, RAM/MPU model, and feature wiring |
| `simulation/` | Original scripted drive plus the PC-only transmission-aware plant |
| `tests/` | Regression models and regression functions |
| `docs/` | Frozen evidence and Step-106/107 listing-backed audits |
| `docs/reference/` | Living evidence register, hardware/firmware cross-reference, MEMCAL model, and theory-of-operation index |
| `build/` | Generated executables, output, and compiler diagnostics |

Files ending in `.inc.h` are implementation fragments, not independent public
headers.  The naming makes that constraint explicit while preventing online
compilers from treating them as separate C source files.

## Behavioral baselines

- Frozen Step-104 behavioral signature: `4BA6B7C6`
- Step-105 transmission trace signature: `9732D09B`
- Step-106 listing-coolant regression: 12/12
- Step-107 diagnostic-stage regression: 17/17
- Step-108 diagnostic-qualification regression: 22/22
- Step-109 integrated Segment-D regression: 40/40
- Step-109 integrated Segment-D signature: `F357A5F2`
- Step-110 Segment-1 output regression: 22/22
- Step-110 integrated output-stage signature: `FAADF8A6`
- Step-111 ignition shutdown/IAC homing regression: 40/40
- Step-111 ignition lifecycle signature: `16D17C9C`
- Step-112 source-ordered reset/startup regression: 19/19
- Step-113 retained-memory/startup regression: 10/10
- Strict C89 compile: no warnings
- Integrated drive regression: 26/26
- Step-104 freeze regression: 10/10
- Step-105 transmission-aware regression: 12/12

The original Step-104 scenario remains present and unchanged.  Step 105 runs
after a fresh reset and derives Drive RPM from vehicle speed, selected ratio,
converter slip, and physical TCC state.  Its shift points, slip, and combined
axle/tire factor are explicitly simulator parameters, not factory claims.

The listing proves that LF42A is an RTS at $F42A.  It also proves that the
assembled ERR14/15 fallback at $F418 branches to LF42A; the `COOLS8` spelling
in the supplied text is not an unresolved executable destination.

See `docs/STEP113_RETAINED_MEMORY_AUDIT.txt` for the listing-backed checksum,
RAM-recovery and LF434 correction. Earlier audit files remain as historical
checkpoints.

Normal simulator execution now runs Segment D once per 16 ordinary IRQs. The
PC-only `sim_legacy_segment_d_freeze` switch is enabled only inside the frozen
Step-104 and Step-105 replay scenarios, so their historical signatures remain
comparable. It is not source RAM and is not part of the target ECM behavior.

Normal simulator execution now also performs Segment 1's raw writes to MPU
addresses `$3FCC/$3FD2/$3FD4/$3FD6/$3FD8` and parallel-I/O `$4004`. Electrical
polarity and the physical output devices remain HAL responsibilities. The
PC-only Segment-1 replay switch is used only by frozen historical signatures.

Ordinary odd minor loops now run the literal ignition-on/key-off front before
the existing load/fuel chain. Ordinary even minor loops run the IAC homing
producer after reference/RPM/spark work, while the existing motor consumer
continues every 6.25 ms. At `$D6EA`, the PC HAL records one software-powerdown
event and stops subsequent IRQ dispatch; physical keep-alive power remains a
hardware integration responsibility.
