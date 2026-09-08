# MEMCAL 16055376 Resistor-Network Reconstruction Record

## Purpose

This record consolidates the surviving evidence for the reverse-engineered
16055376 MEMCAL resistor network and its generic-PCB implementation. It
preserves what is directly supported by the LTspice, KiCad, measurement, and
physical evidence without overstating one-for-one internal component
correspondence.

## Evidence sources

The principal source artifacts are:

1. The physical reverse-engineered MEMCAL and user-supplied close-up photograph
   of the completed generic resistor/jumper board.
2. Direct owner inspection establishing the 66-pin carrier orientation and
   carrier-to-J4 interleave.
3. `NetRes 16055376.asc` — LTspice schematic/model.
4. `NetRes_16055376.kicad_pcb` — KiCad PCB reconstruction.
5. `NetRes 16055375 and 16055376.ods` — historical comparison/measurement
   workbook.

For the **as-built hand reconstruction**, the completed MEMCAL photograph and
direct owner inspection are authoritative wherever a component marking, jumper,
open position, or orientation is legible. KiCad, LTspice, and historical records
are supporting evidence and must not override a clearly visible installed part.
See `docs/reference/MEMCAL_EVIDENCE_HIERARCHY.md`.

## Original-network terminal count

The LTspice source explicitly labels terminals **1 through 16**. The KiCad
reconstruction likewise provides connector positions J1 through J16, so the
supported local correspondence is direct: terminal 1 -> J1 through terminal 16
-> J16.

## Absolute MEMCAL carrier/J4 mapping

Direct physical inspection has resolved the former absolute mapping question.
Top-side carrier pins are numbered IC-style, 1-33 down the left side and 34-66
up the right side. The bottom header interleaves the two sides by row.

For the 16055376 package:

| Network terminal | Absolute carrier pin | Absolute J4 contact | Known motherboard destination |
|---:|---:|---:|---|
| 1 | 25 | 49 | historical CAL sequence likely CAL49; destination unresolved |
| 2 | 26 | 51 | U11 pin 24 |
| 3 | 27 | 53 | unresolved |
| 4 | 28 | 55 | U11 pin 4 |
| 5 | 29 | 57 | U11 pin 3 |
| 6 | 30 | 59 | 100 Ohm to VIGN |
| 7 | 31 | 61 | U11 pin 28, MAP |
| 8 | 32 | 63 | unresolved |
| 9 | 35 | 64 | unresolved |
| 10 | 36 | 62 | unresolved |
| 11 | 37 | 60 | U11 pin 7 |
| 12 | 38 | 58 | unresolved |
| 13 | 39 | 56 | U12 pin 11, CYL |
| 14 | 40 | 54 | unresolved |
| 15 | 41 | 52 | capacitor / U11 pin 16 |
| 16 | 42 | 50 | U11 pin 17 |

The historical CAL-connection notes explicitly run through CAL61. Therefore
absolute J4 contacts 62-64 above must not be casually relabeled as CAL62-CAL64
without schematic confirmation. The duplicated `CAL40` entry after CAL48 in the
notes is treated as a likely transcription error for CAL49, but that correction
remains inferential.

## LTspice resistor set

The 16055376 LTspice schematic contains nine resistor elements:

| SPICE ref | Value |
|---|---:|
| R1 | 150 kOhm |
| R2 | 1.5 kOhm |
| R3 | 130 kOhm |
| R4 | 24 kOhm |
| R5 | 10 kOhm |
| R6 | 8.2 kOhm |
| R7 | 5.1 kOhm |
| R8 | 7.5 kOhm |
| R9 | 10 kOhm |

The schematic also contains terminal-resistance annotations and simulation
sources used to characterize network behavior.

## KiCad generic-grid implementation

The KiCad PCB is a 16-position generic grid. It uses populated resistors,
zero-ohm links, and open positions to realize the required connectivity on a
reusable board pattern.

Notable populated values recorded by KiCad include:

