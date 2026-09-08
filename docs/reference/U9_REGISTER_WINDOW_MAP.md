# U9 custom-peripheral register-window map

## Purpose

This document consolidates the currently supported processor-visible behavior of custom peripheral U9 (`16045148`) and the firmware window at `$3FC0-$3FFF`. It is a functional register map derived from the authoritative assembled listing and previously established schematic endpoints. It does not claim an undocumented internal register design or a one-to-one U9 pin-to-register mapping.

Evidence language follows the project convention:

- **Direct hardware evidence**: a visible schematic pin/net connection.
- **Direct firmware evidence**: emitted instructions, addresses, reads, writes, or data in `evidence/firmware/bua-hac.lst`.
- **Strong inference**: independently proven hardware and firmware endpoints form a coherent path, but the custom device hides the internal mapping.
- **Unresolved boundary**: pin/register correspondence, internal counter structure, waveform generation, polarity, phase, or other behavior not established by available evidence.

## Device-level envelope

The processor schematic establishes U9 as the bus-connected custom peripheral associated with ignition/injection timing and status. Visible U9 signal pins include:

| U9 signal | Pin | Direction at U9 | Connected subsystem |
| --- | ---: | --- | --- |
| `IGN` | 7 | output | U12 ignition/EST path |
| `INJS` | 8 | output | U12 injector-driver section |
| `INJA` | 9 | output | U12 injector-driver section |
| `INJREF` | 38 | input | U12 reference output |
| `IGNREF` | 39 | input | U12 reference output |
| `ESTLOOP` | 41 | input | U12 EST/bypass feedback |
| `KNOCK` | 42 | input | U12 knock output |

These pin functions are direct hardware evidence. The firmware's `$3FC0-$3FFF` accesses are direct processor-visible evidence. Linking an individual pin to an individual address remains a strong inference unless independently documented.

## Processor-visible register map

The following table records only semantics already established by executable use or factory-test exercise.

| Address | Established firmware-visible behavior | Evidence status | Hardware relationship |
| --- | --- | --- | --- |
| `$3FC0` | Reference-period state; copied to RAM `$0095-$0096` at `$CB5A-$CB5D`; written `$FFFF` on excessive no-reference condition at `$CDC1-$CDCA` | Direct firmware evidence | Reference path from U12 `INJREF`/`IGNREF` to U9 is direct hardware evidence; exact contributing pin/counter unresolved |
| `$3FC8` | Spark/reference-period state read during normal spark processing and startup handling | Direct firmware evidence | Coherent with U9 reference/timing role; exact pin/counter mapping unresolved |
| `$3FCA` | Counter whose changes are used as the knock-event quantity at `$D0D1-$D157` and checked by Error-43 logic | Direct firmware evidence | U12 `KNOCK` -> U9 pin 42 is direct; pin 42 -> `$3FCA` is strongly inferred, not documented |
| `$3FCE` | EFI delay register; independently exercised with nonzero values by factory-test code | Direct firmware evidence | Internal relation to `INJS`/`INJA` timing unresolved |
| `$3FD0` | Synchronous injector pulse-width command written by normal firmware and factory test | Direct firmware evidence | U9 outputs `INJS`/`INJA` to U12; exact command-to-waveform mapping unresolved |
| `$3FDC` | Dwell-period value used by spark scheduling and factory test | Direct firmware evidence | U9 `IGN` output participates in U12 ignition path; internal waveform conversion unresolved |
| `$3FE4` | Next-dwell start/timing state, seeded from `$3FEC` | Direct firmware evidence | U9 internal timing implementation unresolved |
| `$3FE6` | Dwell delta/update state | Direct firmware evidence | U9 internal timing implementation unresolved |
| `$3FE8` | Current fire/fall delta used by spark timing | Direct firmware evidence | U9 internal timing implementation unresolved |
| `$3FEC` | Counter value at last reference, used to seed/update dwell state | Direct firmware evidence | Reference-to-counter implementation unresolved |
| `$3FF6` | Reference-to-fire offset written by spark scheduling and factory test | Direct firmware evidence | U9 `IGN` waveform consequence unresolved |
| `$3FFA` | Status word sampled by firmware; high-byte bit 3 marks distributor-reference occurrence during the IRQ interval; sampled status also supplies injector-service bit `$00A0` bit 6 | Direct firmware evidence | U9 receives `INJREF`, `IGNREF`, `ESTLOOP`, `KNOCK`; exact input-to-status-bit mapping unresolved |
| `$3FFC` | Control/status word manipulated for EST enable/bypass and other modes | Direct firmware evidence | U9/U12 EST/bypass hardware loop is direct; individual undocumented bit meanings and polarity remain bounded |

Addresses in the window not listed here remain unclassified by this audit. Absence from this table must not be interpreted as unused hardware.

## Reference and timing group

The strongest reference-related processor-visible sequence is:

