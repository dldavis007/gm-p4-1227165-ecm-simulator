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

## Working physical model

The present working model assumes an off-the-shelf, pin-corresponding carrier
with 33 header positions on each side:

| Carrier positions per side | Population |
| ---: | --- |
| 1-14 | 28-pin EPROM |
| 15-16 | No component lead |
| 17-23 | Network nearest EPROM: 14-pin package, seven leads per side |
| 24 | No component lead |
| 25-32 | Second network: 16-pin package, eight leads per side |
| 33 | No component lead |

This placement is based on user inspection and is sufficient as a working
mechanical model. Exact package identity and pin-to-CAL continuity remain to be
verified on the applicable 9340 MEMCAL. Network pin count and population may
differ on other MEMCAL models.

An empty component position does not prove that the corresponding motherboard
contact is unused; the carrier can route a contact elsewhere.

## Resistor evidence

`Resistors.txt` contains two groups:

- A simple network with nine values: 150 kOhm, 1.5 kOhm, 130 kOhm, 24 kOhm,
  10 kOhm, 8.2 kOhm, 5.1 kOhm, 7.5 kOhm, and 10 kOhm.
- A more complex network associated with the 14-pin reconstruction.

The latest complex-network drawing differs from the text list in several
values and designators. These differences are preserved as unresolved rather
than silently choosing one version. The original LTspice `.asc` files, dated
design revisions, or direct measurements would establish the correct netlist.

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