| KiCad ref | Value | Local role | Absolute relationship |
|---|---:|---|---|
| R1 | 150 kOhm | J1-J2 | J4 49-J4 51 |
| R2 | 1.45 kOhm | J2-J3 | J4 51-J4 53 |
| R3 | 130 kOhm | J3-J4 | J4 53-J4 55 |
| R4 | 0 Ohm | J4-J5 routing link | J4 55-J4 57 |
| R5 | 25.5 Ohm | J5-J6 | J4 57-J4 59; KiCad record conflicts with as-built photo |
| R12 | 10 kOhm | J12-J11 | J4 58-J4 60 |
| R15 | 0 Ohm | routing link from J1 | J4 49-side routing |
| R17 | 0 Ohm | routing link from J3 | J4 53-side routing |
| R21 | 0 Ohm | routing link from J7 | J4 61-side routing |
| R23 | 10 kOhm | internal-grid branch to J16 | branch reaches J4 50 |
| R24 | 8.2 kOhm | internal-grid branch to J15 | branch reaches J4 52 |
| R25 | 5 kOhm | internal-grid branch to J14 | branch reaches J4 54 |
| R26 | 7.4 kOhm | internal-grid branch to J13 | branch reaches J4 56 / CYL |
| R30 | 0 Ohm | grid routing link to J9 | J4 64-side routing |
| R32 | 0 Ohm | internal routing link | internal |
| R33 | 0 Ohm | internal routing link | internal |
| R37 | 0 Ohm | internal routing link | internal |

Numerous other grid positions are explicitly marked `Open`.

## Photograph authority and resolved J5-J6 value

The completed MEMCAL photograph is the controlling record of the **as-built**
16055376 population wherever the relevant marking or placement is legible. The
KiCad table above records the design/reconstruction file, not an authority that
can override the finished physical board.

The formerly open J5-J6 discrepancy is now resolved from the close-up
photograph. The installed component at that adjacent-chain position is clearly
marked **`2402`**, the standard four-digit SMD code for **24,000 Ohm = 24 kOhm**.
The surrounding photographed sequence is also consistent with the expected
150 kOhm / 1.5 kOhm / 130 kOhm / zero-ohm-routing / 24 kOhm chain.

Therefore:

- **As-built J5-J6 value: 24 kOhm — authoritative from photograph.**
- LTspice nominal value: 24 kOhm — agrees with as-built photograph.
- KiCad entry: 25.5 Ohm — retained as a reconstruction/design-record error and
  must not be used as the physical value.

Other small KiCad-vs-LTspice differences, including 1.45 kOhm versus 1.5 kOhm,
5.0 kOhm versus 5.1 kOhm, and 7.4 kOhm versus 7.5 kOhm, remain comparison points
until their installed markings are read from the photograph or confirmed by
direct inspection.

If the photograph is unclear at a particular component location, leave the
value unresolved until direct inspection or targeted measurement resolves it.

## Zero-ohm and open positions

- `0 Ohm` parts are routing/configuration jumpers.
- `Open` parts are intentionally unpopulated grid positions.
- Neither is a calibration resistor value.

This distinction matters because the generic board contains many more possible
component sites than the original/equivalent network contains active resistor
elements.

## Functional interpretation

The corrected absolute mapping gives several high-value theory-of-operation
anchors:

- terminal 13 -> J4 56 -> U12 pin 11, `CYL`;
- terminal 7 -> J4 61 -> U11 pin 28, `MAP`;
- terminal 15 -> J4 52 -> capacitor/U11 pin 16;
- terminal 6 -> J4 59 -> 100 Ohm to VIGN;
- terminal 16 -> J4 50 -> U11 pin 17.

These destinations can now be used to audit the reconstructed network from the
functional direction. They do **not** by themselves establish the custom
U11/U12 internal transfer functions.

The generic-grid drawing should therefore be reduced electrically by collapsing
zero-ohm-connected regions, retaining only nonzero resistor branches, and then
labeling external nodes with their absolute J4 and motherboard destinations.

## Confidence statement

Current confidence is high for the following conclusions:

- 16055376 is a 16-terminal network.
- The generic KiCad reconstruction uses local positions J1-J16 as the terminal
  set.
- The absolute package-terminal-to-carrier-to-J4 mapping is now established.
- The completed MEMCAL photograph is authoritative for the as-built physical
  population wherever legible.
- J5-J6 is physically populated with a 24 kOhm resistor marked `2402`.
- The LTspice 24 kOhm value agrees with the as-built photograph at J5-J6.
- The KiCad 25.5-Ohm entry is not the as-built value and is retained only as a
  design/reconstruction-record discrepancy.
- KiCad and LTspice remain complementary evidence for design/topology, but they
  do not override a clearly visible installed component.

## Remaining verification

The prior blanket requirement to continuity-test all sixteen terminals merely
to discover their absolute connector numbering is superseded by direct physical
inspection of the carrier numbering.

Targeted measurements remain useful for blank destinations, unresolved J4
62-64 identities, internal/common nodes, and any other reconstructed resistor or
jumper relationship that conflicts with the emerging theory of operation.

See `docs/reference/MEMCAL_FUNCTIONAL_NETWORKS.md` for the simplified
functional view used in Step 129 and `docs/reference/MEMCAL_EVIDENCE_HIERARCHY.md`
for the as-built evidence-priority rule.
