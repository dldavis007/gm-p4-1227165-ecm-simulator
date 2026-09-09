# Interrupt and scheduler architecture

## Purpose

This chapter consolidates the listing-backed ordinary interrupt and scheduler architecture of the supplied BUA/9340 firmware. It describes cadence, ordering, minor-loop selection, major-segment dispatch, and the boundary between periodic scheduling and asynchronous/reference hardware. It does not turn U9/U11/U12 into invented timer hardware.

## Core cadence

The established ordinary IRQ cadence is **6.25 ms**. Each ordinary IRQ performs common work and then selects exactly one of two alternating minor-loop sides. Consequently:

- common IRQ services execute every 6.25 ms;
- the odd Air/Fuel side executes every 12.5 ms;
- the even Spark/O2/reference side executes every 12.5 ms;
- the low nibble of the scheduler state selects one of sixteen major segments;
- each major segment therefore executes once per sixteen ordinary IRQs, or every **100 ms**, when ordinary IRQ service proceeds normally.

The 100-ms major cadence is mechanically derived from 16 x 6.25 ms; it is not a separate assumed timer.

The translated scheduler regression verifies 16 IRQ calls produce 16 ordinary IRQ services, 16 scheduler-facing IAC services, eight odd selections, eight even selections, and exactly one call to each major segment. It also verifies that odd/even phase returns to its starting parity after sixteen IRQs.

## Ordinary IRQ ordering

The source-backed structure represented by the C port preserves a hierarchy rather than treating all periodic work as independent timers:

1. enter ordinary IRQ service;
2. perform common 6.25-ms work that is ahead of the minor split;
3. service scheduler-facing IAC work at the ordinary IRQ cadence;
4. advance/select the alternating minor side;
5. execute either the odd Air/Fuel chain or the even Spark/O2/reference chain;
6. dispatch the major segment selected by the scheduler state's low nibble;
7. perform the ordinary serial/output timing work established for that IRQ path.

Exact helper placement should continue to follow the authoritative listing and existing translated source. This chapter defines the scheduling hierarchy, not permission to reorder helpers for convenience.

## Common 6.25-ms work

Several services are intentionally outside the odd/even split. The current translated path establishes common MAF acquisition/processing and scheduler-facing IAC service at 6.25 ms. VSS capture checking is also a high-rate scheduler-facing activity in the translated architecture, distinct from the slower major-segment road-speed calculation.

This distinction matters: a producer may sample or maintain state every IRQ while a consumer computes a slower control result on a minor or major cadence.

For example, the regression evidence verifies the common MAF producer executes on both odd and even IRQs while the normal fuel chain executes only on the odd side.

## Odd 12.5-ms Air/Fuel side

The odd minor side is the normal Air/Fuel path. Listing-backed translations connected here include fast O2/fuel-chain work and the resulting injector-command path. The exact fuel calculations are documented in their subsystem chapters; scheduler architecture only establishes that this side is selected on alternate ordinary IRQs.

Ignition-shutdown state can alter whether the ordinary odd fuel chain proceeds. That lifecycle behavior is preserved as a source-order condition rather than modeled as a different periodic clock.

## Even 12.5-ms Spark/O2/reference side

The even minor side begins with the established diagnostic-word merge and slow-O2 front end, then performs reference/engine-running qualification and related RPM/dwell work where the translated conditions allow it.

Regression evidence establishes that the slow-O2 filter runs exactly on the even 12.5-ms cadence. Once engine-running/reference state is valid, the connected RPM and dwell helpers also update at that even cadence in the C translation.

This does **not** mean distributor reference edges occur at 12.5-ms intervals. Reference edges and periods originate in the U12/U9 hardware path; the even minor consumes processor-visible state derived from that hardware.

## Reference-event versus scheduler time

The ignition/reference architecture has two different notions of time that must not be conflated:

- **scheduler time**: ordinary 6.25-ms IRQ cadence and its 12.5/100-ms derived tasks;
- **engine-event time**: distributor reference occurrence/period state presented through the U12/U9 custom-device path.

Firmware reads U9-window state including reference status at `$3FFA`, reference period at `$3FC0`, and related timing state such as `$3FC8`. RPM calculation around `$CDE6-$CE41` consumes reference-period information. These accesses prove the software-facing interface; they do not prove the internal U9 counter clock, capture circuit, edge polarity, or a direct one-register-per-pin mapping.

CAL42/U11 `OSC` and CAL56/U12 `CYL` also remain distinct from the ordinary IRQ clock and from distributor-reference timing unless new evidence explicitly joins them.

## Major-segment dispatcher

