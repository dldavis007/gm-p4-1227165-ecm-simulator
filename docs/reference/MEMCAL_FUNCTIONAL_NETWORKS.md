# MEMCAL functional-network map

## Purpose

This document separates the electrical function of the two BUA MEMCAL resistor
networks from the generic grid boards used to reconstruct them. The grid/KiCad
layouts remain reconstruction records; this view collapses zero-ohm routing
links and expresses the networks in terms of package terminals, absolute
carrier pins, J4/CAL contacts, and known motherboard destinations.

For the **as-built hand reconstruction**, the completed MEMCAL photograph and
direct owner inspection are authoritative wherever a component marking,
jumper, open position, or orientation is legible. KiCad and LTspice are
supporting design/model evidence and do not override a clearly visible installed
part. See `MEMCAL_EVIDENCE_HIERARCHY.md`.

This is an evidence cross-reference, not a claim that the inaccessible molded
networks used the same internal physical component layout as the discrete
reconstruction.

## Evidence states used here

- **Observed/as-built**: visible on the completed MEMCAL photograph or confirmed
  by direct owner inspection.
- **Measured**: direct electrical measurement of the physical reconstruction.
- **Model/design evidence**: LTspice, KiCad, historical workbook, or notes.
- **Mechanically derived**: follows directly from the physically established
  carrier numbering and connector interleave.
- **Motherboard traced**: destination is present in the retained CAL connection
  notes/schematic work.
- **Unresolved**: no reliable value, destination, or consequence is yet
  established.

When a legible photograph conflicts with a model/design source, the photograph
defines the as-built value/population and the model value remains recorded as a
discrepancy to explain.

## Absolute 66-pin carrier/J4 relationship

Top/component-side carrier numbering is IC-style:

- left side, top to bottom: carrier pins 1-33;
- right side, bottom to top: carrier pins 34-66.

The bottom 66-contact header interleaves the two sides by physical row:

- carrier 1 -> J4 1;
- carrier 2 -> J4 3;
- ...;
- carrier 33 -> J4 65;
- carrier 34 -> J4 66;
- ...;
- carrier 66 -> J4 2.

Equations:

- carrier 1-33: `J4 = 2*carrier - 1`
- carrier 34-66: `J4 = 2*(67-carrier)`

This mechanical correspondence is treated as established from direct physical
inspection. It is separate from PCB trace routing between component pins.

## 16055375: 14-terminal network

### Terminal to CAL mapping

| Network terminal | Absolute carrier pin | J4/CAL contact | Motherboard destination |
| ---: | ---: | ---: | --- |
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

The mapping is mechanically significant: J4/CAL33-CAL46 covers the complete
14-pin network, with odd CAL contacts traversing terminals 1-7 and even CAL
contacts traversing terminals 14-8 in the opposite direction.

### Simplified direct branches

The direct branches preserved by the LTspice reconstruction become:

| Network terminals | CAL nodes | Nominal/model value |
| --- | --- | ---: |
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

These values describe the LTspice/equivalent reconstruction. Where a photographed
installed component is legible, its observed value is the as-built authority.

LTspice terminals 7 and 8 are a common node, so the reconstructed model ties
CAL45 and CAL46 to the same electrical node. This is a particularly useful
functional simplification because CAL45 is traced to U11 pin 8 while CAL46 has
no separate destination recorded in the connection notes.

The LTspice model also includes 620 kOhm, 100 kOhm, 470 kOhm, and 220 kOhm
elements through internal/common-node structure. Those elements must remain
represented by their actual SPICE topology rather than being assigned to a
single CAL-to-CAL branch from the generic PCB appearance.

### Functional observations

- CAL42 reaches U11 pin 18 identified as `OSC`. The model's 91 kOhm
  terminal-10-to-terminal-14 branch therefore gives an evidence-supported
  CAL42/OSC-to-CAL34 relationship. The exact oscillator transfer function
  remains unresolved.
- CAL34 appears repeatedly as the opposite node of many direct branches and is
  structurally important even though its motherboard destination remains
  unresolved.
- CAL45/CAL46 being electrically common in the model should be checked against
  motherboard traces and theory of operation before assigning separate
  semantics.

## 16055376: 16-terminal network

### Terminal to CAL mapping

