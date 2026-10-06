# Measured-matrix fitted NetRes candidates

Read the [engineering report](../../../docs/NETRES_MEASURED_MATRIX_RECONSTRUCTION.md)
before using these candidate circuits. They use original package-pin numbering.
The source workbook is unchanged; the owner confirmed its original-device
measurement provenance on 2026-10-06.

- `16055375_15R`: preferred accuracy/count balance.
- `16055375_14R`: smaller option, with its own values.
- `16055375_16R`: additional branch, retained for comparison.
- `16055376_9R`: nine nonzero resistors plus the two required direct links.

Each candidate includes `_resistors.csv`, `_comparison.csv`, and a passive
`.cir` SPICE subcircuit. Selected candidates also have SVG and PNG schematics.
The comparisons preserve original pair readings; computed values are predictions.
The 16055376 dash entries are treated as open; the blank 12–13 entry is not
invented as a measurement. Pin shorts are implemented by zero-volt sources in
SPICE, counted separately from nonzero resistors. No pin is forced to global
ground. A harness must supply its own reference/stimulus.

`ACCURACY_COMPONENT_COUNT.csv` compares error and count.
`practical_models.json` stores selected continuous and E96 branches.
`fit_results.json` stores pruning history and continuous fitting results.
These are DC model candidates, not measured new hardware or original topology.