The major loop is a sixteen-way dispatcher selected by the low nibble of scheduler state (`L0000` in the listing-backed cross-reference). Over sixteen ordinary IRQs, segments `0` through `F` are each selected once. This gives each segment a nominal 100-ms service cadence.

The major segments distribute slower control and housekeeping work rather than forming sixteen independent threads. Existing translated examples include VSS road-speed calculation, optional `$5800` expansion-ROM dispatch, emissions/accessory control, diagnostics, transmission/TCC, and other subsystem work.

A major segment's 100-ms entry cadence does not imply every operation inside it always changes state: source conditions, mode bits, timers and hardware state can cause early exits or conditional work.

## IAC cadence as an ordering anchor

Idle Air Control (IAC) provides a useful proof of the hierarchy. The established scheduler regression verifies IAC motor service is entered once per ordinary 6.25-ms IRQ before the minor split, while higher-level IAC command production occurs on slower established paths. A persistent packed movement request can therefore consume one physical-step request per ordinary IRQ service without requiring the command producer itself to run every 6.25 ms.

This producer/service separation should be preserved in embedded and PC implementations.

## Serial timing and interrupt boundaries

The normal 160-baud path is tied to ordinary IRQ timing but includes additional hardware compare events. The translated source records the ordinary IRQ scheduling of a first compare at +11 timer counts and a later +143 compare, with the output pulse width determined by the shifted data bit. These compare events are not extra scheduler minor loops; they are hardware/event timing associated with the serial waveform.

The 8192-baud path has its own SCI-facing interrupt/protocol behavior. Its software lifecycle is translated, while physical SCI byte timing and ALDL transceiver behavior remain hardware boundaries.

The vector table also contains exceptional entries distinct from the ordinary scheduler. Step 119 established the emitted vector words and an immediate `RTI` at `$F27B`; unsupplied `$6000` behavior and processor-specific vector naming/consequences remain explicit boundaries.

## Simulator implementation rule

The PC simulator should model the scheduler deterministically from the ordinary IRQ event rather than create unrelated host threads for every firmware cadence. A suitable hierarchy is:

`6.25-ms IRQ event -> common service -> odd/even 12.5-ms branch -> one of 16 major segments`

External events such as reference pulses, VSS captures, SCI activity, and compare events should update their processor-visible state at the appropriate event time, then let the translated firmware consume that state according to source ordering.

This keeps host scheduling jitter from becoming an accidental firmware feature and preserves the F0/F2/F3/F4 split:

- F0: source-backed IRQ ordering, counters, branches and dispatch;
- F2: injected reference/VSS/serial events and observable raw outputs;
- F3: optional engine/vehicle/sensor timing models;
- F4: undocumented custom-device electrical/timing transformations.

## Established timing summary

| Activity | Established cadence/source | Classification |
| --- | --- | --- |
| ordinary IRQ | 6.25 ms | confirmed firmware architecture |
| common MAF / scheduler-facing common work | every ordinary IRQ where established | F0 |
| IAC motor service | 6.25 ms | F0 service cadence; motor physics F3/F4 |
| odd Air/Fuel minor | every other IRQ = 12.5 ms | F0 |
| even Spark/O2/reference minor | every other IRQ = 12.5 ms | F0 |
| slow O2 front end | even minor = 12.5 ms | F0 |
| major segment 0-F | one segment/IRQ; each segment every 16 IRQs = 100 ms | F0 |
| distributor reference edges | engine-event dependent | F2 input/state; U12/U9 transformation F4 |
| U9 reference-period/status consumption | firmware locations established by listing | F0 access, custom internals F4 |
| 160-baud compare events | ordinary-IRQ-triggered + hardware compare timing | F0 scheduling; timer/electrical realization F4 |
| 8192-baud SCI events | SCI/protocol event driven | F0 software lifecycle; physical timing F4 |

## What this chapter closes

The ordinary scheduler geometry is now sufficiently established to serve as the theory-of-operation Chapter 5: 6.25-ms IRQ, alternating 12.5-ms minors, sixteen-way 100-ms major dispatch, common-service ordering, and separation of scheduler time from engine/reference events.

The following remain intentionally unresolved or outside this chapter:

- the physical source and exact electrical generation of the ordinary IRQ;
- undocumented internal U9 timer/capture architecture;
- any unsupported relationship between U11 `OSC` and processor/U9 timebases;
- processor-specific vector names not directly established by project evidence;
- electrical timing/polarity inside U9/U11/U12 and output/transceiver hardware; and
- host/plant timing assumptions beyond processor-visible event interfaces.