| Network terminal | Absolute carrier pin | J4/CAL contact | Motherboard destination |
| ---: | ---: | ---: | --- |
| 1 | 25 | J4 49 | unresolved; source notes likely intended CAL49 |
| 2 | 26 | CAL51 | U11 pin 24 |
| 3 | 27 | CAL53 | unresolved |
| 4 | 28 | CAL55 | U11 pin 4 |
| 5 | 29 | CAL57 | U11 pin 3 |
| 6 | 30 | CAL59 | 100 Ohm to VIGN |
| 7 | 31 | CAL61 | U11 pin 28, MAP |
| 8 | 32 | J4 63 | unresolved |
| 9 | 35 | J4 64 | unresolved |
| 10 | 36 | J4 62 | unresolved |
| 11 | 37 | CAL60 | U11 pin 7 |
| 12 | 38 | CAL58 | unresolved |
| 13 | 39 | CAL56 | U12 pin 11, CYL |
| 14 | 40 | CAL54 | unresolved |
| 15 | 41 | CAL52 | capacitor / U11 pin 16 |
| 16 | 42 | CAL50 | U11 pin 17 |

The user connection-note series explicitly covers CAL29-CAL61. Therefore J4
62-64 remain absolute connector numbers, not invented CAL62-CAL64 semantics.

### Simplified reconstruction relationships

The nominal LTspice model contains nine resistor values:

150 kOhm, 1.5 kOhm, 130 kOhm, 24 kOhm, 10 kOhm, 8.2 kOhm, 5.1 kOhm,
7.5 kOhm, and 10 kOhm.

The generic KiCad board records routing links and substitute/measured values.
Known local relationships include:

| Local reconstruction relationship | J4/CAL relationship | KiCad/model record |
| --- | --- | --- |
| J1-J2 | J4 49-CAL51 | 150 kOhm |
| J2-J3 | CAL51-CAL53 | 1.45 kOhm; nominal model 1.5 kOhm |
| J3-J4 | CAL53-CAL55 | 130 kOhm |
| J4-J5 | CAL55-CAL57 | 0 Ohm routing link |
| J5-J6 | CAL57-CAL59 | KiCad 25.5 Ohm; nominal model contains 24 kOhm; as-built value must be read from photograph/direct inspection |
| J12-J11 | CAL58-CAL60 | 10 kOhm |

Additional internal-grid branches reach J16/CAL50, J15/CAL52,
J14/CAL54, and J13/CAL56. Their functional simplification must follow the
actual node topology before assigning a direct CAL-to-CAL branch.

### Functional observations

- CAL56 reaches U12 pin 11 identified as `CYL`; this is a prime anchor for
  decoding fixed cylinder selection once U12 behavior is established.
- CAL61 reaches U11 pin 28 identified as `MAP`, placing network terminal 7 at a
  known MAP-related hardware boundary.
- CAL52 reaches a capacitor/U11 pin 16 path and may participate in timing or
  filtering; exact function remains unresolved.
- CAL59 is tied through 100 Ohm to VIGN on the motherboard, providing an
  external reference for validating terminal behavior.

## Why the generic grid should not define the theory of operation

The hand-built boards intentionally use a reusable grid. Zero-ohm parts select
routing paths and many possible grid positions are left open. Consequently the
physical board can look much more complicated than the equivalent network.

For theory-of-operation work, use this order:

1. Read the completed MEMCAL photograph/direct inspection first to establish the
   as-built resistor and jumper population.
2. Collapse every zero-ohm-connected region to one electrical node.
3. Preserve only nonzero resistors between those nodes.
4. Rename external nodes by network terminal and J4/CAL contact.
5. Add the motherboard destination where traced.
6. Compare the resulting network with KiCad, LTspice, measurements, and firmware
   evidence; treat conflicts as audit items rather than normalizing them away.

The original grid drawing remains valuable evidence and must not be discarded;
it simply should not be mistaken for either the as-built authority or the
simplest functional schematic.

## Reverse-engineering verification strategy

The functional theory can now be used as an independent audit of the physical
reconstruction. A discrepancy may indicate:

- an incorrect resistor reading or transcription;
- a mistaken zero-ohm jumper placement;
- a missed common node or PCB trace;
- a connector/pin orientation error;
- a deliberate available-value substitution in the hand reconstruction; or
- a real circuit relationship whose function is not yet understood.

The strongest closure occurs when the authoritative as-built photograph/direct
inspection, targeted measurements, motherboard connectivity, and
firmware/theory-of-operation consequences agree. LTspice and KiCad then provide
valuable explanatory and historical design context rather than overriding the
physical build.
