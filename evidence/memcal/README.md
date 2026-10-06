# MEMCAL evidence

This directory contains source evidence and reconstruction records for the BUA /
GM 1227165 MEMCAL (memory/calibration) work.

## As-built prototype photographs

The reconstructed MEMCAL prototype photographs are committed under
`evidence/memcal/photos/` and are the authoritative as-built evidence whenever
component markings, jumper population, opens, orientation, or physical routing
are legible in the photographs.

- [`photos/reverse_engineered_memcal_overview.jpg`](photos/reverse_engineered_memcal_overview.jpg)
  — full-resolution, broadly viewable overall reconstructed MEMCAL prototype
  view.
- [`photos/reverse_engineered_memcal_resistor_board_closeup.jpg`](photos/reverse_engineered_memcal_resistor_board_closeup.jpg)
  — full-resolution, broadly viewable close-up used for the 16055375 / 16055376
  resistor-network and jumper audit.

The original `.heic` files remain beside the JPEGs as archival source files. The
JPEGs preserve the original 4032 x 3024 pixel dimensions, have metadata removed,
and are the preferred copies for GitHub and ordinary source-code editors.

Where the photographed as-built hardware conflicts with KiCad, LTspice,
historical notes, or derived models, the photograph controls the as-built
reconstruction and the discrepancy should be retained rather than silently
normalized.

## Supporting records

- `MEMCAL_16055375_RECONSTRUCTION_RECORD.md`
- `MEMCAL_16055375_TOPOLOGY_AUDIT.md`
- `MEMCAL_16055376_RECONSTRUCTION_RECORD.md`
- `MEMCAL_16055376_AS_BUILT_PHOTO_AUDIT.md`
- `MEMCAL_16055376_TOPOLOGY_AUDIT.md`
- `MemCal photo 86 Vette BUA.png` — original/reference MEMCAL photograph.
- `MemCal, Cal connections.docx`
- `Resistors.txt`

See `docs/reference/MEMCAL_EVIDENCE_HIERARCHY.md` for the project-wide evidence-priority rule and the numbered STEP audit documents for the chronological reconstruction history.

## Original models and resistance workbook

The `originals/` directory preserves both LTspice drawings, both KiCad
PCB/schematic/project sets and the original ODS resistance workbook.
See the [manifest](../../docs/ORIGINAL_EVIDENCE_MANIFEST.md) and
[Step-178 audit](../../docs/STEP178_ORIGINAL_EVIDENCE_PRESERVATION_AUDIT.md).
The recovered workbook raises unresolved cross-terminal discrepancies for the
16055375 candidate; full electrical equivalence is not established.
