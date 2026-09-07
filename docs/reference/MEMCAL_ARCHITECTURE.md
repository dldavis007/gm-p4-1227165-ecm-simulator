# MEMCAL architecture and current reconstruction

## Confirmed functional role

The MEMCAL connects the program/calibration EPROM and fixed calibration
networks to the ECM motherboard through J4. The ECM schematic shows a 66-contact
interface with PROM address/data/control signals, CAL29-CAL61 connections,
power and ground, and direct MAP/TPS-related contacts near the end of the
connector.

The PROM interface exposes `A0-A14`, `D0-D7`, `/ROMCS`, and `/ROMOE`. The
processor therefore sees a 32 KiB PROM window at `$8000-$FFFF`. A larger memory
device can hold selectable or mirrored 32 KiB images, but additional memory is
not simultaneously addressable without bank-selection hardware and firmware.

## 66-contact numbering clarification

The MEMCAL/J4 interface has **66 contacts total**. Direct physical inspection of
the reverse-engineered carrier establishes IC-style top/component-side
numbering:

- left side, top to bottom: carrier pins 1-33;
- right side, bottom to top: carrier pins 34-66.

The bottom 66-contact header is interleaved by physical row:

- carrier 1 -> J4 1;
- carrier 2 -> J4 3;
- ...;
- carrier 33 -> J4 65;
- carrier 34 -> J4 66;
- ...;
- carrier 66 -> J4 2.

Equivalent formulas are:

- carrier 1-33: `J4 = 2*carrier - 1`
- carrier 34-66: `J4 = 2*(67-carrier)`

Reverse formulas:

- odd J4 contact H: `carrier = (H+1)/2`
- even J4 contact H: `carrier = 67-H/2`

Physical landmarks are therefore:

- upper-left: carrier 1 / J4 1;
- lower-left: carrier 33 / J4 65;
- lower-right: carrier 34 / J4 66;
- upper-right: carrier 66 / J4 2.

This replaces the earlier shorthand that treated the carrier only as 33 paired
positions. The original notes listed the first 33 component-side positions and
assumed the opposite side could be extrapolated; the absolute top-side carrier
numbering is now explicit for all 66 pins.

Two user-supplied photographs made during reverse engineering show the actual
MEMCAL installed in the ECM and a close view of the hand-reconstructed resistor
board. They are primary physical-layout evidence and should be retained with
the project evidence set.

## Working physical model

With the corrected 66-pin carrier numbering, the component population is:

| Physical row positions | Left-side carrier pins | Right-side carrier pins | Population |
| ---: | --- | --- | --- |
| 1-14 | 1-14 | 66-53 | 28-pin EPROM |
| 15-16 | 15-16 | 52-51 | No direct component lead |
| 17-23 | 17-23 | 50-44 | 16055375, 14 terminals |
| 24 | 24 | 43 | No direct component lead |
| 25-32 | 25-32 | 42-35 | 16055376, 16 terminals |
| 33 | 33 | 34 | No direct component lead |

An empty component position does not prove that the corresponding motherboard
contact is unused; carrier PCB traces can route a contact elsewhere.

The mechanical carrier-to-J4 mapping is now established. What remains open is
the internal electrical routing or semantic meaning of specific contacts where
no component lead or motherboard destination has yet been traced.

## Historical research-note corroboration

The user-maintained `Usefull Info.pdf` preserves historical research notes that
independently describe 1986-1988 5.0/5.7 TPI MEMCALs as using part numbers
16055375 and 16055376 and characterize them as a **7-pin-per-side** and an
**8-pin-per-side** network, respectively. The same note says the 16055376 is
near the end of the MEMCAL. This independently agrees with the reconstructed
14-terminal and 16-terminal package geometry and with the carrier placement
above.

The PDF also records a BUA/9340/16059335/1227165/L98/5.7/MD8/G44 application
entry and a later BUA 1728 entry. These are useful historical identity
corroboration, but the notes are preserved as research evidence rather than
substituted for direct hardware, schematic, PROM, or continuity evidence.

A separate note in the PDF says 16055375 "appears to be" replaceable by a
CTS8620/761-3-R15K network. Because that wording is tentative and comes from a
historical research lead, this project does not treat the proposed substitute
as confirmed electrical equivalence.

## Resistor-network reconstruction evidence

The two resistor networks have separate reconstruction records:

