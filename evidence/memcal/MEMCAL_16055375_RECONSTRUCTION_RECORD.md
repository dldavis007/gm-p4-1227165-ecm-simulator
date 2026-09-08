# MEMCAL 16055375 Resistor-Network Reconstruction Record

## Purpose

This record consolidates the surviving evidence for the reverse-engineered 16055375 MEMCAL resistor network. It is an evidence/reconstruction record, not a claim that every internal element of the original packaged network has been reproduced component-for-component.

## Evidence sources

The reconstruction is supported by multiple independent artifacts retained by the project owner:

1. The physical reverse-engineered MEMCAL and close-up photograph of the hand-built resistor/jumper board.
2. `NetRes 16055375.asc`, an LTspice model of the original/equivalent network.
3. `NetRes_16055375.kicad_pcb`, the KiCad PCB used for the physical reconstruction.
4. `NetRes 16055375 and 16055376.ods`, containing terminal-resistance comparison/measurement information.
5. Direct owner inspection establishing the 66-pin carrier orientation and carrier-to-J4 interleave.

For the **as-built hand reconstruction**, the completed MEMCAL photograph and direct owner inspection are authoritative wherever a component marking, jumper, open position, or orientation is legible. KiCad, LTspice, and historical records are supporting evidence and must not override a clearly visible installed part. See `docs/reference/MEMCAL_EVIDENCE_HIERARCHY.md`.

## Original-network model

The LTspice schematic models the 16055375 as a 14-terminal resistor network. The source uses bare numeric resistor values rather than values carrying an explicit `k` suffix. The physical reconstruction, KiCad values, measured terminal-resistance work, and SMD markings establish that the intended hardware scale for the corresponding branches is in kilohms.

| SPICE ref | LTspice model number | Reconstructed physical scale |
|---|---:|---:|
| R1 | 13 | 13 kOhm |
| R2 | 39 | 39 kOhm |
| R3 | 18 | 18 kOhm |
| R4 | 36 | 36 kOhm |
| R5 | 47 | 47 kOhm |
| R6 | 75 | 75 kOhm |
| R7 | 330 | 330 kOhm |
| R8 | 15 | 15 kOhm |
| R9 | 510 | 510 kOhm |
| R10 | 270 | 270 kOhm |
| R11 | 620 | 620 kOhm |
| R12 | 10 | 10 kOhm |
| R13 | 91 | 91 kOhm |
| R14 | 91 | 91 kOhm |
| R15 | 100 | 100 kOhm |
| R16 | 75 | 75 kOhm |
| R17 | 470 | 470 kOhm |
| R18 | 220 | 220 kOhm |

The SPICE source explicitly labels terminals 1 through 14 and shows terminals 7 and 8 on a common node.

## Generic PCB implementation

The KiCad reconstruction uses a generic grid-style PCB rather than a purpose-routed reproduction of the original molded resistor network. It provides sixteen physical connector positions, J1 through J16. Populated zero-ohm resistors are routing jumpers, not calibration values.

## 14-terminal network on the 16-position reconstruction board

| Original network terminal | Generic KiCad position |
|---:|---:|
| 1 | J1 |
| 2 | J2 |
| 3 | J3 |
| 4 | J4 |
| 5 | J5 |
| 6 | J6 |
| 7 | J7 |
| -- | J8 unused package position |
| -- | J9 unused package position |
| 8 | J10 |
| 9 | J11 |
| 10 | J12 |
| 11 | J13 |
| 12 | J14 |
| 13 | J15 |
| 14 | J16 |

## Absolute MEMCAL carrier/J4 mapping

Direct physical inspection has resolved the previous 66-contact numbering uncertainty. Top-side carrier pins are numbered IC-style: 1-33 down the left side and 34-66 up the right side. The bottom header is interleaved such that carrier 1 -> J4 1, carrier 33 -> J4 65, carrier 34 -> J4 66, and carrier 66 -> J4 2.

For the 16055375 package this gives:

| Network terminal | Absolute carrier pin | J4/CAL contact | Known motherboard destination |
|---:|---:|---:|---|
| 1 | 17 | CAL33 | U11 pin 22 |
| 2 | 18 | CAL35 | U11 pin 21 |
| 3 | 19 | CAL37 | U11 pin 9 |
| 4 | 20 | CAL39 | unresolved |
| 5 | 21 | CAL41 | U11 pin 1 |
| 6 | 22 | CAL43 | U11 pin 2 |
| 7 | 23 | CAL45 | U11 pin 8 |
| 8 | 44 | CAL46 | unresolved |
| 9 | 45 | CAL44 | U11 pin 13 |
| 10 | 46 | CAL42 | U11 pin 18, OSC |
| 11 | 47 | CAL40 | U11 pin 10 |
| 12 | 48 | CAL38 | U11 pin 11 |
| 13 | 49 | CAL36 | U11 pin 12 |
| 14 | 50 | CAL34 | unresolved |

Thus CAL33-CAL46 spans the entire 14-terminal package exactly.

## Collapsed reconstructed topology

