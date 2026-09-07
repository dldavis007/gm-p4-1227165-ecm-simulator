# MEMCAL functional-network map

## Purpose

This document separates the electrical function of the two BUA MEMCAL resistor
networks from the generic grid boards used to reconstruct them. The grid/KiCad
layouts remain primary reconstruction evidence; this view collapses zero-ohm
routing links and expresses the networks in terms of package terminals,
absolute carrier pins, J4/CAL contacts, and known motherboard destinations.

This is an evidence cross-reference, not a claim that the inaccessible molded
networks used the same internal physical component layout as the discrete
reconstruction.

## Evidence states used here

- **Observed/reconstructed**: preserved physical, LTspice, KiCad, measurement,
  photograph, or owner-inspection evidence.
- **Mechanically derived**: follows directly from the physically established
  carrier numbering and connector interleave.
- **Motherboard traced**: destination is present in the retained CAL connection
  notes/schematic work.
- **Unresolved**: no functional destination or internal consequence is yet
  established.

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

| Network terminals | CAL nodes | Nominal value |
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

LTspice terminals 7 and 8 are a common node, so the reconstructed model ties
CAL45 and CAL46 to the same electrical node. This is a particularly useful
functional simplification because CAL45 is traced to U11 pin 8 while CAL46 has
no separate destination recorded in the connection notes.

The LTspice model also includes 620 kOhm, 100 kOhm, 470 kOhm, and 220 kOhm
elements through internal/common-node structure. Those elements must remain
represented by their actual SPICE topology rather than being assigned to a
single CAL-to-CAL branch from the generic PCB appearance.

### Functional observations

- CAL42 is not merely a numbered resistor-network terminal: it reaches U11 pin
  18 identified as `OSC`. The 91 kOhm terminal-10-to-terminal-14 branch is
  therefore an evidence-supported **CAL42/OSC-to-CAL34 network relationship**.
  The exact oscillator transfer function remains unresolved.
- CAL34 appears repeatedly as the opposite node of many direct branches. That
  makes it a structurally important common/reference node in the equivalent
  network even though its motherboard destination is not yet identified.
- CAL45/CAL46 being electrically common should be checked against motherboard
  traces and theory of operation before assigning separate semantics to those
  contacts.

## 16055376: 16-terminal network

### Terminal to CAL mapping

| Network terminal | Absolute carrier pin | J4/CAL contact | Motherboard destination |
| ---: | ---: | ---: | --- |
| 1 | 25 | CAL49 | unresolved; source notes appear to duplicate CAL40 here |
| 2 | 26 | CAL51 | U11 pin 24 |
| 3 | 27 | CAL53 | unresolved |
| 4 | 28 | CAL55 | U11 pin 4 |
| 5 | 29 | CAL57 | U11 pin 3 |
| 6 | 30 | CAL59 | 100 Ohm to VIGN |
| 7 | 31 | CAL61 | U11 pin 28, MAP |
| 8 | 32 | CAL63 | unresolved |
| 9 | 35 | CAL64 | unresolved |
| 10 | 36 | CAL62 | unresolved |
| 11 | 37 | CAL60 | U11 pin 7 |
| 12 | 38 | CAL58 | unresolved |
| 13 | 39 | CAL56 | U12 pin 11, CYL |
| 14 | 40 | CAL54 | unresolved |
| 15 | 41 | CAL52 | capacitor / U11 pin 16 |
| 16 | 42 | CAL50 | U11 pin 17 |

The user connection-note series explicitly covers CAL29-CAL61. Therefore the
labels CAL62-CAL64 above are mechanically derived connector positions for
network terminals 8-10, not claims that the original notes named those
positions as CAL signals. Their motherboard identities remain unresolved and
must be checked against the ECM schematic before semantic use.

### Simplified reconstruction relationships

The nominal LTspice model contains nine resistor values:

150 kOhm, 1.5 kOhm, 130 kOhm, 24 kOhm, 10 kOhm, 8.2 kOhm, 5.1 kOhm,
7.5 kOhm, and 10 kOhm.

The generic KiCad board contains routing links and substitute/measured values.
Known direct local relationships translate as follows:

| Local reconstruction relationship | CAL relationship | Reconstructed value/status |
| --- | --- | --- |
| J1-J2 | CAL49-CAL51 | 150 kOhm |
| J2-J3 | CAL51-CAL53 | 1.45 kOhm; nominal model 1.5 kOhm |
| J3-J4 | CAL53-CAL55 | 130 kOhm |
| J4-J5 | CAL55-CAL57 | 0 Ohm routing link |
| J5-J6 | CAL57-CAL59 | 25.5 Ohm recorded in KiCad reconstruction; requires review against 24 kOhm nominal model branch |
| J12-J11 | CAL58-CAL60 | 10 kOhm |

Additional internal-grid branches reach J16/CAL50, J15/CAL52,
J14/CAL54, and J13/CAL56. Their functional simplification must follow the
actual KiCad/SPICE node topology before assigning a direct CAL-to-CAL branch.

### Functional observations

- CAL56 reaches U12 pin 11 identified as `CYL`. The network path feeding this
  terminal is therefore a prime candidate for decoding the fixed cylinder
  selection once U12 behavior is established.
- CAL61 reaches U11 pin 28 identified as `MAP`, placing network terminal 7 at a
  known MAP-related hardware boundary. This does not by itself establish the
  analog transfer function.
- CAL52 reaches a capacitor/U11 pin 16 path and may participate in timing or
  filtering; its exact role remains unresolved.
- CAL59 is tied through 100 Ohm to VIGN on the motherboard, providing another
  useful external reference for validating the reconstructed terminal behavior.

## Why the generic grid should not define the theory of operation

The hand-built boards intentionally use a reusable grid. Zero-ohm parts select
routing paths and many possible grid positions are left open. Consequently the
physical board can look much more complicated than the equivalent network.

For theory-of-operation work, use this order:

1. Collapse every zero-ohm-connected region to one electrical node.
2. Preserve only nonzero resistors between those nodes.
3. Rename external nodes by network terminal and J4/CAL contact.
4. Add the motherboard destination where traced.
5. Only then infer a functional role from schematic and firmware evidence.

The original grid drawing remains valuable evidence and must not be discarded;
it simply should not be mistaken for the simplest functional schematic.

## Reverse-engineering verification strategy

The functional theory can now be used as an independent audit of the physical
reconstruction. A discrepancy may indicate:

- an incorrect resistor reading or transcription;
- a mistaken zero-ohm jumper placement;
- a missed common node or PCB trace;
- a connector/pin orientation error;
- a deliberate available-value substitution in the hand reconstruction; or
- a real circuit relationship whose function is not yet understood.

The strongest closure occurs when physical reconstruction, LTspice/KiCad
terminal behavior, motherboard connectivity, and firmware/theory-of-operation
consequences all agree.
