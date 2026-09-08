# CAL42/OSC, CAL56/CYL, and reference-timing paths

## Scope

This audit separates three adjacent but electrically distinct functions:

1. the CAL42 connection to U11 `OSC`;
2. the CAL56 connection to U12 `CYL`; and
3. the distributor-reference signal path through U12 to U9/U11 and firmware.

The separation matters because an oscillator resistor does not by itself prove
a processor clock, and a cylinder-selection input is not the reference pulse.
The custom-device interiors remain undocumented.

Evidence language follows the project convention:

- **Direct evidence** is a visible schematic connection or executable listing
  instruction/data byte.
- **Mechanically derived** is a CAL identity obtained from the established
  66-pin carrier-to-J4 interleave.
- **Strong inference** joins independently demonstrated endpoints across an
  undocumented custom device.
- **Unresolved** means the available evidence does not establish the behavior.

## Evidence-tiered path summary

| Path | MEMCAL evidence | Motherboard endpoint | Firmware evidence | Conclusion |
| --- | --- | --- | --- | --- |
| CAL42 / `OSC` | 16055375 terminal 10 maps mechanically to CAL42; reconstructed external-terminal equivalent shows 91 kOhm to CAL34 | CAL42 goes directly to U11 pin 18 `OSC` | No direct CPU read or identified U9 register consequence | Electrical/configuration endpoint direct; frequency, duty cycle, and affected U11 functions unresolved |
| CAL56 / `CYL` | 16055376 terminal 13 maps mechanically to CAL56; photographed reconstruction supports 7.5 kOhm to CAL53 | CAL56 goes directly to U12 pin 11 `CYL` | `$002F & $18` is compared with `LC225` at `$F682-$F68B`; mismatch feeds Error 41 | Static cylinder/fuel-mode consistency path strongly inferred; electrical encoding and `$002F` provenance unresolved |
| Distributor reference | No demonstrated dependency on CAL42 or CAL56 | conditioned `ESTHI/ESTLO` reaches U12 pin 29 `REF`; U12 pins 18/19 output `INJREF/IGNREF` to U9 and U11 | U9-window status/period values are consumed at `$CAC6-$CAD3`, `$CB5A-$CB5D`, and `$CDE6-$CE41` | Endpoints and firmware behavior direct; U12/U9 pulse transformation unresolved |

## CAL42 is a U11 oscillator boundary

The established carrier mapping gives this mechanically derived chain:

| Physical item | Identity |
| --- | --- |
| Absolute carrier pin 46 | J4/CAL42 |
| 16055375 terminal 10 | CAL42 |
| Motherboard destination | U11 pin 18, `OSC` |

The photographed 16055375 reconstruction is treated as the authority for the
as-built network. Its simplified external-terminal model places 91 kOhm
between CAL42 and CAL34. CAL34 is absolute carrier pin 50, J4/CAL34, and
16055375 terminal 14;
its motherboard destination remains unresolved.

This proves that the MEMCAL supplies an electrical parameter to the U11 pin
labelled `OSC`. It does not prove:

- the oscillator frequency or duty cycle;
- whether CAL34 is ground, supply, reference, or another custom-device node;
- which U11 output or internal function uses the oscillator; or
- any relationship to U9's firmware-visible counters.

In particular, the listing's documented 65.536-kHz reference-period arithmetic
and U9 register window must not be attributed to CAL42. No retained schematic
net joins CAL42/U11 `OSC` to U9, and no instruction reads CAL42.

## CAL56 is separate from the distributor-reference input

The established carrier mapping gives this mechanically derived chain:

| Physical item | Identity |
| --- | --- |
| Absolute carrier pin 39 | J4/CAL56 |
| 16055376 terminal 13 | CAL56 |
| Motherboard destination | U12 pin 11, `CYL` |

The photo-authoritative functional network connects CAL56 to CAL53 through
7.5 kOhm. CAL53 is absolute carrier pin 27, J4/CAL53, and 16055376 terminal 3;
its motherboard destination remains unresolved. Therefore even the static voltage presented
to `CYL` cannot yet be calculated from authoritative endpoints.

