# 1227165 ECM technical-reference foundation

This directory is the entry point for assembling a traceable technical
reference for the GM P4 ECM service number 1227165 and the supplied 9340 PROM
material. It links the executable assembly, assembled listing, calibration and
MEMCAL evidence, ECM schematics, behavioral C port, and regression evidence
without treating interpretation as fact.

`BUA` is retained as source and photographed label text. It is not used by
itself to prove that every artifact belongs to one calibration or MEMCAL
variant.

## Evidence order

When sources disagree, use this order:

1. Emitted instructions, addresses, and bytes in the verified `bua-hac.lst`.
2. Calibration tables and their executable address usage.
3. Independent cross-checks, measurements, and repeatable tests.
4. Source comments and labels.
5. Secondary descriptions and forum information.

Translate behavior first and assign semantics second. Unknown hardware and
damaged source remain explicitly unresolved.

`bua-hac.txt` is excluded from firmware evidence because its corrected-revision
status is not established. Do not use it to fill, reinterpret, or override the
assembled listing.

## Reference documents

- [Evidence register](EVIDENCE_REGISTER.md) records sources, confidence terms,
  confirmed facts, variants, and open questions.
- [Hardware/firmware cross-reference](HARDWARE_FIRMWARE_CROSS_REFERENCE.md)
  connects functions to assembly, RAM or registers, calibration, schematic
  nets, C modules, and tests.
- [MEMCAL architecture](MEMCAL_ARCHITECTURE.md) records the established physical
  carrier/J4 mapping while separating it from unresolved electrical behavior.
- [MEMCAL motherboard/firmware paths](MEMCAL_MOTHERBOARD_FIRMWARE_PATHS.md)
  traces selected CAL connections through U11/U12 and into listing-backed
  behavior while preserving custom-device boundaries.
- [Ignition/injection signal chain](IGNITION_INJECTION_SIGNAL_CHAIN.md) traces
  reference, EST/feedback, and injector paths across U11/U12/U9 and the
  executable `$3FC0-$3FFF` firmware interface.
- [U11/U12 functional boundaries](U11_U12_FUNCTIONAL_BOUNDARIES.md) consolidates
  the known external roles of both custom devices while keeping undocumented
  transfer functions, thresholds, phase, polarity and current control explicit.
- [U9 register-window map](U9_REGISTER_WINDOW_MAP.md) records the established
  processor-visible semantics in `$3FC0-$3FFF` while separating them from
  undocumented U9 pin-to-register mappings.
- [U9 register-window closure](U9_REGISTER_WINDOW_CLOSURE.md) defines the
  evidence-supported address set and keeps every remaining location explicitly
  unclassified rather than guessing reserved or unused behavior.
- [MAP and firmware-load signal chain](MAP_LOAD_SIGNAL_CHAIN.md) separates
  physical MAP/MAP2 and CAL61/U11 paths from the executable
  MAF/reference-period load producer.
- [U10 A/D and sensor acquisition](ADC_SENSOR_ACQUISITION.md) maps every visible
  analog channel to normal firmware selectors, raw/processed state, and the
  factory-test sweep.
- [OSC, CYL, and reference timing](OSC_CYL_REFERENCE_TIMING_PATHS.md) separates
  CAL42/U11 oscillator configuration, CAL56/U12 cylinder selection, and the
  actual distributor-reference path into U9/U11 and firmware.
- [ESC and knock signal chain](ESC_KNOCK_SIGNAL_CHAIN.md) keeps CAL29/U10 AN9
  separate from CAL32/U12/U9 event counting and follows the listing-backed
  retard, recovery, spark, and Error-43 behavior.
- [Theory-of-operation index](THEORY_OF_OPERATION_INDEX.md) is the planned
  chapter structure for the eventual complete ECM description.

The historical `docs/STEPxxx_*.txt` files remain checkpoint records. They
should not be rewritten into a single latest-state narrative. New evidence
that changes an interpretation should be recorded here and in the applicable
new checkpoint while preserving the old audit.

## Maintenance workflow

For each new finding:

1. Record the source artifact and exact location or address.
2. Describe observable behavior before giving it a functional name.
3. Assign an evidence status defined in `EVIDENCE_REGISTER.md`.
4. Add or update the applicable cross-reference row.
5. Link the corresponding C implementation and regression when present.
6. If behavior changes, rerun the strict C89 build and all regressions and
   document the intentional baseline change.
