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

The MEMCAL/J4 interface has **66 contacts total**, arranged as 33 corresponding
positions on each side of the carrier. The original user reverse-engineering
notes explicitly listed only the first 33 contacts because the connections for
the opposite side, contacts 34 through 66, were intended to be extrapolated
from the corresponding carrier positions. The absence of a separately written
34-66 list must not be interpreted as evidence for a 33-contact MEMCAL.

For preservation and future reconstruction, references to a carrier position
1-33 describe a physical position along one side/pairing of the 66-contact
interface unless a document explicitly identifies a J4 contact number. A
complete electrical pin map should distinguish physical carrier position from
absolute J4 contact number.

Two user-supplied photographs made during reverse engineering show the actual
MEMCAL installed in the ECM and a close view of the hand-reconstructed resistor
board. They are primary physical-layout evidence and should be retained with
the project evidence set. The photographs support physical construction and
population observations; individual electrical continuities still require the
connection notes, schematics, or measurement evidence.

## Working physical model

The present working model assumes an off-the-shelf, pin-corresponding carrier
with 33 header positions on each side, for 66 contacts total:

| Carrier positions per side | Population |
| ---: | --- |
| 1-14 | 28-pin EPROM |
| 15-16 | No component lead |
| 17-23 | 16055375 network: 14-pin package, seven leads per side |
| 24 | No component lead |
| 25-32 | 16055376 network: 16-pin package, eight leads per side |
| 33 | No component lead |

This placement is based on user inspection and is sufficient as a working
mechanical model. The network identities are now supported by surviving
LTspice and KiCad reconstruction artifacts, but the final package-pin-to-J4/CAL
continuity remains to be physically verified on the applicable 9340 MEMCAL.
Network pin count and population may differ on other MEMCAL models.

An empty component position does not prove that the corresponding motherboard
contact is unused; the carrier can route a contact elsewhere.

## Historical research-note corroboration

The user-maintained `Usefull Info.pdf` preserves historical research notes that
independently describe 1986-1988 5.0/5.7 TPI MEMCALs as using part numbers
16055375 and 16055376 and characterize them as a **7-pin-per-side** and an
**8-pin-per-side** network, respectively. The same note says the 16055376 is
near the end of the MEMCAL. This independently agrees with the reconstructed
14-terminal and 16-terminal package geometry and with the working carrier
placement above.

The PDF also records a BUA/9340/16059335/1227165/L98/5.7/MD8/G44 application
entry and a later BUA 1728 entry. These are useful historical identity
corroboration, but the notes are preserved as research evidence rather than
substituted for direct hardware, schematic, PROM, or continuity evidence.

A separate note in the PDF says 16055375 "appears to be" replaceable by a
CTS8620/761-3-R15K network. Because that wording is tentative and comes from a
historical research lead, this project does not treat the proposed substitute
as confirmed electrical equivalence.

## Resistor-network reconstruction evidence

The two resistor networks now have separate reconstruction records:

- [`evidence/memcal/MEMCAL_16055375_RECONSTRUCTION_RECORD.md`](../../evidence/memcal/MEMCAL_16055375_RECONSTRUCTION_RECORD.md)
- [`evidence/memcal/MEMCAL_16055376_RECONSTRUCTION_RECORD.md`](../../evidence/memcal/MEMCAL_16055376_RECONSTRUCTION_RECORD.md)

### 16055375

The surviving LTspice model represents 16055375 as a **14-terminal** resistor
network. The physical replacement was implemented on a generic **16-position**
grid board. For this reconstruction, the supported local correspondence is:

- network terminals 1-7 -> J1-J7,
- J8 and J9 -> unused package positions,
- network terminals 8-14 -> J10-J16.

Zero-ohm resistors on the board are configuration/routing jumpers rather than
calibration resistor values. The SPICE model, KiCad board, resistance-analysis
work, and physical photograph mutually support this interpretation.

The reconstruction should be described as an implementation intended to
reproduce the required terminal connectivity/behavior. It is not proven to be
a component-for-component copy of the inaccessible internal construction of
the original molded network.

### 16055376

The surviving LTspice model represents 16055376 as a **16-terminal** resistor
network with nine modeled resistor elements: 150 kOhm, 1.5 kOhm, 130 kOhm,
24 kOhm, 10 kOhm, 8.2 kOhm, 5.1 kOhm, 7.5 kOhm, and 10 kOhm.

The KiCad reconstruction also exposes sixteen positions, J1-J16, so its local
package-position correspondence is direct: network terminal 1 -> J1 through
terminal 16 -> J16.

As with the 16055375 board, the KiCad implementation uses a configurable grid
with zero-ohm links and open positions. Some fitted values differ slightly from
the nominal LTspice values. Those differences are preserved as reconstruction
choices/measurement-derived substitutions rather than silently treated as
proof of the original packaged network's internal values.

## Historical resistor evidence

`Resistors.txt` contains two groups:

- A simple network with nine values: 150 kOhm, 1.5 kOhm, 130 kOhm, 24 kOhm,
  10 kOhm, 8.2 kOhm, 5.1 kOhm, 7.5 kOhm, and 10 kOhm. This matches the nominal
  16055376 LTspice model.
- A more complex network associated with the 14-pin 16055375 reconstruction.

Earlier documentation treated conflicts between the text list and later
reconstruction drawings as unresolved. The surviving LTspice and KiCad files
now establish a much stronger revision/evidence basis for both reconstructed
networks. Remaining uncertainty is primarily whether every fitted physical
value on the actual hand-built carrier exactly matches the design artifacts,
and how each local network pin maps through the carrier to absolute J4/CAL
contacts.

## Required physical continuity verification

Before declaring the MEMCAL carrier pinout complete, perform a physical
continuity check on the actual reverse-engineered carrier. Verify every
accessible EPROM and resistor-network pin against the 66-contact header side,
including both populated and intentionally unconnected positions where
practical.

The measurement record should identify:

| Carrier-side item | Local pin/position | Absolute 66-contact header pin | Verified | Notes |
| --- | ---: | ---: | --- | --- |
| EPROM | | | | |
| 16055375 | | | | |
| 16055376 | | | | |

Where two component-side positions are electrically common, record each
physical continuity independently rather than deriving one from assumed
symmetry. This test is intended to replace the current extrapolation of the
second half of the 66-contact carrier with directly measured hardware evidence.

## CAL connection summary

The user connection notes and ECM schematics associate the following contacts
with named hardware functions:

| CAL contact | Destination or observed role | Evidence state |
| --- | --- | --- |
| CAL29 | ESC network through 100 ohm | Connection confirmed |
| CAL30 | VIGN network through 30 ohm | Connection confirmed |
| CAL32 | Knock circuit/custom-device pin | Connection confirmed |
| CAL33, 35-38, 40-45, 50-52, 55, 57, 60 | U11 custom-device configuration connections | Connection confirmed; individual analog effects unresolved |
| CAL42 | U11 oscillator connection | Connection confirmed |
| CAL52 | Capacitor/U11 timing connection | Connection confirmed |
| CAL56 | Cylinder-selection input | Connection confirmed; exact encoded selection to be proved |
| CAL59 | VIGN network through 100 ohm | Connection confirmed |
| CAL61 | U11 MAP-related input | Connection confirmed |

Several numerically adjacent J4 positions are VCC, ground, blank, or direct
signals rather than CAL inputs. The duplicated `CAL40` in the connection-notes
document is treated as an apparent transcription error, not as a second net.

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
