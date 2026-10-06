# Step 178 — Original evidence preservation and consistency audit

## Result

Six missing firmware artifacts, nine MEMCAL model/design/workbook artifacts,
and one native research-note DOCX export are now preserved in the repository.
The existing authoritative listing is also independently verified, giving
seventeen manifest entries and sixteen newly archived source files.
See [manifest](ORIGINAL_EVIDENCE_MANIFEST.md) for paths, source URLs and hashes.
All seven firmware files match their previously recorded SHA-256 values.
No newly corrected firmware image was found; listing authority remains intact.

## Recovered terminal-pair evidence

The ODS has sheets named 16055375 and 16055376. The 16055375 sheet contains
all 91 upper-triangle terminal pairs; it records 7-8 as zero. It also records
6-14=158, 7/8-14=291, 9-14=220, 6-9=187, 5-9=202 and 4-9=231.
The scale is inferred as kOhm from the model/parts context, not an explicit
measurement certificate. Dates, instrument, isolation conditions, uncertainty
and whether each value was measured or derived are not documented in the table.
Workbook provenance must remain qualified.

The documented simplified 16055375 candidate, with its retained-parts-list
220-kOhm terminal-9-to-14 branch, gives:

| Pair | Workbook, inferred kOhm | Candidate, kOhm |
| --- | ---: | ---: |
| 6-14 | 158 | 153.371 |
| 7/8-14 | 291 | 295.000 |
| 9-14 | 220 | 220.000 |
| 6-9 | 187 | 373.371 |
| 5-9 | 202 | 353.879 |
| 4-9 | 231 | 370.433 |

The earlier terminal-14 comparisons cannot establish full terminal equivalence.
The recovered table is substantially inconsistent with this candidate at several
cross-terminal pairs. This is a model/workbook discrepancy, not proof of a
hardware fault, not a new physical measurement, and not permission to substitute
KiCad or LTspice values for photo-authoritative as-built parts.

Run `python tools/audit_memcal_resistance.py` to regenerate all 91 comparisons
in `evidence/memcal/16055375_WORKBOOK_MODEL_COMPARISON.csv`.
The tool uses standard-library Gaussian elimination with a one-unit test current.
It compares the documented candidate topology, not a fresh extraction of every
copper trace or a newly photographed as-built board.

## Other consistency checks

The imported 16055376 LTspice drawing retains the nine nominal values and
terminal connectivity documented by its topology audit. Both LTspice drawings
use bare resistor numbers; the inferred kilohm hardware scale remains explicit.
KiCad retains conflicting design values: 16055376 R2=1.45k, R5=25.5,
R25=5k, R26=7.4k; 16055375 retains its 100k branch and some bare value strings.
They are preserved as design records, not silently corrected originals.
The 16055376 workbook includes approximately 25 for terminal 4/5-6, while
the photo audit establishes the installed 24k component. Historical resistance
entries do not override the photographed population.

The research-note export preserves multiple BUA identities and tentative
replacement-network leads already discussed by the existing evidence register.
It does not establish exact calibration identity from BUA alone.

## Corrections and next work

Current summaries now link preserved originals and reflect the completed
16055376 photo/topology audit. The 16055375 equivalence claim is qualified by
the recovered table; historical audits receive explicit later-evidence addenda.
Next physical evidence should establish the workbook's setup and measure the
isolated as-built 16055375 terminal matrix, prioritizing 6-9, 5-9, 4-9 and
their paths to terminals 7/8 and 14. This does not block the behavioral port.
Target definition and HIL safety architecture remain unimplemented roadmap work.

## Validation

Raw downloads match their Drive metadata sizes. Firmware preservation hashes
match the existing seven-file provenance record; S-record/listing/binary and
historical image comparisons were independently rechecked. All local Markdown
file links resolve. Strict C89 `make clean test` passes without warnings and
with frozen signatures unchanged. No C source, calibration or behavioral baseline
was changed.

## Step-179 owner-confirmation addendum

The owner subsequently confirmed the workbook as original-device measurements
and the target for replacements, explaining that the prior reconstruction was
an imperfect approximation. Thus source-device provenance is now confirmed by
the owner; detailed instrument/setup conditions remain unspecified. Step 179
renames `workbook_inferred_kohm` to `original_measured_kohm` in the comparison
and its generating tool without changing the original workbook or its values.
See [measured-matrix reconstruction](NETRES_MEASURED_MATRIX_RECONSTRUCTION.md).
