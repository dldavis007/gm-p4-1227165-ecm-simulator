# Current project status

## Behavioral-port milestone

Step 171 restores the qualified whole-image closure after Steps 168 and 169
translated the diagnostic and factory branches of the 160-baud LF880 manager
identified by Step 167. Step 170 exposes the completed manager through a
cell-level raw HAL.

No known reachable, internally defined executable area in the supplied PROM
image remains classified as unported. The complete Strict C89 workflow passes
without compiler warnings, and all frozen behavioral signatures remain
unchanged.

## What this establishes

- Listing-backed application behavior is represented at the behavioral level.
- Normal startup, scheduling, control, diagnostics, factory test, shutdown and
  retained restart are integrated through established software/HAL boundaries.
- The 8192-baud message core and complete 160-baud manager have explicit raw
  transport seams.
- Hardware-facing inputs and outputs are separated from the translated control
  algorithms through the raw HAL.
- Evidence status, variants and unresolved interpretations remain explicit.

## What this does not establish

- Byte-for-byte or cycle-exact emulation of the original processor.
- Electrical polarity, analog transfer, driver current, transceiver behavior,
  cable loading, collision handling or physical actuator response.
- Undocumented internal behavior of U9, U11, U12 or other custom devices.
- Behavior at vector destinations `$6000` or optional HUD window `$5800`, whose
  code is absent from the supplied PROM image.
- Exact calibration identity from the `BUA` label alone.
- Physical verification of every MEMCAL network relationship; the optional
  16055375 terminal-resistance matrix remains a refinement, not a closure
  blocker.

## Authority rules

1. Emitted instructions, addresses and bytes in `bua-hac.lst`.
2. Calibration tables and executable address use.
3. Independent cross-checks, measurements and repeatable tests.
4. Source comments and labels.
5. Secondary descriptions and forum material.

Translate observable behavior before assigning semantics. Do not fill an
unknown hardware or absent-ROM boundary with an assumption.

## Legitimate next work

Behavioral changes require an exact listing-backed discrepancy or new primary
evidence. Otherwise, useful work belongs to one of these separate tracks:

- documentation and usability;
- controlled physical measurement and evidence import;
- target-specific embedded/HIL implementation at the existing HAL seams; or
- new PC scenarios that remain explicitly simulator-only and do not redefine
  firmware behavior.

For practical use of the existing PC model, see the
[simulator, build, and testing guide](SIMULATOR_BUILD_TEST_GUIDE.md).
For the integrated engineering description, see the
[formal theory of operation](../THEORY_OF_OPERATION.md).
For a separately scoped target port, see the
[embedded and HIL implementation roadmap](EMBEDDED_HIL_IMPLEMENTATION_ROADMAP.md).

See [Step 171](STEP171_WHOLE_IMAGE_RECLOSURE_AUDIT.txt), the
[evidence register](reference/EVIDENCE_REGISTER.md), and the
[embedded-migration chapter](reference/C_PORT_ARCHITECTURE_EMBEDDED_MIGRATION.md)
for the detailed basis and boundaries.
