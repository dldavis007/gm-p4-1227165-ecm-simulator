# Simulator, build, and testing guide

## Purpose

This guide explains how to build and run the current C89 PC model, what the
standard regression command checks, how the supplied scenarios and raw HAL fit
together, and how to interpret results without confusing simulation with
original-ECM electrical fidelity.

For the completion statement and remaining boundaries, read
[Current project status](CURRENT_PROJECT_STATUS.md) first.

## Repository model

The project intentionally builds as one C translation unit. Compile `main.c`
only. It includes the ordered fragments under:

- `src/` — translated ECM algorithms, memory/register model and HAL-facing code;
- `simulation/` — deterministic PC-only stimulus and plant models; and
- `tests/` — focused, scheduler, lifecycle and integration regressions.

Files ending in `.inc.h` are implementation fragments. Do not compile them as
independent source files. Their include order and file-local `static` linkage
are part of the portability and regression contract.

## Requirements and compiler contract

- A C89 compiler; the verified host compiler is GCC.
- `make`, `sh`, or Windows command prompt for the chosen workflow.
- No external C libraries beyond the standard library.

The verified flags are:

```text
-std=c89 -Wall -Wextra -pedantic
```

Warnings are treated as regression signals.

## Complete verification

From the repository root in an environment with GNU Make and GCC:

```text
make clean
make test
```

`make test`:

1. builds `main.c` as `build/bua_step121`;
2. captures output in `build/bua_step121_output.txt`;
3. checks every frozen signature and PASS string listed in the Makefile;
4. rejects genuine `FAIL` lines while allowing historical `FAIL 0` summaries;
5. returns nonzero if a gated check fails.

Successful output ends with the Makefile summary that the Step-104/105
baselines, Step-106 through Step-121 regressions, and Step-128/155-170
HAL/manager regressions pass.

GitHub Actions runs the same Strict C89 gate on configured repository events. A
green workflow proves that the commit passed this PC gate; it does not prove
physical ECM equivalence.

## Other supplied build paths

### Linux or macOS

```text
sh build_gcc.sh
```

This builds, runs, and writes diagnostics and output under `build/`. It does not
perform every Makefile `grep` assertion, so `make test` remains the acceptance
command when Make is available.

### Windows GCC

Build only:

```text
build_gcc.bat
```

Build and run:

```text
build_run_gcc.bat
```

The executable is `build\bua_step121.exe`; captured output is
`build\bua_step121_output.txt`.

### Direct compiler command

```text
gcc -std=c89 -Wall -Wextra -pedantic main.c -o build/bua_step121
```

Create `build/` first if necessary, then run `build/bua_step121`.

### OnlineGDB

Import the complete repository tree and compile only `main.c`. Preserve the
`src/`, `simulation/`, and `tests/` relative directories. Do not combine or
compile `.inc.h` files separately. Because browser editors may have project-size
or persistence limits, local GCC or GitHub Actions is preferred for the
authoritative complete run.

## What one program run contains

`main.c` is an automated harness, not an interactive simulator. A run:

- initializes the ECM and cluster-facing model;
- prints scheduler, VSS, RPM, spark and 160-baud demonstrations;
- executes focused algorithm and boundary regressions;
- exercises scheduler, startup, factory, diagnostic, shutdown and lifecycle
  integration;
- runs the 160-baud and 8192-baud communication regressions;
- runs the deterministic Step-104 normal-operation drive; and
- resets state and runs the independent Step-105 transmission-aware scenario.

The long output is expected. For acceptance, rely on process status, Makefile
checks and exact signatures rather than visually scanning only the final lines.

## Verification layers

| Layer | Purpose | What a PASS establishes |
| --- | --- | --- |
| Focused regressions | Arithmetic, tables, state machines and boundaries | The asserted behavior passes for the defined vectors. |
| Scheduler/integration | Source order, cadence and reachable wiring | The tested producer/consumer path is connected in the modeled scheduler. |
| Lifecycle | Reset, startup, run, shutdown, retained restart and factory isolation | The tested phases compose in established order. |
| Deterministic scenarios | Multi-subsystem behavior across a repeatable trace | The defined PC stimulus produces the expected assertions and signature. |
| Frozen signatures | Broad change detection over selected state and traces | The serialized state covered by the signature is unchanged. |

