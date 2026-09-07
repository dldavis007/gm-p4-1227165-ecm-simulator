# MEMCAL 16055376 Resistor-Network Reconstruction Record

## Purpose

This record consolidates the surviving evidence for the reverse-engineered 16055376 MEMCAL resistor network and its generic-PCB implementation. It is intended to preserve what is directly supported by the LTspice and KiCad source artifacts without overstating internal one-for-one component equivalence.

## Evidence sources

The principal source artifacts are:

1. `NetRes 16055376.asc` — LTspice schematic/model.
2. `NetRes_16055376.kicad_pcb` — KiCad PCB reconstruction.
3. `NetRes 16055375 and 16055376.ods` — historical comparison/measurement workbook.
4. The physical reverse-engineered MEMCAL and user-supplied close-up photograph of the generic resistor/jumper board.

## Original-network terminal count

The LTspice source explicitly labels terminals **1 through 16**. Unlike the 16055375 network, which is modeled as a 14-terminal package, the 16055376 evidence supports a full **16-terminal network**.

The KiCad reconstruction likewise provides connector positions J1 through J16. For this network, the simplest supported local correspondence is therefore:

| Original network terminal | KiCad carrier position |
|---:|---:|
| 1 | J1 |
| 2 | J2 |
| 3 | J3 |
| 4 | J4 |
| 5 | J5 |
| 6 | J6 |
| 7 | J7 |
| 8 | J8 |
| 9 | J9 |
| 10 | J10 |
| 11 | J11 |
| 12 | J12 |
| 13 | J13 |
| 14 | J14 |
| 15 | J15 |
| 16 | J16 |

This is a **local network-to-carrier mapping only**. Absolute correspondence to the 66-contact MEMCAL/J4 connector remains subject to physical continuity verification of the actual carrier.

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

The schematic also contains terminal-resistance annotations and simulation sources used to characterize the network behavior.

## KiCad generic-grid implementation

The KiCad PCB is a 16-position generic grid implementation. It uses a matrix of populated resistors, zero-ohm links, and open positions to realize the required connectivity on a reusable board pattern.

The PCB contains direct adjacent-position components and a second internal row of configurable links. Notable populated values include:

| KiCad ref | Value | Local role |
|---|---:|---|
| R1 | 150 kOhm | between J1 and J2 |
| R2 | 1.45 kOhm | between J2 and J3 |
| R3 | 130 kOhm | between J3 and J4 |
| R4 | 0 Ohm | routing link between J4 and J5 |
| R5 | 25.5 Ohm | between J5 and J6 |
| R12 | 10 kOhm | between J12 and J11 |
| R15 | 0 Ohm | grid routing link from J1 |
| R17 | 0 Ohm | grid routing link from J3 |
| R21 | 0 Ohm | grid routing link from J7 |
| R23 | 10 kOhm | internal-grid branch to J16 |
| R24 | 8.2 kOhm | internal-grid branch to J15 |
| R25 | 5 kOhm | internal-grid branch to J14 |
| R26 | 7.4 kOhm | internal-grid branch to J13 |
| R30 | 0 Ohm | grid routing link to J9 |
| R32 | 0 Ohm | internal routing link |
| R33 | 0 Ohm | internal routing link |
| R37 | 0 Ohm | internal routing link |

Numerous other grid positions are explicitly marked `Open`.

## Interpretation of value differences

The KiCad board should **not** be interpreted as a component-for-component drawing of the inaccessible internal 16055376 package.

Several populated PCB values are close to, but not numerically identical to, values in the LTspice representation. Examples include:

- 1.45 kOhm on the PCB versus 1.5 kOhm in the SPICE model.
- 5.0 kOhm on the PCB versus 5.1 kOhm in the SPICE model.
- 7.4 kOhm on the PCB versus 7.5 kOhm in the SPICE model.

The PCB also includes values and zero-ohm links that reflect the generic-grid routing implementation rather than a literal internal schematic translation.

Accordingly, the strongest supported interpretation is that the PCB was constructed to reproduce the required **terminal-to-terminal electrical behavior/connectivity** of the 16055376 network using available discrete values and configurable links.

## Zero-ohm and open positions

The same documentation rule used for the 16055375 reconstruction applies here:

- `0 Ohm` parts are routing/configuration jumpers.
- `Open` parts are intentionally unpopulated grid positions.
- Neither should be treated as calibration resistor values.

This distinction is important because the generic board contains many more possible resistor locations than the original network contains active resistor elements.

## Relationship to the physical MEMCAL

The user has the physical reverse-engineered MEMCAL and can directly continuity-test the EPROM/network side of the carrier against the 66-contact header side.

That physical test should eventually establish, pin by pin:

- 16055376 local terminal J1-J16 -> absolute MEMCAL/J4 contact.
- 16055375 local network positions -> absolute MEMCAL/J4 contact.
- EPROM pins -> absolute MEMCAL/J4 contact.
- intentionally open or common positions.

The continuity measurements should be recorded explicitly rather than inferred from carrier symmetry.

## Confidence statement

Current confidence is high for the following local conclusions:

- 16055376 is represented by a 16-terminal network.
- The KiCad reconstruction uses all sixteen generic carrier positions J1-J16 as the local terminal set.
- The LTspice source preserves the original/equivalent resistor network model values.
- The KiCad PCB implements the required behavior with discrete resistors plus zero-ohm routing links and open grid positions.
- The SPICE and KiCad representations are complementary evidence, not necessarily literal one-for-one internal schematics.

Absolute mapping from these sixteen local terminals to the 66-contact MEMCAL/J4 header remains open pending direct physical continuity verification.
