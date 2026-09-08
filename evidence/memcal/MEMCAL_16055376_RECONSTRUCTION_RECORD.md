# MEMCAL 16055376 Resistor-Network Reconstruction Record

## Purpose

This record consolidates the surviving evidence for the reverse-engineered 16055376 MEMCAL resistor network and its generic-grid implementation. The completed MEMCAL photograph is authoritative for the **as-built** hand reconstruction wherever a component marking, jumper, open position, or orientation is legible.

Supporting sources include the LTspice model, KiCad reconstruction, historical measurement workbook, and direct owner inspection of the 66-pin carrier. See `docs/reference/MEMCAL_EVIDENCE_HIERARCHY.md`, `MEMCAL_16055376_AS_BUILT_PHOTO_AUDIT.md`, and `MEMCAL_16055376_TOPOLOGY_AUDIT.md`.

## Terminal count and absolute mapping

16055376 is a 16-terminal network. Local terminal 1 maps to generic position J1 through terminal 16 to J16.

Direct physical inspection establishes IC-style carrier numbering: 1-33 down the left side and 34-66 up the right side. The bottom J4 header interleaves the two sides by physical row.

| Network terminal | Carrier pin | J4 contact | Known motherboard destination |
|---:|---:|---:|---|
| 1 | 25 | 49 | historical sequence likely CAL49; unresolved |
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

The retained CAL-connection notes explicitly run through CAL61. J4 contacts 62-64 therefore remain absolute connector numbers until schematic evidence assigns semantics. The duplicated `CAL40` after CAL48 in the historical notes is treated as a likely transcription error for CAL49, but that remains inferential.

## Authoritative as-built resistor population

The completed close-up photograph makes all nine nonzero resistor markings legible:

| Marking | As-built value |
|---|---:|
| `1503` | 150 kOhm |
| `1501` | 1.5 kOhm |
| `1303` | 130 kOhm |
| `2402` | 24 kOhm |
| `1002` | 10 kOhm |
| `8201` | 8.2 kOhm |
| `5101` | 5.1 kOhm |
| `7501` | 7.5 kOhm |
| `1002` | 10 kOhm |

This exactly matches the LTspice nominal resistor set.

The KiCad file contains several non-authoritative value differences, including 1.45 kOhm, 5.0 kOhm, 7.4 kOhm, and a 25.5-Ohm entry where the finished board carries 24 kOhm (`2402`). Those entries are retained as design/reconstruction-record discrepancies and do not define the physical build.

## Zero-ohm topology closure

The KiCad connectivity provides a precise trace of the generic-grid routing. Its populated zero-ohm links are:

- R4: J4-J5
- R15: J1-internal node N17
- R17: J3-internal node N19
- R21: J7-internal node N23
- R30: internal node N24-J9
- R32: N18-N19
- R33: N19-N20
- R37: N23-N24

Collapsing these routing links gives:

- **J4 = J5**
- **N17 = J1**
- **N18 = N19 = N20 = J3**
- **N23 = N24 = J7 = J9**
- **J8 isolated/open**
- **J10 isolated/open**

After inserting the photograph-authoritative resistor values, the complete simplified network is:

| Terminal relationship | As-built relationship |
|---|---:|
| J1-J2 | 150 kOhm |
| J2-J3 | 1.5 kOhm |
| J3-J4/J5 common node | 130 kOhm |
| J4/J5 common node-J6 | 24 kOhm |
| J1-J16 | 10 kOhm |
| J3-J15 | 8.2 kOhm |
| J3-J14 | 5.1 kOhm |
| J3-J13 | 7.5 kOhm |
| J12-J11 | 10 kOhm |
| J7-J9 | direct/common node |
| J8 | isolated/open |
| J10 | isolated/open |

The LTspice model independently has the same terminal topology and the same nine resistor values. Therefore the 16055376 generic-grid reconstruction and LTspice equivalent network agree exactly after zero-ohm collapse and substitution of the photograph-authoritative values.

## Absolute J4/CAL form

The simplified network becomes:

| 16055376 terminals | Absolute J4/CAL nodes | As-built relationship |
|---|---|---:|
| 1-2 | J4 49-CAL51 | 150 kOhm |
| 2-3 | CAL51-CAL53 | 1.5 kOhm |
| 3-4/5 | CAL53-(CAL55=CAL57) | 130 kOhm |
| 4/5-6 | (CAL55=CAL57)-CAL59 | 24 kOhm |
| 1-16 | J4 49-CAL50 | 10 kOhm |
| 3-15 | CAL53-CAL52 | 8.2 kOhm |
| 3-14 | CAL53-CAL54 | 5.1 kOhm |
| 3-13 | CAL53-CAL56 | 7.5 kOhm |
| 12-11 | CAL58-CAL60 | 10 kOhm |
| 7-9 | CAL61-J4 64 | direct/common node |
| 8 | J4 63 | isolated/open within this network |
| 10 | J4 62 | isolated/open within this network |

Useful consequences include:

- CAL56 / U12 pin 11 (`CYL`) is tied to CAL53 through 7.5 kOhm.
- CAL52 / capacitor-U11 pin 16 is tied to CAL53 through 8.2 kOhm.
- CAL59 / VIGN-related path is tied to the CAL55/CAL57 common node through 24 kOhm.
- CAL61 / U11 pin 28 (`MAP`) is directly common with J4 64 inside the MEMCAL network.
- CAL55 and CAL57 are directly common inside the MEMCAL network.

These are electrical relationships only; they do not establish the internal transfer functions of U11 or U12.

## Closure status

High-confidence closure now includes:

- 16-terminal package mapping;
- complete nine-resistor as-built value set from the completed photograph;
- zero-ohm grid collapse;
- terminal-level topology;
- exact topology and value agreement with LTspice.

The reconstruction itself is no longer the main uncertainty for 16055376. Remaining work is interpretation of motherboard destinations and custom-device behavior, especially J4 62-64, CAL53/CAL54, CYL selection, and the MAP-related path.