- [`evidence/memcal/MEMCAL_16055375_RECONSTRUCTION_RECORD.md`](../../evidence/memcal/MEMCAL_16055375_RECONSTRUCTION_RECORD.md)
- [`evidence/memcal/MEMCAL_16055376_RECONSTRUCTION_RECORD.md`](../../evidence/memcal/MEMCAL_16055376_RECONSTRUCTION_RECORD.md)

A simplified functional cross-reference is now maintained separately:

- [`docs/reference/MEMCAL_FUNCTIONAL_NETWORKS.md`](MEMCAL_FUNCTIONAL_NETWORKS.md)

The functional document intentionally collapses zero-ohm grid routing and maps
package terminals to absolute carrier pins, J4/CAL contacts, and known
motherboard destinations. The original KiCad/grid records remain preserved as
physical reconstruction evidence.

### 16055375

The surviving LTspice model represents 16055375 as a **14-terminal** resistor
network. The physical replacement was implemented on a generic **16-position**
grid board. For this reconstruction, the supported local correspondence is:

- network terminals 1-7 -> generic positions J1-J7;
- generic positions J8 and J9 -> unused package positions;
- network terminals 8-14 -> generic positions J10-J16.

Absolute MEMCAL mapping is now mechanically established:

- terminals 1-7 -> carrier 17-23 -> CAL33,35,37,39,41,43,45;
- terminals 8-14 -> carrier 44-50 -> CAL46,44,42,40,38,36,34.

This means CAL33-CAL46 exactly spans the complete 14-terminal network.
Zero-ohm resistors on the hand-built board are configuration/routing jumpers,
not calibration resistor values.

### 16055376

The surviving LTspice model represents 16055376 as a **16-terminal** resistor
network with nine modeled resistor elements: 150 kOhm, 1.5 kOhm, 130 kOhm,
24 kOhm, 10 kOhm, 8.2 kOhm, 5.1 kOhm, 7.5 kOhm, and 10 kOhm.

The KiCad reconstruction exposes sixteen local positions J1-J16, so local
network terminal 1 -> J1 through terminal 16 -> J16.

Absolute MEMCAL mapping is now mechanically established:

- terminals 1-8 -> carrier 25-32 -> J4 49,51,53,55,57,59,61,63;
- terminals 9-16 -> carrier 35-42 -> J4 64,62,60,58,56,54,52,50.

For contacts above the CAL61 range explicitly present in the user's connection
notes, J4 62-64 are retained as absolute connector contacts without assigning
unsupported CAL semantics.

As with the 16055375 board, the generic-grid implementation uses zero-ohm links
and open positions. Some fitted values differ slightly from nominal LTspice
values. Those differences are preserved as reconstruction choices or potential
review points rather than silently treated as proof of the original molded
network's internal values.

## Historical resistor evidence

`Resistors.txt` contains two groups:

- A simple network with nine values: 150 kOhm, 1.5 kOhm, 130 kOhm, 24 kOhm,
  10 kOhm, 8.2 kOhm, 5.1 kOhm, 7.5 kOhm, and 10 kOhm. This matches the nominal
  16055376 LTspice model.
- A more complex network associated with the 14-pin 16055375 reconstruction.

The surviving LTspice and KiCad files now establish a strong revision/evidence
basis for both reconstructed networks. The corrected 66-pin numbering removes
the former package-pin-to-J4 uncertainty. Remaining uncertainty is primarily
whether every fitted physical value/jumper on the hand-built reconstruction is
correct and what electrical/functional consequences the custom-device inputs
produce.

## Functional-network interpretation

The hand-built resistor boards were intentionally designed as reusable generic
grids. Therefore their visual complexity must not be used as the theory of
operation.

For functional interpretation:

1. collapse all zero-ohm-connected grid regions to electrical nodes;
2. retain the nonzero resistor branches between those nodes;
3. label external nodes by package terminal and J4/CAL contact;
4. add the traced motherboard destination;
5. compare that equivalent network against the LTspice model and firmware or
   hardware theory of operation.

This method provides an independent audit of the reconstruction. If the
functional result conflicts with the expected hardware role, the discrepancy
should be investigated as a possible resistor-reading error, jumper-placement
error, missed trace/common node, deliberate substitution, or unresolved circuit
behavior.

## CAL connection summary

The user connection notes and ECM schematics associate the following contacts
with named hardware functions:

| J4/CAL contact | Network relationship | Destination or observed role | Evidence state |
| --- | --- | --- | --- |
| CAL29 | no direct network lead at carrier 15 | ESC network through 100 Ohm | Connection confirmed |
| CAL30 | no direct network lead at carrier 52 | 30 Ohm to VIGN | Connection confirmed |
| CAL32 | no direct network lead at carrier 51 | U12 pin 15, KNOCK | Connection confirmed |
| CAL33 | 16055375 pin 1 | U11 pin 22 | Connection confirmed |
| CAL34 | 16055375 pin 14 | destination unresolved | Mechanical mapping confirmed |
| CAL35 | 16055375 pin 2 | U11 pin 21 | Connection confirmed |
| CAL36 | 16055375 pin 13 | U11 pin 12 | Connection confirmed |
| CAL37 | 16055375 pin 3 | U11 pin 9 | Connection confirmed |
| CAL38 | 16055375 pin 12 | U11 pin 11 | Connection confirmed |
| CAL39 | 16055375 pin 4 | destination unresolved | Mechanical mapping confirmed |
| CAL40 | 16055375 pin 11 | U11 pin 10 | Connection confirmed |
| CAL41 | 16055375 pin 5 | U11 pin 1 | Connection confirmed |
| CAL42 | 16055375 pin 10 | U11 pin 18, OSC | Connection confirmed |
| CAL43 | 16055375 pin 6 | U11 pin 2 | Connection confirmed |
| CAL44 | 16055375 pin 9 | U11 pin 13 | Connection confirmed |
| CAL45 | 16055375 pin 7 | U11 pin 8 | Connection confirmed |
| CAL46 | 16055375 pin 8 | destination unresolved | Mechanical mapping confirmed |
| CAL47 | no direct network lead at carrier 24 | destination unresolved | Mechanical mapping confirmed |
| CAL48 | no direct network lead at carrier 43 | destination unresolved | Mechanical mapping confirmed |
| J4 49 | 16055376 pin 1 | source-note sequence likely intended CAL49; destination unresolved | Mechanical mapping confirmed; label correction inferred |
| CAL50 | 16055376 pin 16 | U11 pin 17 | Connection confirmed |
| CAL51 | 16055376 pin 2 | U11 pin 24 | Connection confirmed |
| CAL52 | 16055376 pin 15 | capacitor / U11 pin 16 | Connection confirmed |
| CAL53 | 16055376 pin 3 | destination unresolved | Mechanical mapping confirmed |
| CAL54 | 16055376 pin 14 | destination unresolved | Mechanical mapping confirmed |
| CAL55 | 16055376 pin 4 | U11 pin 4 | Connection confirmed |
| CAL56 | 16055376 pin 13 | U12 pin 11, CYL | Connection confirmed; exact encoded selection unresolved |
| CAL57 | 16055376 pin 5 | U11 pin 3 | Connection confirmed |
| CAL58 | 16055376 pin 12 | destination unresolved | Mechanical mapping confirmed |
| CAL59 | 16055376 pin 6 | 100 Ohm to VIGN | Connection confirmed |
| CAL60 | 16055376 pin 11 | U11 pin 7 | Connection confirmed |
| CAL61 | 16055376 pin 7 | U11 pin 28, MAP | Connection confirmed |

The duplicated `CAL40` in the historical connection-notes document after
CAL48 is treated as an apparent sequence/transcription error, probably CAL49.
That correction remains explicitly inferential until corroborated by schematic
or physical trace evidence.

## Remaining physical verification

A blanket continuity test of every component pin solely to establish the 66-pin
mechanical numbering is no longer required; the carrier/header numbering has
been physically established.

Targeted continuity measurements remain valuable where they can answer an
actual unresolved electrical question, especially:

- carrier positions with no direct component lead;
- blank motherboard destinations;
- common nodes suggested by the LTspice model;
- any reconstructed resistor/jumper relationship that conflicts with theory of
  operation;
- EPROM electrical signal mapping where direct schematic evidence is absent.

## Modeling boundary

The behavioral C core should normally consume the signals produced by the ECM
hardware rather than simulate every MEMCAL resistor. Use a hardware profile or
HAL configuration only where a fixed MEMCAL selection changes processor-visible
behavior. Keep these layers distinct:

1. Resistor population and carrier routing.
2. Custom-device analog or timing effect.
3. Processor-visible signal, register bit, interrupt, or count.
4. Executable firmware response.

Only the fourth layer is automatically established by the PROM source. A
semantic claim spanning the earlier layers requires corresponding schematic,
measurement, or test evidence.