A focused PASS is not evidence for untested hardware semantics. A frozen hash
is a change detector, not proof that every included value is physically exact.

## Protected signatures

| Checkpoint | Signature |
| --- | --- |
| Step 104 normal operation | `4BA6B7C6` |
| Step 105 transmission trace | `9732D09B` |
| Step 109 integrated diagnostics | `F357A5F2` |
| Step 110 output stage | `FAADF8A6` |
| Step 111 ignition lifecycle | `16D17C9C` |
| Step 160 full raw-HAL lifecycle | `2E92563C` |
| Step 161 raw-HAL output lifecycle | `B22B98ED` |
| Step 162 raw-VOLT output safety | `DB86F332` |
| Step 163 raw-HAL power-on | `040E3A8B` |
| Step 164 raw-HAL factory IRQ | `0194B153` |
| Step 165 raw-HAL factory A/D | `0EACFF4F` |
| Step 166 8192-baud SCI raw HAL | `52396771` |

Do not update a signature merely to make a failure disappear. An intentional
change requires exact supporting evidence, a focused regression, and a record
of the behavioral difference.

## Deterministic scenarios

### Step 104 normal-operation drive

`simulation/dynamic_drive.inc.h` drives processor-facing speed, reference,
throttle and airflow stimuli through a repeatable profile. It checks interaction
among engine-run qualification, load, fuel, acceleration enrichment, BLM, VSS,
RPM, DFCO, injector service and idle behavior. Historical replay switches keep
the frozen comparison isolated from later corrected live behavior.

### Step 105 transmission-aware drive

`simulation/transmission_drive.inc.h` derives Drive RPM from vehicle speed,
selected gear, converter slip and physical TCC state. Gear ratios, shift points,
slip, smoothing and axle/tire factor are PC-only plant assumptions unless
independent evidence establishes otherwise.

Neither scenario claims factory-accurate vehicle dynamics, exhaust response,
sensor noise, transmission hydraulics or engine torque response.

## Raw HAL inputs

`src/hal_interface.inc.h` exposes raw processor-facing setters for:

- U10 inputs: MAP2, VOLT, O2, MAP, CTS, TPS, PUMPVOLT, DIAG, MAT, ESC and MAF;
- the processor-visible U9 knock counter;
- two normal-operation FMD reply bytes;
- VSS MPH and reference-RPM pulse-source helpers; and
- bounded low RAM for tests requiring an established raw byte.

MPH and RPM setters are simulator conveniences that generate pulse timing; they
do not make their scaling exact hardware facts. MAP2, MAP and ESC are present
because factory test establishes their selectors; the supplied normal image
does not explicitly request those three channels.

Do not inject final processed state when translated acquisition or conversion
is the behavior under test.

## Raw observations and communications

Observers expose bounded low RAM, U9 `$3FC0-$3FFF`, `$4000-$400F`, `$5000`,
injector pulse-width counts, IAC position, and established outputs `$3FCC`,
`$3FD2`, `$3FD4`, `$3FD6`, `$3FD8`, and `$4004`.

The 8192-baud boundary accepts a complete raw message and exposes status,
response length and response bytes. The 160-baud boundary initializes the
translated manager, advances one listing-equivalent cell at a supplied timer
origin, and exposes raw line, table-index, bit-count and transmit-byte state.

These APIs do not model external voltage polarity, transceivers, connector
loading, collision behavior or a target timer driver.

## Failure handling

When `make test` fails:

1. inspect the first failed `grep` or genuine `FAIL` line;
2. inspect nearby output in `build/bua_step121_output.txt`;
3. classify it as a compiler, assertion, integration or signature failure;
4. compare the change with the authoritative listing or applicable evidence;
5. fix the cause and rerun the entire gate.

Do not delete a check, weaken an assertion, or refresh a baseline without an
evidence-supported reason.

## Fidelity boundary

The simulator is strongest at deterministic behavioral comparison. It does not
establish exact analog transfer, electrical timing, custom-device internals,
injector or ignition electrical dynamics, transmission hydraulics, exhaustive
formal path coverage, or code absent from `$5800` and `$6000`.

See [Simulation and verification](reference/SIMULATION_VERIFICATION_THEORY.md)
for deeper rationale and
[C-port architecture and embedded migration](reference/C_PORT_ARCHITECTURE_EMBEDDED_MIGRATION.md)
before replacing PC stimulus or observation with target hardware.
