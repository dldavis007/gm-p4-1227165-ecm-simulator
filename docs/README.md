# Documentation hub

This is the entry point for the GM P4 1227165 simulator documentation. Use the
current-state documents for orientation and the numbered step audits as an
immutable development trail.

## Start here

- [Current project status](CURRENT_PROJECT_STATUS.md) — what is complete, what
  remains bounded, and what can legitimately start new work.
- [Simulator, build, and testing guide](SIMULATOR_BUILD_TEST_GUIDE.md) — how to
  build, run, verify, use the raw HAL, and interpret the results.
- [Step history index](STEP_HISTORY_INDEX.md) — navigable Steps 104-173 record.
- [Technical-reference foundation](reference/README.md) — integrated system,
  firmware, hardware, MEMCAL, theory and migration chapters.
- [Step 171 whole-image reclosure](STEP171_WHOLE_IMAGE_RECLOSURE_AUDIT.txt) —
  final behavioral-port closure basis.

## Build and verification

From the repository root:

```text
make test
```

This compiles only `main.c` with `gcc -std=c89 -Wall -Wextra -pedantic`, runs
the full regression program, checks every Makefile-gated signature/result, and
rejects genuine `FAIL` output. The implementation fragments under `src/`,
`simulation/`, and `tests/` are included by `main.c`; they are not independent
translation units.

See [Simulation and verification](reference/SIMULATION_VERIFICATION_THEORY.md)
for the distinction between regression evidence and physical-hardware fidelity.

## Evidence and interpretation

- [Evidence register](reference/EVIDENCE_REGISTER.md)
- [Hardware/firmware cross-reference](reference/HARDWARE_FIRMWARE_CROSS_REFERENCE.md)
- [Theory-of-operation index](reference/THEORY_OF_OPERATION_INDEX.md)
- [C-port architecture and embedded migration](reference/C_PORT_ARCHITECTURE_EMBEDDED_MIGRATION.md)

The verified assembled listing `evidence/firmware/bua-hac.lst` is authoritative
for firmware behavior. `bua-hac.txt` is excluded. Photographs and direct
physical inspection control legible as-built MEMCAL values and jumper
population; KiCad and LTspice are supporting evidence.

## Historical records

Files named `STEPxxx_*.txt` document what was established at that checkpoint.
Later corrections are recorded as explicit addenda or later audits. Do not
silently rewrite an old audit to make it appear that a later fact was already
known.
