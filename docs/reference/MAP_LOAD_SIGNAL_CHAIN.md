# MAP and firmware-load signal chain

## Purpose

This document separates three related but non-equivalent concepts in the
1227165 ECM:

1. the external MAP and MAP2 analog inputs visible on the motherboard;
2. the CAL61/MEMCAL connection to U11 pin 28 labelled `MAP`; and
3. the executable firmware's principal load value at RAM `$0063`.

The schematic proves electrical endpoints. The listing proves processor
behavior. Neither source documents U11's internal transfer function, so this
audit does not assume that CAL61 scaling, U11 `MAP`, U10 MAP A/D channels, and
firmware load are the same signal.

## Hardware paths

### MAP and MAP2 sensor conditioning

The input schematic shows two separately conditioned analog paths:

| External net | Visible conditioning | Internal net | U10 A/D input |
| --- | --- | --- | ---: |
| `MAP#` | 18 kOhm series, 51 kOhm to ground, 0.47 uF capacitor | `MAP` | AN3, pin 4 |
| `MAP2#` | 18 kOhm series, 51 kOhm to ground, 0.47 uF capacitor | `MAP2` | AN0, pin 1 |

U10 (`16034988`) is the serial A/D device connected to the CPU-side SPI nets.
The two analog channels are therefore directly visible processor-conversion
resources. Their normal-operation use is a separate firmware question.

### CAL61 and U11

The physically established carrier mapping places 16055376 terminal 7 at
CAL61. The photo-authoritative functional network makes CAL61 common with J4
64. The motherboard connects CAL61 and the conditioned `MAP` net to U11 pin 28.

This directly proves that CAL61 participates in a MAP-related U11 boundary. It
does not prove that the resistor network scales the U10 AN3 measurement, that
U11 outputs a processor-visible MAP value, or that U11's MAP function is used
to calculate normal firmware load.

CAL59, 16055376 terminal 6, is tied through 100 Ohm to `VIGN` with a shunt
capacitor. The reconstructed network connects CAL59 through 24 kOhm to the
CAL55/CAL57 common node at U11 pins 4 and 3. This is a direct supply/bias anchor
for U11 configuration, but no MAP consequence is assigned.

## What the firmware actually samples

The normal-operation calls to A/D routine `$F1BE` use these explicitly loaded
channel selectors:

| Selector | Schematic/function used by firmware | Representative listing use |
| ---: | --- | --- |
| `$10` | battery | `$C88E`, `$DE6D` |
| `$20` | O2 | `$D9AD` |
| `$40` | coolant | `$F3B9` |
| `$50` | TPS | `$C999`, `$CC24` |
| `$60` | pump voltage | `$E816` |
| `$70` | diagnostic/start-run input | `$C8A5`, `$C92F`, `$EA2C` |
| `$80` | MAT | `$EBB3` |
| `$A0` | MAF | `$EB05`, `$F7AC` |

No normal-operation call in the supplied listing explicitly selects channel
`$00` (U10 AN0/MAP2) or `$30` (U10 AN3/MAP). Factory-test code
`$FDB5-$FDC5` does scan channels 0 through B and stores the results at
`$017B-$0186`; this proves the channels can be converted and observed during
factory test, not that they drive the normal load algorithm.

## Executable load producer

The principal load path in this 9340 calibration is MAF- and reference-period
based:

1. Analog MAF is selected by `LC014` bit 2. `$F7AC-$F7B6` reads A/D channel
   `$A0`, stores it at `$00ED`, multiplies it by seven, and stores the
   intermediate at `$00EF`.
2. Subsequent airflow processing produces the airflow word at `$00EA-$00EB`.
3. `$D769-$D7A0` multiplies `$00EA-$00EB` by reference period `$0095-$0096`,
   applies calibration `LC69A`, doubles/rounds/saturates the result, and writes
   the new load byte to `$0063`.
4. Before the new value is stored, the old `$0063` value moves to `$0061` and
   the prior `$0061` value moves to `$0062`.

Therefore `$0061` and `$0062` are executable load-history bytes in this path.
The listing/ALDL comment that calls `$0061` “Raw Ld Val” is not supported by
the instructions and must not override them. `$0063` is the current produced
load value; “filtered load” or historical `LV8` are retained as convenient
labels, not claims that a MAP sensor directly produced it.

## Downstream behavior

The `$0063` load value is a major shared firmware input. Direct listing uses
include:

| Consumer | Representative address range | Observable use |
| --- | --- | --- |
| Dwell/spark | `$CEB3` onward, `$CFA7-$D0xx` | dynamic dwell and spark-table/load terms |
| Highway spark | `$CFF4-$D0xx` | load threshold and load-indexed advance |
| Startup/cranking fuel | `$D769-$D86B` | load production precedes cranking/run fuel paths |
| Acceleration enrichment | `$D976-$DAxx` | change in load and load comparisons |
| Base fuel | `$DD6C` onward | load as fuel-table/flow input |
| EGR/AIR/purge | `$DF0A`, `$E387-$E40F`, `$E7CA` | load-indexed or load-qualified control |
| MAF diagnostics | `$E5EB-$E6A1` | load qualifies low/high MAF diagnostics |
| Knock-failure test | `$EB72-$EB8F` | load threshold qualifies Error-43 test state |

These consumers prove the importance of the MAF/reference-derived load value.
They do not establish a hidden MAP substitution path.

## Terminology correction: MAF versus MAP

At `$CC3A-$CC63`, the instructions test pump voltage, airflow `$00B6`, and
MAF diagnostic thresholds before updating `$00B0`. At `$CDA5`, nearby comments
explicitly enter “MAF (Hot Wire) Routines.” Some individual source comments
call `$00B0` a “MAP Hi” counter, while others call it “MAF Hi.” The executable
inputs and Error 33/34 context support **MAF diagnostic counter** as the stronger
description. The isolated `MAP` wording is retained as a source-comment
discrepancy, not promoted to behavior.

## C and regression correspondence

| Executable behavior | C implementation | Regression evidence |
| --- | --- | --- |
| Analog MAF acquisition and airflow | `src/fuel_air.inc.h` | MAF and air regressions |
| MAF/reference-period load producer | `src/fuel_air.inc.h` | load-producer, scheduler and fuel/spark regressions |
| Load history `$0061/$0062` | `src/fuel_air.inc.h` | Step-95 load-producer regression |
| Load consumers | `src/ecm_core.inc.h`, fuel, air, EGR, purge and diagnostic modules | corresponding subsystem regressions |
| Factory A/D scan | `src/factory_test_execution.inc.h` | factory-test execution regression |

No C or behavioral-baseline change is required. The implementation already
models the listing-backed MAF/load path and keeps physical A/D transfer curves
and U11 behavior at the HAL/custom-device boundary.

## Evidence conclusions

- **Direct hardware evidence:** MAP and MAP2 reach U10 AN3/AN0; conditioned
  MAP and CAL61 meet at U11 pin 28; CAL59 provides a VIGN-related bias path.
- **Direct firmware evidence:** normal load is produced from MAF-derived
  airflow and reference period; normal code does not explicitly request A/D
  channels 0 or 3; factory test scans both.
- **Strong inference:** CAL61 configures some U11 MAP-related behavior.
- **Unresolved:** U11's use of MAP/CAL61, any relationship to injection
  limiting, why MAP2 exists, and whether another MEMCAL/firmware variant uses
  the A/D MAP channels in normal operation.