1. U12 accepts conditioned distributor `REF` and produces `INJREF` and `IGNREF` to U9 and U11.
2. Firmware samples `$3FFA`; high-byte bit 3 reports a distributor reference during the preceding IRQ interval.
3. Firmware copies `$3FC0` as reference-period state and uses `$3FC8` in spark/reference processing.
4. The firmware normalizes period and derives RPM at `$CDE6-$CE41`.
5. Spark scheduling uses `$3FDC`, `$3FE4`, `$3FE6`, `$3FE8`, `$3FEC`, and `$3FF6` to stage dwell and fire timing.

Steps 2-5 are direct firmware evidence. Step 1 is direct hardware evidence. The internal U9 transformation joining those endpoints is unresolved.

## Injection group

Normal firmware writes synchronous injector pulse width to `$3FD0`; factory-test code independently exercises `$3FD0` and `$3FCE`. The processor-side meaning of `$3FCE` as EFI delay is therefore stronger than a source-comment-only interpretation.

U9 visibly outputs `INJS` and `INJA` to the U12 injector-driver section. U12 then participates in current sensing/limiting and drives `INJOUT` toward Q1. The available evidence supports the following boundary:

`firmware command ($3FD0/$3FCE) -> U9 custom timing -> INJS/INJA -> U12 custom driver/current control -> INJOUT/Q1`

Only the processor-visible command addresses and external pin connections are established. Phase, bank/double-fire implementation, signal polarity, current-control behavior, and the distinction between `INJS` and `INJA` remain unresolved.

## Spark/EST group

Firmware stages spark timing in `$3FDC`, `$3FE4`, `$3FE6`, `$3FE8`, `$3FEC`, and `$3FF6`, and changes `$3FFC` during EST/bypass handling. U9 visibly outputs `IGN` to U12 and receives `ESTLOOP` from U12.

This establishes a closed command/feedback envelope but does not establish:

- which `$3FFC` bit maps electrically to which U9/U12 function;
- the polarity of `IGN` or `ESTLOOP`;
- the U9 timer/counter architecture;
- dwell/fire waveform edge definitions; or
- the exact electrical source of every firmware feedback counter/state.

Error 42 remains evidence of feedback consistency behavior, not proof of an undocumented pin/register mapping.

## Knock group

The hardware path is direct through U12: CAL32 and external `KNOCK#` meet at U12 pin 15, U12 pin 14 outputs `KNOCK`, and that net reaches U9 pin 42. Firmware uses changes in `$3FCA` as the knock-event quantity for attack, recovery, and Error-43 qualification.

The coherent endpoints make U9 pin 42 -> `$3FCA` a strong inference. It is deliberately not promoted to direct evidence because the schematic does not expose U9's internal pin-to-register implementation. The separate CAL29/U10-AN9 analog ESC path remains distinct.

## Status word boundary

`$3FFA` is the most important aggregate status interface currently identified. Firmware directly uses at least:

- high-byte bit 3 for distributor-reference occurrence; and
- the sampled status subsequently represented by `$00A0` bit 6 to gate injector service.

The fact that these are distinct software bits is direct. Their exact U9 input sources are not. In particular, `INJREF` and `IGNREF` must not be assigned one-to-one to those bits without independent evidence.

## Functional grouping, not a fabricated register specification

The current evidence supports grouping the window into four externally meaningful roles:

1. **Reference/counter state** — `$3FC0`, `$3FC8`, `$3FEC`, `$3FFA`.
2. **Knock event state** — `$3FCA`.
3. **Injection command/timing** — `$3FCE`, `$3FD0`.
4. **Spark/dwell/control** — `$3FDC`, `$3FE4`, `$3FE6`, `$3FE8`, `$3FF6`, `$3FFC`.

These groups are useful for theory of operation and HAL design. They are not a claim that U9 internally contains four independent blocks, nor that unlisted addresses have no function.

## Simulation consequence

Step 137 does not justify changing the translated C behavior or frozen baselines. The existing model should continue to expose listing-backed register/state semantics while treating U9's electrical pins, waveform generation, and internal timing logic as an F4 custom-device boundary.

If a future U9 model is added, the safest progression is:

1. model only processor-visible register semantics already established by the listing;
2. expose raw external pin events through an F2 interface;
3. keep phase, polarity, electrical thresholds, and vehicle/sensor assumptions outside the firmware-exact layer; and
4. add pin-to-register behavior only when physical measurement or independent U9 documentation provides direct support.

## Highest-value unresolved questions

- Exact `INJREF`/`IGNREF` contribution to `$3FC0`, `$3FC8`, `$3FEC`, and `$3FFA`.
- Exact `ESTLOOP` contribution to `$3FFA`, `$3FFC`, Error 42, or other feedback state.
- Direct confirmation that U9 pin 42 increments or otherwise produces `$3FCA`.
- Electrical meaning and timing relationship of `INJS` versus `INJA`.
- Exact meaning of undocumented `$3FFC` bits.
- Classification of remaining `$3FC0-$3FFF` addresses not yet reached by established normal/factory firmware paths.
