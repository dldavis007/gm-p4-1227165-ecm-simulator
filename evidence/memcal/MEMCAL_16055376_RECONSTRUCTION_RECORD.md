# MEMCAL 16055376 Resistor-Network Reconstruction Record

## Purpose

This record consolidates the surviving evidence for the reverse-engineered 16055376 MEMCAL resistor network and its generic-grid implementation. The completed MEMCAL photograph is authoritative for the **as-built** hand reconstruction wherever a component marking, jumper, open position, or orientation is legible.

Supporting sources include the LTspice model, KiCad reconstruction, historical measurement workbook, and direct owner inspection of the 66-pin carrier. See `docs/reference/MEMCAL_EVIDENCE_HIERARCHY.md` and `MEMCAL_16055376_AS_BUILT_PHOTO_AUDIT.md`.

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

The completed close-up photograph makes all nine nonzero resistor markings legible. The installed resistor set is:

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

This is the complete nine-resistor value set for the as-built reconstruction.

## Comparison with LTspice

The LTspice model contains the same nine nominal values:

150 kOhm, 1.5 kOhm, 130 kOhm, 24 kOhm, 10 kOhm, 8.2 kOhm, 5.1 kOhm, 7.5 kOhm, and 10 kOhm.

Therefore the **complete photographed as-built nonzero resistor value set agrees exactly with the LTspice nominal set**.

This is an important closure result: resistor-value uncertainty is no longer the main issue for 16055376. The remaining question is node/topology equivalence after the generic-grid routing is collapsed.

## KiCad reconstruction record

The KiCad file is retained as design/reconstruction evidence. It uses a configurable 16-position grid with populated resistors, zero-ohm links, and open positions.

Some KiCad values do not match the finished board. Examples include approximately 1.45 kOhm instead of 1.5 kOhm, 5.0 kOhm instead of 5.1 kOhm, 7.4 kOhm instead of 7.5 kOhm, and the previously recorded 25.5-Ohm adjacent-chain value.

Those values must not be used as the physical population. The photograph shows the completed board actually carries 1.5 kOhm, 5.1 kOhm, 7.5 kOhm, and 24 kOhm at the corresponding value positions. The formerly disputed component is clearly marked `2402`, establishing 24 kOhm and identifying the KiCad 25.5-Ohm record as a design/reconstruction-record error.

## Zero-ohm and open positions

The photograph visibly contains numerous components marked `0` and direct routed/bridged connections. These are configuration/routing links required by the generic grid; they are not calibration resistor values.

The completed photograph is authoritative for visible population. However, exact photo-site-to-KiCad-reference assignments should only be made when the net position is unambiguous. A visible zero-ohm link should not be given a reference designator merely by appearance.

## Functional simplification

The generic grid should not be used directly as the theory of operation. The correct simplification procedure is:

1. use the photograph to establish the as-built population;
2. collapse all zero-ohm-connected regions into electrical nodes;
3. retain the nine nonzero resistor branches between those nodes;
4. associate external nodes with local J1-J16 terminals;
5. convert them to the established J4/CAL mapping above;
6. compare the resulting terminal-level network with LTspice.

Because the as-built nonzero value set now matches LTspice exactly, LTspice is a strong candidate for the simplest equivalent topology. That topology is not considered fully proved until the zero-ohm routing and terminal nodes are traced and shown to agree.

## Functional anchors

Several network terminals already have useful motherboard destinations:

- terminal 13 -> J4 56 -> U12 pin 11, `CYL`;
- terminal 7 -> J4 61 -> U11 pin 28, `MAP`;
- terminal 15 -> J4 52 -> capacitor/U11 pin 16;
- terminal 6 -> J4 59 -> 100 Ohm to VIGN;
- terminal 16 -> J4 50 -> U11 pin 17.

These are theory-of-operation anchors, not proof of U11/U12 internal transfer functions.

## Current confidence

High confidence:

- 16055376 has 16 terminals.
- terminal-to-carrier-to-J4 mapping is established.
- all nine nonzero as-built resistor values are now directly confirmed from the completed photograph.
- all nine exactly match the LTspice nominal set.
- zero-ohm parts are routing/configuration links.

Still open:

- exact node-by-node collapse of the zero-ohm routing grid;
- semantic identity of J4 62-64;
- unresolved motherboard destinations;
- internal behavior of U11/U12.

The next MEMCAL task is therefore a **topology audit**, not another resistor-value audit.
