# BUA / GM 1227165 Step-105 transmission-aware project

This extends the modular frozen Step-104 C89 PC harness with an independent
Step-105 transmission-aware driving scenario.  It targets the 1986 Corvette
L98 GM P4 ECM, service number 1227165,
using the supplied 9340 / 16059335 PROM source.  `BUA` remains a source label,
not an independently proven calibration identity.

## Build

The project deliberately builds as one translation unit:

```text
gcc -std=c89 -Wall -Wextra -pedantic main.c -o build/bua_step105
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
| `docs/` | Step-104 freeze evidence plus Step-105 output and audit |
| `build/` | Generated executables, output, and compiler diagnostics |

Files ending in `.inc.h` are implementation fragments, not independent public
headers.  The naming makes that constraint explicit while preventing online
compilers from treating them as separate C source files.

## Behavioral baselines

- Frozen Step-104 behavioral signature: `4BA6B7C6`
- Step-105 transmission trace signature: `9732D09B`
- Strict C89 compile: no warnings
- Integrated drive regression: 26/26
- Step-104 freeze regression: 10/10
- Step-105 transmission-aware regression: 12/12

The original Step-104 scenario remains present and unchanged.  Step 105 runs
after a fresh reset and derives Drive RPM from vehicle speed, selected ratio,
converter slip, and physical TCC state.  Its shift points, slip, and combined
axle/tire factor are explicitly simulator parameters, not factory claims.

See `docs/STEP105_TRANSMISSION_AUDIT.txt` for the verification record.
