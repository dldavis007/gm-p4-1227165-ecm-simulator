# Ignition and injection signal chain

## Purpose and boundary

This document follows the 1227165 ignition and injection paths across the
motherboard schematic, U11 (`16054995`), U12 (`16034984`), bus-connected U9
(`16045148`), and the executable firmware. It describes visible pins, nets,
register accesses, and translated behavior. It does not invent the internal
logic, transfer functions, waveform generation, or pin-to-register encoding of
U9, U11, or U12.

Evidence terms follow the project rules:

- **Direct hardware evidence**: a visible schematic pin/net connection.
- **Direct firmware evidence**: emitted instructions or bytes in
  `evidence/firmware/bua-hac.lst`.
- **Strong inference**: separately proven endpoints form a coherent system
  path, but an undocumented custom device hides the internal link.
- **Unresolved boundary**: the evidence does not establish the transformation.

## Device roles visible at their pins

| Device | Directly visible ignition/injection connections | Supported statement | Unresolved statement |
| --- | --- | --- | --- |
| U9 `16045148` | Bus/address/control; inputs `INJREF` pin 38, `IGNREF` pin 39, `ESTLOOP` pin 41 and `KNOCK` pin 42; outputs `IGN` pin 7, `INJS` pin 8 and `INJA` pin 9 | U9 is the processor-bus-connected custom ignition/injection peripheral associated with the `$3FC0-$3FFF` firmware window | Exact pin-to-register mapping and internal scheduling logic |
| U12 `16034984`, reference/control section | `REF` pin 29, `CYL` pin 11, `IGN` pin 23, `EST` pin 24, `BYPASS` pin 21, `INJLOOP` pin 52; outputs `INJREF` pin 18, `IGNREF` pin 19, `IRQ` pin 32, `PUMP` pin 10 and `ESTLOOP` pin 25 | U12 conditions or participates in reference, EST-loop, cylinder-selection and related control paths | Pulse decoding, cylinder encoding, phase rules, thresholds and timing |
| U12 `16034984`, injector-driver section | Inputs `INJS` pin 47, `INJA` pin 46 and `~LIMP` pin 20; `ESENSE`, `ISENSE+`, `ISENSE-`; output `INJOUT` pin 2 to Q1 | U12 participates directly in injector actuation and current-sense/limiting circuitry | Current-control algorithm, polarities and the meaning of `INJS` versus `INJA` |
| U11 `16054995` | `INJREF` pin 5, `IGNREF` pin 6, `TPS` pin 20, `CTS` pin 23, `MAP` pin 28, multiple CAL pins; output `INJ` pin 19 to `INJLIMP` | U11 combines reference, sensor and fixed MEMCAL connections at a custom-device boundary | Its internal injection computation, limiting rule, oscillator behavior and transfer functions |

## Reference-input path

The ignition/injection schematic shows the external `ESTHI` and `ESTLO` paths
feeding a visible analog conditioning stage at U12 pins 26-28. Its output
passes through 27 kOhm and filtering to U12 pin 29 `REF`. U12 outputs
`INJREF` and `IGNREF`; both feed U9 and U11:

| Net | U12 source | U9 destination | U11 destination |
| --- | ---: | ---: | ---: |
| `INJREF` | pin 18 | pin 38 | pin 5 |
| `IGNREF` | pin 19 | pin 39 | pin 6 |

The hardware endpoints are direct evidence. It is a strong inference that U12
turns conditioned distributor activity into the two reference signals consumed
by U9/U11. Their exact pulse relationship, phase, polarity and separation are
not documented by the schematic.

The firmware provides the processor-visible half of the path:

- `$CAC6-$CAD3` reads status word `$3FFA`; high-byte bit 3 indicates that a
  distributor reference pulse occurred during the IRQ interval and sets the
  firmware reference flag.
- `$CB5A-$CB5D` copies `$3FC0`, labelled “last 2 ref periods,” to RAM `$0095`.
- `$CDE6-$CE41` normalizes that period according to `LC009` and derives RPM.
- `$3FC8` is read as the spark/reference-period counter in startup and normal
  spark processing.

This proves firmware consumption of U9-side reference status and periods. It
does not prove that any one U9 input pin maps one-to-one to a particular
register or status bit.

## Spark/EST command and feedback path

U9 directly exposes output `IGN` and input `ESTLOOP`. U12 directly receives
`IGN`, `EST`, `BYPASS`, and the conditioned `REF` path and outputs `ESTLOOP`.
The schematic therefore proves a closed hardware path around U9 and U12, but
does not expose the internal waveform or bypass truth table.

The executable firmware stages spark timing in the U9-associated register
window:

| Address | Direct firmware use | Evidence status |
| --- | --- | --- |
| `$3FC8` | current spark/reference period | Direct firmware evidence |
| `$3FDC` | dwell-period value | Direct firmware evidence |
| `$3FE4` | next-dwell start, seeded from `$3FEC` | Direct firmware evidence |
| `$3FE6` | dwell delta/update | Direct firmware evidence |
| `$3FE8` | current fire/fall delta | Direct firmware evidence |
| `$3FEC` | counter value at last reference | Direct firmware evidence |
| `$3FF6` | reference-to-fire offset | Direct firmware evidence |
| `$3FFC` | control/status word manipulated for EST enable/bypass and other modes | Direct firmware evidence; individual undocumented bits remain bounded |

