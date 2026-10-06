# Documentation hub

This is the entry point for the GM P4 1227165 simulator documentation. Use the
current-state documents for orientation and the numbered step audits as an
immutable development trail.

## Start here

- [Formal theory of operation](../THEORY_OF_OPERATION.md) — master engineering
  description synthesized from the sixteen detailed theory chapters; also
  available as a stable [PDF publication](../THEORY_OF_OPERATION.pdf).
- [Current project status](CURRENT_PROJECT_STATUS.md) — what is complete, what
  remains bounded, and what can legitimately start new work.
- [Simulator, build, and testing guide](SIMULATOR_BUILD_TEST_GUIDE.md) — how to
  build, run, verify, use the raw HAL, and interpret the results.
- [Embedded and HIL implementation roadmap](EMBEDDED_HIL_IMPLEMENTATION_ROADMAP.md)
  — phased target selection, porting, seam verification and hardware validation.
- [Step history index](STEP_HISTORY_INDEX.md) — navigable Steps 104-179 record.
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
- [Formal theory of operation](../THEORY_OF_OPERATION.md)
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

## Original-source preservation

[Preservation manifest](ORIGINAL_EVIDENCE_MANIFEST.md) and
[Step-178 consistency audit](STEP178_ORIGINAL_EVIDENCE_PRESERVATION_AUDIT.md)
identify archived originals and the recovered 16055375 workbook discrepancy.

## Measured NetRes replacement candidates

[Measured-matrix reconstruction report](NETRES_MEASURED_MATRIX_RECONSTRUCTION.md)
contains fitted circuits, standard resistor values and pair-error comparisons.

## Step 180 — NetRes circuit roles

[ECM schematic interpretation](NETRES_ECM_CIRCUIT_INTERPRETATION.md) identifies
VCC/ground references, coupled bias circuits, and the TPS-to-U11-MAP routing.
[Step-180 audit](STEP180_NETRES_ECM_INTERPRETATION_AUDIT.md) records scope and
verification. Candidate values and simulator behavior are unchanged.

## Step 181 — combined electrical/firmware theory

Theory of Operation revision 1.1 adds a six-sheet functional-block index and
expanded MEMCAL/U11 and DIAG/ALDL treatments, including the owner's reported
2023 DIAG-input solder repair. The PDF is regenerated. This begins systematic
electrical expansion; the index explicitly records unfinished blocks and custom-
chip boundaries. See [Step-181 audit](STEP181_SCHEMATIC_BLOCK_THEORY_AUDIT.md).

## Step 182 — power/reset/LIMP and injector electrical blocks

Theory of Operation revision 1.2 expands supply/retention/reset, the external
LIMP source and destinations, watchdog service boundaries and Q1 injector
drive/sense/feedback. U12 assertion rules and the unconnected INJLIMP pin 48
remain explicit gaps. PDF regenerated; simulator behavior and resistor
candidates unchanged. See [audit](STEP182_POWER_LIMP_INJECTOR_THEORY_AUDIT.md).