U12 pin 29 `REF` is a different pin and a different path. The schematic shows
conditioned `ESTHI` and `ESTLO` feeding that input. U12 then presents
`INJREF` and `IGNREF` at pins 18 and 19 to both U9 and U11. Thus CAL56 can
select or qualify a cylinder mode inside U12, but it must not be described as
the pulse source without internal U12 evidence.

## Firmware reference timing

The executable image consumes reference activity through the U9-associated
register window:

| Listing location | Direct behavior |
| --- | --- |
| `$CAC6-$CAD3` | Reads `$3FFA`; high-byte bit 3 marks a distributor reference during the preceding IRQ interval and sets the minor-loop reference flag |
| `$CB5A-$CB5D` | Copies `$3FC0`, commented as the last two reference periods, to RAM `$0095-$0096` |
| `$CDC1-$CDCA` | Detects excessive minor loops without a reference and writes `$FFFF` to `$3FC0` |
| `$CDE6-$CDF5` | Selects the eight-cylinder or alternate normalization path using PROM byte `LC009` |
| `$CDF6-$CE41` | Stores the normalized period and derives RPM state including `$0056/$0058` |
| `$CD2E-$CD47` | Uses reference occurrence and `$3FC8` changes in normal spark-period processing |

These operations establish processor-visible event and period semantics. They
do not establish which U9 input pin produces each status bit/register or the
counter implementation inside U9.

For this calibration `LC009=$00`, so `$CDE6-$CDF5` uses `$0095-$0096`
directly. The listing comments encode alternate cylinder selections in
`LC009`, but that PROM byte is software calibration data, not a measured
CAL56 voltage.

## Cylinder-mode consistency and Error 41

The other cylinder-related PROM byte is `LC225=$00`, documented in the listing
as eight-cylinder PFI fuel mode. During injector service:

1. `$F682` loads RAM `$002F`.
2. `$F684` retains bits `$18`.
3. `$F686-$F689` compares that result with `LC225`.
4. A mismatch sets `$0040` bit 0 at `$F68B`.
5. `$E6A1-$E6AA` latches Error 41 and clears the transient precursor.

The CAL56-to-U12 `CYL` endpoint and this executable comparison strongly support
an end-to-end cylinder/fuel-mode consistency function. They do not prove that
`$002F` bits 3-4 are a direct encoding of CAL56; U12 and U9/MPU behavior hides
that link. `LC009` and `LC225` are also distinct consumers and must not be
collapsed merely because both equal zero in the supplied eight-cylinder image.

## Supported theory of operation

The narrow supported theory is:

1. External distributor activity is conditioned and presented to U12 `REF`.
2. U12 supplies `INJREF` and `IGNREF` to U9 and U11.
3. U9 exposes reference occurrence and timing state consumed by firmware.
4. Firmware derives RPM and schedules spark/injection from that state.
5. CAL56 supplies a separate U12 cylinder-configuration input, and firmware
   checks a processor-visible fuel-mode field against PROM configuration.
6. CAL42 supplies a separate U11 oscillator-related configuration input whose
   downstream function is not documented.

Steps 1-4 and the visible connections in 5-6 are direct evidence. The causal
link across the custom devices in step 5 is strong inference. No causal link
from CAL42 to a particular timing register or U11 output is presently claimed.

## Simulator boundary and future evidence

The C model already represents reference pulses as HAL/custom-peripheral
stimulus and translates the listing-backed status, period, RPM, spark, injector,
and Error-41 behavior. Step 134 therefore requires no source or baseline change.

Useful future physical tests are:

- determine CAL34 and CAL53 motherboard destinations;
- measure CAL42 and CAL56 voltage/waveform behavior on a running ECM;
- compare CAL56 levels and `$002F & $18` across known cylinder variants;
- simultaneously observe U12 `REF`, `INJREF`, and `IGNREF` to establish phase,
  polarity, and pulse-count relationships; and
- identify U9 register semantics from independent documentation or controlled
  register-versus-pin testing.