At `$D20C-$D267`, firmware converts desired spark angle to time, limits its
rate and range relative to the reference period, and updates `$3FE8`, `$3FE6`,
`$3FDC`, and `$3FF6`. At `$CE5D-$CEB0` and `$D28D-$D301`, it enters/exits
low-RPM or bypass states, seeds dwell state, and changes `$3FFC`. These are
software-exact operations; conversion of those values into `IGN`/EST waveforms
inside U9/U12 remains a hardware boundary.

Error 42 is a feedback-consistency mechanism, not proof of a particular analog
circuit. Firmware counts spark feedback in `$00B4`, evaluates it around first
reference and EST/bypass transitions at `$D271-$D2D7`, and qualifies/latches
the diagnostic at `$E6AC-$E6DC`. The U12 `ESTLOOP` to U9 connection is a strong
hardware correlate, but its precise correspondence to `$00B4` is not directly
documented.

## Injection command and service path

The schematic shows two related hardware paths:

1. U9 outputs `INJS` and `INJA` to the U12 injector-driver section. U12 drives
   `INJOUT` into Q1 and receives current-sense signals.
2. U11 receives `INJREF`, `IGNREF`, sensor inputs and MEMCAL configuration and
   outputs `INJ` to the `INJLIMP` net. U12 also receives `INJLOOP` and `~LIMP`.

These connections support an injection-command/current-limiting subsystem, but
they do not establish the roles of `INJS` versus `INJA`, U11's algorithm, or
the polarity and timing of the driver signals.

The executable processor-side behavior is clearer:

- `$F67B-$F768` runs common injector bookkeeping only when `$00A0` bit 6 is
  present in the status sampled from `$3FFA`; this event bit is independent of
  the distributor-reference bit 3.
- `$F682-$F68B` compares `$002F & $18` with `LC225` and sets the Error-41
  precursor on a cylinder/fuel-mode mismatch.
- `$F9D2-$F9E4` limits the computed synchronous pulse width and writes it to
  `$3FD0`.
- Factory-test code `$FE4F-$FE9F` independently writes `$3FD0`, `$3FDC`,
  `$3FF6`, and nonzero `$3FCE`, confirming that `$3FCE` is an EFI delay
  register even though normal operation leaves it zero.

The status-gated service and synchronous pulse-width write are direct firmware
evidence. The established simulation cadence of one all-eight service per
crank revolution for this 9340 calibration is a system model supported by the
reference geometry and single common injector driver. The U9-internal
double-fire/bank/phase implementation remains unresolved.

## Cylinder and knock inputs at the same boundary

CAL56 reaches U12 pin 11 `CYL`; CAL32 reaches U12 pin 15 `KNOCK`. U12's
`KNOCK` output reaches U9 pin 42. These paths affect the same custom-device
boundary as reference and injection handling, but Step 131 does not assign
their undocumented encodings. See `MEMCAL_MOTHERBOARD_FIRMWARE_PATHS.md` for
the Error-41 and Error-43 cross-reference.

## C and regression correspondence

| Listing-backed behavior | C location | Regression evidence |
| --- | --- | --- |
| Reference status, period and RPM | `src/scheduler_serial.inc.h`, `src/ecm_core.inc.h` | `tests/spark_reference_regression.inc.h`, scheduler and startup-fuel regressions |
| Spark angle-to-time and U9-window writes | `src/ecm_core.inc.h` | `tests/fuel_spark_regression.inc.h` |
| EST enable/bypass, feedback and low-RPM handling | `src/ecm_core.inc.h`, Mode-4 lifecycle | fuel/spark and Mode-4 lifecycle regressions |
| Injector status service and bookkeeping | `src/injector_service.inc.h` | scheduler and startup-fuel regressions |
| Synchronous pulse-width output | `src/ecm_core.inc.h` and injector service integration | fuel/spark and startup-fuel regressions |
| Factory raw register exercise | `src/factory_test_execution.inc.h` | factory-test execution regression |

No C or baseline change is justified by this audit. The current implementation
already keeps physical waveform production, U9/U11/U12 internals, driver
polarity, and electrical current limiting outside the translated firmware.

## Remaining questions

- Exact U9 input/output pin-to-register and pin-to-status-bit mapping.
- U12's transformation of `REF` into `INJREF` and `IGNREF`.
- Cylinder encoding at CAL56/U12 and provenance of `$002F` bits 3-4.
- Meaning and polarity of `INJS`, `INJA`, `INJLIMP`, `INJLOOP`, and `~LIMP`.
- Electrical relationship between U12 `ESTLOOP` and firmware feedback counter
  `$00B4`.
- U11's use of reference, sensor, oscillator, and MEMCAL inputs.

