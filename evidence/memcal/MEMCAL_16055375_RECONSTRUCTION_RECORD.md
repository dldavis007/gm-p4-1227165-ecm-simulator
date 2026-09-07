# MEMCAL 16055375 Resistor-Network Reconstruction Record

## Purpose

This record consolidates the surviving evidence for the reverse-engineered
16055375 MEMCAL resistor network. It is an evidence/reconstruction record, not
a claim that every internal element of the original packaged network has been
reproduced component-for-component.

## Evidence sources

The reconstruction is supported by multiple independent artifacts retained by
the project owner:

1. The physical reverse-engineered MEMCAL and close-up photograph of the
   hand-built resistor/jumper board.
2. `NetRes 16055375.asc`, an LTspice model of the original/equivalent network.
3. `NetRes_16055375.kicad_pcb`, the KiCad PCB used for the physical
   reconstruction.
4. `NetRes 16055375 and 16055376.ods`, containing terminal-resistance
   comparison/measurement information.
5. Direct owner inspection establishing the 66-pin carrier orientation and
   carrier-to-J4 interleave.

The original Drive copies remain historical source evidence. The close-up
photograph is primary physical evidence of the implemented reconstruction.

## Original-network model

The LTspice schematic models the 16055375 as a 14-terminal resistor network.
The source uses bare numeric resistor values rather than values carrying an
explicit `k` suffix. The physical reconstruction, KiCad values, measured
terminal-resistance work, and SMD markings establish that the intended hardware
scale for the corresponding branches is in kilohms.

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

The SPICE source explicitly labels terminals 1 through 14 and shows terminals
7 and 8 on a common node.

## Generic PCB implementation

The KiCad reconstruction uses a generic grid-style PCB rather than a
purpose-routed reproduction of the original molded resistor network. It
provides sixteen physical connector positions, J1 through J16.

The populated zero-ohm resistors are routing jumpers. They are not calibration
resistor values. Their function is to connect the generic grid to the connector
positions required for this particular implementation.

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

The apparent 16-position board is therefore not evidence that the original
16055375 had sixteen terminals. A useful cross-check is the SPICE common-node
relationship: terminals 7 and 8 correspond to generic J7 and J10, and the
jumper configuration reproduces that relationship.

## Absolute MEMCAL carrier/J4 mapping

Direct physical inspection has resolved the previous 66-contact numbering
uncertainty. Top-side carrier pins are numbered IC-style: 1-33 down the left
side and 34-66 up the right side. The bottom header is interleaved such that
carrier 1 -> J4 1, carrier 33 -> J4 65, carrier 34 -> J4 66, and carrier 66 ->
J4 2.

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

## Directly comparable resistor relationships

The direct branches in the LTspice model can now be expressed both as package
terminals and as CAL nodes:

| Original terminals | CAL nodes | Reconstructed physical scale |
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
| 13-14 | CAL36-CAL34 | 18 kOhm |
| 12-14 | CAL38-CAL34 | 10 kOhm |
| 11-14 | CAL40-CAL34 | 91 kOhm |
| 10-14 | CAL42-CAL34 | 91 kOhm |
| 8-9 | CAL46-CAL44 | 75 kOhm |

The J11-J16 branch on the generic KiCad reconstruction is populated as
100 kOhm, but that is **not** a direct original-terminal-9-to-terminal-14
resistor in the LTspice model. The SPICE model instead places its 100 and
220 elements through an internal node in that portion of the network.

The SPICE model also contains 620, 470, and 220 elements associated with
internal/common-node structure. They must not be assigned one-for-one from the
appearance of the generic grid without tracing the complete reconstructed
netlist.

## Functional interpretation

The corrected mapping makes the functional structure clearer without changing
the preserved reconstruction evidence. For example, terminal 10 is CAL42,
which reaches U11 pin 18 identified as `OSC`, and the direct 91 kOhm branch from
terminal 10 to terminal 14 therefore becomes an evidence-supported
CAL42/OSC-to-CAL34 relationship. The exact oscillator function remains to be
established by theory-of-operation evidence.

Likewise, the SPICE common node at terminals 7 and 8 is now CAL45/CAL46. That
relationship should be considered when interpreting the motherboard traces and
any separate semantic labels for those contacts.

## Photograph corroboration

The close-up photograph shows the expected generic-grid construction and
numerous zero-ohm routing links. Several visible nonzero SMD markings are
consistent with values represented in the reconstruction/model, including
markings corresponding to 13 kOhm, 75 kOhm, 330 kOhm, and 10 kOhm.

Ambiguous or rotated markings must not override the KiCad/SPICE source data
without a position-specific visual review.

## Interpretation boundary

The strongest supported interpretation is:

- 16055375 is a 14-terminal electrical network.
- The hand-built replacement uses a generic 16-position grid.
- Generic J8 and J9 are unused package positions.
- Zero-ohm parts configure/reroute the grid and are not calibration values.
- The absolute package-terminal-to-carrier-to-J4 mapping is now established.
- KiCad, LTspice, resistance-comparison work, and the physical photograph
  mutually corroborate the reconstruction.
- The reconstruction should not be described as a proven
  component-for-component copy of the inaccessible molded network.

## Remaining verification

The former blanket requirement to continuity-test every accessible pin merely
to establish the opposite half of the 66-pin connector is superseded by the
physical numbering inspection above.

Targeted measurements remain appropriate where they resolve a real electrical
question: blank motherboard destinations, common nodes, internal carrier
routing, or any resistor/jumper relationship that conflicts with the emerging
theory of operation.

See `docs/reference/MEMCAL_FUNCTIONAL_NETWORKS.md` for the simplified
functional view used in Step 129.
