# MEMCAL 16055376 Topology Audit

## Purpose

This record reduces the generic-grid 16055376 reconstruction to its terminal-level electrical network. The completed MEMCAL photograph remains authoritative for the as-built component population and values. The KiCad file is used here only to trace the generic-grid connectivity; LTspice is used as an independent equivalent-network comparison.

## As-built nonzero population

The completed photograph shows the nine installed nonzero values:

- 150 kOhm (`1503`)
- 1.5 kOhm (`1501`)
- 130 kOhm (`1303`)
- 24 kOhm (`2402`)
- 10 kOhm (`1002`)
- 8.2 kOhm (`8201`)
- 5.1 kOhm (`5101`)
- 7.5 kOhm (`7501`)
- 10 kOhm (`1002`)

These exactly match the LTspice nominal value set. KiCad value differences remain preserved as design-record discrepancies and do not define the as-built values.

## Zero-ohm collapse from KiCad connectivity

The generic KiCad grid uses these populated zero-ohm links:

- R4: J4 to J5
- R15: J1 to internal node N17
- R17: J3 to internal node N19
- R21: J7 to internal node N23
- R30: internal node N24 to J9
- R32: internal node N18 to N19
- R33: internal node N19 to N20
- R37: internal node N23 to N24

Collapsing those links gives the following electrical node identities:

- **J4 = J5**
- **N17 = J1**
- **N18 = N19 = N20 = J3**
- **N23 = N24 = J7 = J9**

The open grid positions leave **J8 isolated** and **J10 isolated**.

## Simplified terminal-level network

After zero-ohm collapse and replacement of KiCad design values with photo-authoritative as-built values, the network reduces to:

| Terminals | As-built value / relationship |
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

No additional nonzero branch survives the generic-grid simplification.

## LTspice comparison

The LTspice model independently reduces to the same terminal topology:

- terminals 1-2: 150 kOhm
- terminals 2-3: 1.5 kOhm
- terminals 3-4/5 common node: 130 kOhm
- terminals 4/5 common node-6: 24 kOhm
- terminals 1-16: 10 kOhm
- terminals 3-15: 8.2 kOhm
- terminals 3-14: 5.1 kOhm
- terminals 3-13: 7.5 kOhm
- terminals 12-11: 10 kOhm
- terminals 7 and 9 common
- terminal 8 open
- terminal 10 open

Thus the simplified KiCad connectivity and LTspice topology agree exactly once the zero-ohm routing is collapsed. The completed photograph also confirms the complete nine-resistor value set used in that topology.

## Absolute J4/CAL functional form

Using the established 16055376 package-to-J4 mapping, the same network is:

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

The labels J4 62-64 remain connector numbers unless motherboard schematic evidence establishes CAL semantics beyond CAL61.

## Functional anchors

This simplified network immediately exposes several useful hardware relationships:

- CAL56 / U12 pin 11 (`CYL`) is connected to CAL53 through 7.5 kOhm.
- CAL52 / capacitor-U11 pin 16 is connected to CAL53 through 8.2 kOhm.
- CAL59 / 100-Ohm-to-VIGN path is connected to the CAL55/CAL57 common node through 24 kOhm.
- CAL61 / U11 pin 28 (`MAP`) is directly common with J4 64 inside the MEMCAL network.
- CAL55 and CAL57 are directly common inside the MEMCAL network.

These are electrical relationships, not claims about the internal transfer functions of U11 or U12.

## Closure status

For the reverse-engineered 16055376 network, the following are now strongly closed:

- 16-terminal package mapping;
- complete nine-resistor as-built value set;
- zero-ohm routing collapse;
- terminal-level topology;
- exact topology agreement with LTspice.

Remaining work is no longer resistor-network reconstruction. It is motherboard/theory interpretation of unresolved connector destinations and custom-device behavior, especially J4 62-64, CAL53/CAL54, and the CYL/MAP-related consequences.