A complete zero-ohm/net audit of `NetRes_16055375.kicad_pcb` now establishes the terminal-level topology of the generic-grid reconstruction.

The zero-ohm routing collapses internal nets so that:

- original terminal 7 (J7) and original terminal 8 (J10) are directly common;
- J8 and J9 remain unused package positions;
- a large internal grid region collapses directly to J16, original terminal 14.

After that collapse, the reconstruction reduces to these resistor branches:

| Original terminals | CAL nodes | Reconstructed branch |
|---|---|---:|
| 1-2 | CAL33-CAL35 | 13 kOhm |
| 2-3 | CAL35-CAL37 | 39 kOhm |
| 4-5 | CAL39-CAL41 | 36 kOhm |
| 5-6 | CAL41-CAL43 | 47 kOhm |
| 1-14 | CAL33-CAL34 | 75 kOhm |
| 2-14 | CAL35-CAL34 | 330 kOhm |
| 3-14 | CAL37-CAL34 | 15 kOhm |
| 4-14 | CAL39-CAL34 | 510 kOhm |
| 5-14 | CAL41-CAL34 | 270 kOhm |
| 6-14 | CAL43-CAL34 | 470 kOhm |
| 13-14 | CAL36-CAL34 | 18 kOhm |
| 12-14 | CAL38-CAL34 | 10 kOhm |
| 11-14 | CAL40-CAL34 | 91 kOhm |
| 10-14 | CAL42-CAL34 | 91 kOhm |
| 9-14 | CAL44-CAL34 | 100 kOhm |
| 9-(7/8) | CAL44-(CAL45/CAL46) | 75 kOhm |

CAL45 and CAL46 are therefore electrically common in the reconstructed network.

See `MEMCAL_16055375_TOPOLOGY_AUDIT.md` for the trace basis and comparison.

## Agreement and discrepancy versus LTspice

Most of the collapsed reconstruction agrees directly with LTspice: the 13, 39, 36, 47, 75, 330, 15, 510, 270, 18, 10, 91, and 91 kOhm branch relationships above align with the model, and the terminal-7/terminal-8 common node is reproduced.

However, the **terminal-6 / terminal-9 / terminal-7/8 / terminal-14 region is not resistor-for-resistor topologically identical**.

The generic-grid reconstruction simplifies that area to:

- terminal 6 -> terminal 14: 470 kOhm;
- terminal 9 -> terminal 14: 100 kOhm;
- terminal 9 -> terminal 7/8 common: 75 kOhm.

The LTspice model contains additional internal-node structure in the same region, including explicit 620 kOhm and 220 kOhm elements in addition to 470 kOhm, 100 kOhm, and 75 kOhm.

This is now an explicit audit discrepancy. It must not be hidden by describing the hand reconstruction as topologically identical to LTspice.

Possible explanations include an intentional equivalent-network transformation, a simplification derived from terminal-resistance measurements, a missed or incorrect jumper/component in the physical reconstruction, an error in one of the historical artifacts, or a genuinely non-equivalent reconstruction.

## Functional interpretation

The corrected mapping makes the functional structure clearer without changing the preserved reconstruction evidence. Terminal 10 is CAL42, which reaches U11 pin 18 identified as `OSC`; the reconstructed 91 kOhm CAL42-to-CAL34 branch remains a useful theory-of-operation anchor.

Likewise, the reconstructed common node at terminals 7 and 8 is CAL45/CAL46. That relationship should be considered when interpreting motherboard traces and any separate semantic labels for those contacts.

## Photograph authority

The completed MEMCAL photograph is the controlling record of the **as-built** resistor/jumper population wherever the relevant marking or placement is legible. The photo confirms the generic-grid construction and numerous zero-ohm routing links. KiCad is used to establish hidden copper/net connectivity where the photograph cannot itself show the complete electrical route.

If a clearly legible photographed component conflicts with KiCad, LTspice, or a historical note, the photographed installed part is the as-built value/population and the conflicting design/model value remains preserved as an audit discrepancy.

## Interpretation boundary

High-confidence conclusions:

- 16055375 is a 14-terminal electrical network.
- The hand-built replacement uses a generic 16-position grid.
- Generic J8 and J9 are unused package positions.
- Zero-ohm parts configure/reroute the grid and are not calibration values.
- The absolute package-terminal-to-carrier-to-J4 mapping is established.
- The collapsed reconstruction topology is now documented.
- Original terminals 7 and 8 are common in both the reconstruction and LTspice.
- Most reconstructed branch relationships directly agree with LTspice.

Still unresolved:

- whether the simplified terminal-6 / terminal-9 section is electrically equivalent at the external terminals to the LTspice internal-node network;
- whether the difference arose intentionally or through a reconstruction/model error;
- unresolved motherboard destinations and U11/U12 internal behavior.

The best next verification is a **terminal-resistance comparison focused on terminals 6, 7/8, 9, and 14**, using the saved measurement workbook and/or targeted resistance measurements of the physical reconstruction. That will determine whether the simplified topology is an intentional electrical equivalent or an actual reverse-engineering error.
