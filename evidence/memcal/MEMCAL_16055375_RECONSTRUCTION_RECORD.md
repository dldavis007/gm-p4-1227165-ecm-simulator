# MEMCAL 16055375 Resistor-Network Reconstruction Record

## Purpose

This record consolidates the surviving evidence for the reverse-engineered 16055375 MEMCAL resistor network. It is an evidence/reconstruction record, not a claim that every internal element of the original packaged network has been reproduced component-for-component.

## Evidence sources

The reconstruction is supported by multiple independent artifacts retained by the project owner:

1. The physical reverse-engineered MEMCAL and close-up photograph of the hand-built resistor/jumper board.
2. `NetRes 16055375.asc`, an LTspice model of the original network and its terminal behavior.
3. `NetRes_16055375.kicad_pcb`, the KiCad PCB used for the physical reconstruction.
4. `NetRes 16055375 and 16055376.ods`, containing terminal-resistance comparison/measurement information.

The original Drive copies remain historical source evidence. The user-supplied close-up photograph is primary physical evidence of the implemented reconstruction.

## Original-network model

The LTspice schematic models the 16055375 as a 14-terminal resistor network. The source uses bare numeric resistor values rather than values carrying an explicit `k` suffix. The physical reconstruction, KiCad values, measured terminal-resistance work, and SMD markings establish that the intended hardware scale for the corresponding branches is in kilohms. To keep the distinction explicit, the table below records the LTspice model number and the corresponding reconstructed physical scale separately.

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

The SPICE source explicitly labels terminals 1 through 14. It also shows terminals 7 and 8 on a common node.

## Generic PCB implementation

The KiCad reconstruction uses a generic grid-style PCB rather than a purpose-routed reproduction of the original molded resistor network. It provides sixteen physical connector positions, J1 through J16.

The populated zero-ohm resistors are routing jumpers. They are not calibration resistor values. Their function is to connect the generic grid to the connector positions required for this particular network implementation.

This interpretation is confirmed by the project owner's description of the board construction and is consistent with the visible zero-ohm parts in the physical close-up.

## 14-terminal network on the 16-position carrier

The evidence supports the following physical-position correspondence for the 16055375 reconstruction:

| Original network terminal | KiCad carrier position |
|---:|---:|
| 1 | J1 |
| 2 | J2 |
| 3 | J3 |
| 4 | J4 |
| 5 | J5 |
| 6 | J6 |
| 7 | J7 |
| -- | J8 unused for 14-pin package position |
| -- | J9 unused for 14-pin package position |
| 8 | J10 |
| 9 | J11 |
| 10 | J12 |
| 11 | J13 |
| 12 | J14 |
| 13 | J15 |
| 14 | J16 |

Thus the apparent 16-position KiCad carrier is not evidence that the original 16055375 package had sixteen terminals. It is the generic physical carrier used to implement the 14-terminal network, with two center positions unused for the package correspondence.

A useful cross-check is the common-node relationship: SPICE terminals 7 and 8 are electrically common. Under the mapping above these correspond to J7 and J10. The PCB jumper configuration reproduces that relationship.

## Directly comparable resistor relationships

The following direct branches in the LTspice model provide useful cross-checks against the reconstructed PCB/netlist:

| Original terminals | Carrier positions | Reconstructed physical scale |
|---|---|---:|
| 1-2 | J1-J2 | 13 kOhm |
| 2-3 | J2-J3 | 39 kOhm |
| 4-5 | J4-J5 | 36 kOhm |
| 5-6 | J5-J6 | 47 kOhm |
| 1-14 | J1-J16 | 75 kOhm |
| 2-14 | J2-J16 | 330 kOhm |
| 3-14 | J3-J16 | 15 kOhm |
| 4-14 | J4-J16 | 510 kOhm |
| 5-14 | J5-J16 | 270 kOhm |
| 13-14 | J15-J16 | 18 kOhm |
| 12-14 | J14-J16 | 10 kOhm |
| 11-14 | J13-J16 | 91 kOhm |
| 10-14 | J12-J16 | 91 kOhm |
| 8-9 | J10-J11 | 75 kOhm |

The J11-J16 branch in the KiCad reconstruction is populated as 100 kOhm, but that is **not** a direct terminal-9-to-terminal-14 resistor in the LTspice model. The SPICE model instead places its 100 and 220 elements through an internal node in that portion of the network. This is one of the reasons the KiCad reconstruction must not be described as a literal one-component-for-one-component transcription of the SPICE model.

The SPICE model also contains 620, 470 and 220 elements associated with the lower/common-node portion of the network. These must not be assumed to correspond one-for-one with a single physical resistor on the generic PCB without tracing the complete reconstructed netlist.

## Photograph corroboration

The close-up photograph shows the expected generic-grid construction and numerous zero-ohm routing links. Several visible nonzero SMD markings are consistent with values represented in the reconstruction/model, including markings corresponding to 13 kOhm, 75 kOhm, 330 kOhm and 10 kOhm.

The photograph is used here as corroborating physical evidence. Ambiguous or rotated markings must not be used to override the KiCad/SPICE source data without a position-specific visual review.

## Interpretation boundary

The strongest supported interpretation is:

- 16055375 is represented by a 14-terminal electrical network.
- The hand-built replacement uses a generic 16-position grid carrier.
- J8 and J9 are unused in the 14-terminal package-position correspondence.
- Zero-ohm SMD parts configure/reroute the generic grid and are not calibration values.
- The KiCad design, LTspice model, resistance-comparison work and physical photograph mutually corroborate the reconstruction.
- The reconstruction was intended to reproduce the relevant terminal connectivity/behavior; it should not be described as a proven component-for-component copy of the inaccessible internal construction of the original packaged resistor network.

## MEMCAL/J4 boundary

This record establishes the local 16055375 network/carrier relationship. It does **not** by itself establish the absolute 66-contact MEMCAL/J4 numbering for every network terminal. The separate MEMCAL carrier mapping must be used to connect these local network positions to absolute MEMCAL/J4 contacts and CAL signals.

Accordingly, the project's open exact package-pin-to-J4/CAL mapping question should remain open until that final correspondence is traced and independently checked.

## Required physical continuity verification

Before closing the exact MEMCAL/J4 mapping question, perform a physical continuity check on the actual reverse-engineered MEMCAL carrier. Using an ohmmeter/continuity tester, verify the correspondence between **every accessible pin on the EPROM/resistor-network side of the carrier and the 66-contact header side**.

This check should cover the EPROM pins, the 16055375 network positions, the companion resistor-network positions, and any other populated or intentionally connected carrier positions. Record both positive continuity and intentionally unconnected positions where practical.

The result should be captured as a pin-by-pin table containing at least:

| Carrier-side item | Local pin/position | 66-contact header pin | Continuity verified | Notes |
|---|---:|---:|---|---|
| EPROM | | | | |
| 16055375 network | | | | |
| companion network | | | | |

Where multiple carrier-side pins are electrically common, record each physical correspondence rather than inferring the second connection from symmetry.

This physical measurement is the preferred final verification because it will independently test the current extrapolation of the undocumented half of the 66-contact carrier and can convert the package-pin-to-J4/CAL mapping from inferred/document-derived evidence to directly measured hardware evidence.
