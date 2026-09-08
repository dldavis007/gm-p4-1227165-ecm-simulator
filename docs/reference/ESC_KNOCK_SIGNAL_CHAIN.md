# ESC and knock signal chain

## Scope

This audit separates the analog electronic-spark-control (ESC) monitor path
through CAL29 and U10 from the knock-event path through CAL32, U12, and U9. It
then follows only the processor-visible behavior established by the assembled
9340 listing.

A shared system purpose does not make the two electrical paths interchangeable.
No undocumented threshold, polarity, pulse-shaping, or register mapping is
assigned to U12 or U9.

## Evidence classes

- **Direct hardware evidence**: a visible schematic component, pin, or net.
- **Mechanically derived mapping**: a CAL identity obtained from the established
  physical 66-pin carrier/J4 correspondence.
- **Direct firmware evidence**: an emitted instruction, byte, RAM access, or
  custom-peripheral register access in `bua-hac.lst`.
- **Strong inference**: independently established endpoints agree, while a
  custom-device interior prevents a complete causal proof.
- **Unresolved**: the evidence does not establish the behavior.

## Two distinct physical paths

| Path | External/MEMCAL node | Visible motherboard circuit | Processor-side endpoint | Evidence conclusion |
| --- | --- | --- | --- | --- |
| Analog ESC monitor | `ESC#` to CAL29 | 100 Ohm series into CAL29; capacitor to ground; 4.02 kOhm pull-up; 200 kOhm from CAL29 to `ESC` | `ESC` reaches U10 AN9, selector `$90` | Direct electrical path; normal-image functional use not demonstrated |
| Knock event | `KNOCK#` and CAL32 | `KNOCK#` through 10 kOhm to U12 pin 15; CAL32 joins pin 15 with capacitor and 470 kOhm to ground | U12 pin 14 `KNOCK` reaches U9 pin 42 | Direct endpoints; U12 threshold/polarity and U9 register mapping unresolved |

CAL29 is absolute carrier pin 15/J4-CAL29 and CAL32 is absolute carrier pin
51/J4-CAL32. Neither is a terminal of the two reconstructed resistor-network
packages. Their identification follows the established carrier/CAL mapping and
the motherboard schematic rather than an inferred resistor-network function.

## CAL29 and U10 AN9

The input schematic directly establishes the analog path from `ESC#` through
CAL29 to net `ESC`, and the processor schematic places `ESC` at U10 AN9. The
channel selector is therefore `$90` by the visible U10 channel order.

The firmware evidence is deliberately narrower:

- The normal image has no explicit `$90` request immediately before the common
  A/D routine `$F1BE`.
- Factory test `$FDB5-$FDC5` does request `$90` as the tenth sample and stores
  it at `$0184`.

Thus the assembled image proves that AN9 can be observed in factory test, but
does not prove that its raw count drives normal knock retard or Error 43. Any
claim that CAL29 resistance or AN9 voltage directly sets a normal-operation
knock threshold would exceed the evidence.

## CAL32, U12, and U9

CAL32 and the external `KNOCK#` input meet at U12 pin 15. U12 pin 14 outputs a
separate net named `KNOCK`, which terminates at U9 pin 42. This is direct
hardware evidence for a custom-device knock-event path.

The schematic does not disclose:

- the electrical threshold or polarity at U12 pin 15;
- whether CAL32 is a bias, sensitivity, hysteresis, or variant-selection node;
- the pulse width or filtering at U12 pin 14; or
- which U9 counter/status field is driven by pin 42.

Those details remain unresolved even though the end-to-end purpose is strongly
supported.

## Firmware knock-event consumption

Normal spark processing at `$D0D1-$D157` reads U9-window register `$3FCA`,
subtracts the prior snapshot at `$00A1`, and reduces the difference to an
8-bit event quantity. It then applies coolant, RPM, speed, battery, mode, and
fault qualifications before multiplying that quantity by the RPM-indexed
attack-rate table `LC1BE-LC1C2`. The result accumulates into knock retard
`$00A5` and is limited by normal or wide-open-throttle calibration.

This directly proves that `$3FCA` deltas are the executable input to knock
attack. The listing's factory-address table calls `$3FCA` “Counter #3” and
associates it with a high input, while the schematic supplies U9 pin 42
`KNOCK`. Their relationship is a strong inference, not a proven one-to-one pin
map.

The principal state is:

| State | Direct firmware role |
| --- | --- |
| `$3FCA-$3FCB` | U9-window counter sampled for knock-event and failure timing |
| `$00A1-$00A2` | prior `$3FCA` snapshot used for event delta |
| `$00A5` | accumulated knock-retard value |
| `$0040` bit 7 | knock-control qualification/enable state |
| `$0003` bit 7 | coolant-rise enable latch |
| `$0002` bit 7 | knock-failure/fail-safe mode input to retard selection |
| `$003B` bit 5 | Error-43 low-activity precursor/inhibit state |
| `$00A3` | prior high byte used by the Error-43 counter-activity test |
| `$00A4` | cadence counter for the Error-43 activity check |

At `$D157`, firmware halves `$00A5` before subtracting it from spark relative
to reference. This scale conversion is direct executable behavior; it does not
establish electrical degrees per U12 pulse.

## Recovery and Error 43

Segment A `$EB3A-$EB59` applies an RPM-indexed percentage recovery to `$00A5`
and forces at least a one-count decrement when recovery executes.

The Error-43 path is distinct from immediate knock attack:

1. `$EB5B-$EB8D` manages startup/test eligibility using calibration enable,
   scheduler state, load, coolant, and mode flags.
2. `$EB8F-$EBB1` periodically compares the high byte of `$3FCA` with `$00A3`.
3. If the change reaches the `LC229` criterion, `$003B` bit 5 is set as an
   Error-43 precursor.
4. `$E6DF-$E6E9` latches Error 43 in `$004E` bit 4 from that precursor or the
   fail-mode bit in `$0002`.
5. `LC22A` supplies fixed fail-safe knock retard when the failure mode is
   active; `LC22B` participates in the diagnostic spark test.

The nearby historical listing comments contain inconsistencies, including an
Error-42 reference where the code is gating Error 43 and stale “MAP Hi” labels
for `$00B0`. The emitted operands and control flow are authoritative: `$00B0`
is used only as a qualification input here, while `$3FCA` activity supplies the
counter-based test.

## Hardware/firmware conclusion

The supported end-to-end interpretation is:

1. CAL32 and `KNOCK#` condition a U12 input.
2. U12 produces a `KNOCK` output to U9.
3. U9 exposes counter state that firmware uses for knock attack and an
   activity-based failure test.
4. Firmware converts the event quantity into calibrated retard, subtracts it
   from spark, recovers it over time, and applies Error-43 fail-safe behavior.

Steps 1 and 4 are direct evidence. Steps 2's visible endpoints are direct, but
its transfer is unresolved. The pin-42-to-`$3FCA` portion of step 3 is strong
inference. CAL29/U10 AN9 is a separate factory-observable analog monitor and is
not inserted into this chain without normal-image evidence.

## C and verification boundary

The C implementation translates the listing-backed `$3FCA` delta consumer,
qualification, attack, limit, recovery, spark subtraction, and Error-43 latch.
It accepts hardware-event/counter state at the HAL/custom-peripheral boundary
and does not synthesize U12 or U9 internals. Existing fuel/spark, major-loop,
diagnostic, factory-test, and frozen-profile regressions cover those behaviors.
No source or baseline change is justified by Step 135.

## Remaining evidence needs

- Measure CAL29/AN9 voltage during controlled ESC conditions and compare the
  factory-test `$0184` sample.
- Measure CAL32 and U12 pins 15/14 during controlled knock stimulation.
- Correlate U9 pin 42 pulses with `$3FCA` changes and `$00A1/$00A5` state.
- Establish U12 threshold, polarity, filtering, and CAL32's electrical role
  from authoritative device information or controlled testing.
- Determine whether another PROM variant explicitly samples U10 selector
  `$90` during normal operation.
