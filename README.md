# BUA / GM 1227165 Step-135 hardware/MEMCAL signal-chain audit

This retains the modular frozen Step-104 C89 PC harness, independent Step-105
transmission-aware driving scenario, and Step-106 listing correction.  Step
107 begins the listing-backed translation of Major Segment D diagnostics,
Step 108 translates its complete `LE4F7..LE75C` qualification chain, and Step
109 completes the field-service and flash-code routes and scheduler wiring.
Step 110 audits the remaining executable image and completes the listing-exact
Segment-1 MPU/I/O output staging at `$EDA3..$EF03`. Step 111 translates and
scheduler-wires the key-off front at `$D6D1..$D769`, the LF447 block-learn
commit, the IAC reset/park state machine at `$D370..$D3DB`, and a thin HAL
event at the software-powerdown endpoint.
Step 112 adds a listing-only, source-ordered model of `$C800..$C9F3`, including
the normal scheduler handoff and explicit boundaries for factory test, the
optional HUD ROM, SWI wait, MPU peripherals, and 8192-baud service. It does
not use `bua-hac.txt` as firmware evidence.
Step 113 translates the exact LF3A7 retained-error checksum and LF434 BLM/SAM
initialization, corrects the live LF434 default from historical decimal 120 to
the emitted `$80` (128), and preserves the older value only inside explicit
Step-104/105/109 signature replays.
Step 114 integrates the listing-ordered normal startup initializer at
`$C93B..$C9F3` through a distinct power-on entry point. It reuses established
coolant, TPS and Segment-E behavior, preserves the direct SCI/MPU/timer writes,
and leaves the unresolved SCI device-control-block walk as a named boundary.
Step 115 resolves that device-control-block initialization and translates the
listing's sole device `$80` receive, validation, checksum, Modes 0–4, response,
and transmit-state core. Physical byte timing and unrepresented ROM reads stay
behind explicit PC/HAL boundaries.
Step 116 connects completed SCI messages to the source-ordered `$CB5F..$CBB4`
Mode-4 scheduler lifecycle and `$CBCA..$CBD9` communication timeout. Mode-4
activation now occurs after reception, and entry-only error, IAC and BLM reset
commands execute exactly once.
Step 117 integrates the factory-test boot and IRQ control path at
`$C8B2..$C8DF`, `$CA6F..$CA73`, and `$FC72..$FD28`. It preserves the emitted
factory data-pointer table, optional `$AA` RAM fill, FMD exchange boundary,
96-count minor loop, serial cadence selection, battery/ignition decisions, and
literal COP restart. Custom-control and physical output exercises beginning at
`$FD29` remain a separate hardware-facing step.
Step 118 completes the listing-backed factory-test execution loop at
`$FD29..$FEA3`. It preserves the raw numeric mode-bit branches, nonvolatile
RAM checksum, twelve-channel A/D capture, alternating coolant pull-up samples,
six-channel PWM exercise, lamp/fan and IAC bit sequences, and the three exact
reference-period fuel/spark branches. Electrical effects and the final wait
loop remain HAL boundaries.
Step 119 preserves the eight emitted vector words at `$FFF0..$FFFE`, connects
the `$C9F4` slot to ordinary/factory IRQ dispatch, confirms `$F27B` is an
immediate `RTI`, and represents the four `$C800` destinations as reset/startup
requests. Both `$6000` targets and the processor-specific SWI-to-slot mapping
remain explicit boundaries rather than comment-derived assumptions.
Step 120 composes the previously audited startup pieces into one live,
source-ordered `$C800..$C9F3` power-on dispatcher. It preserves retained RAM
until validation, clears the exact volatile ranges, selects factory test
before retained recovery, records the absent HUD ROM and `$C938` SWI as
boundaries, and hands normal boots to the Step-114 initializer. ROM checksum,
socket-check result, and hardware samples remain explicit host/HAL inputs.
Step 121 connects reset-vector power-on, normal IRQ operation, ignition-off
shutdown, BLM commit, IAC homing, the software-powerdown boundary, and a later
retained restart in one end-to-end PC/HAL lifecycle. It also proves corrupted
retained recovery and factory-path isolation. Physical power switching and
processor reset consequences remain explicitly acknowledged HAL boundaries.
Step 122 re-audits the complete assembled image after the Step-111 through
Step-121 integrations. It finds no known reachable, internally defined
firmware area still classified as unported. Remaining limitations are
calibration/data, external or absent ROM, processor/custom-device behavior,
and physical electrical/timing boundaries. This is a coverage conclusion,
not a claim of instruction-by-instruction or whole-machine equivalence.
Step 123 inventories the historical Library/Drive evidence before cleanup. It
confirms the committed assembled listing, identifies the six ECM schematic
sheets and MEMCAL records awaiting controlled import, classifies duplicate
candidates and historical Step packages, and records the missing original
PROM/S-record comparison artifact. It changes no behavioral code and authorizes
no Library deletion.
Step 124 imports one hash-verified copy of each of the six ECM schematic sheets,
the MEMCAL photograph, connection notes, and resistor notes. It proves the
numbered schematic and photograph copies byte-identical, preserves all variant
and provenance limitations, and still authorizes no historical-file deletion.
Step 125 audits seven unique historical Step-104/105/109/110/111 packages,
proves the two Step-111 ZIPs byte-identical, and records how their source,
output, warning, audit, and signature content relates to the active repository.
The redundant packages are intentionally not committed. It changes no
behavioral code and performs no historical-file deletion.
Step 126 closes the MEMCAL/source-provenance investigation supportable from the
retained evidence. It clarifies the 66-contact MEMCAL carrier, documents the
14-terminal 16055375 and 16-terminal 16055376 network reconstructions and their
zero-ohm routing-jumper scheme, adds historical package-geometry corroboration,
and verifies the corrected `bua-hac.asm -> bua-hac.lst / bua-hac.s19 /
bua-hac.bin` build relationship. The authoritative listing remains the primary
repository byte/address evidence. Physical carrier continuity and installed-
resistor checks remain explicitly deferred hardware measurements rather than
Step-126 blockers.
Step 127 classifies the remaining hardware-facing simulator interfaces by
fidelity: listing-exact firmware/raw-register behavior, schematic-connected
hardware, processor-visible HAL signals, simulator-only plant/transfer
assumptions, and external/unknown device boundaries. It confirms that no new
translated-core behavior is required by the current evidence and prioritizes
explicit HAL setters/observers as the safest next implementation work.
Step 128 adds that explicit HAL seam around already-established raw inputs and
outputs. It introduces named setters/observers for O2, battery, VSS, reference
RPM, bounded RAM, MPU, parallel-I/O and related raw state without adding new
electrical semantics, vehicle-plant behavior, or custom-device assumptions.
Step 129 converts the reverse-engineered MEMCAL resistor networks from generic
grid layouts into functional terminal-level networks, closes the physical
66-pin carrier-to-J4 mapping, and establishes photograph-first authority for
as-built resistor and zero-ohm-jumper population. It also closes the 16055376
24-kOhm discrepancy in favor of the photographed hardware and LTspice model.
Step 130 carries those MEMCAL mappings through the motherboard and into
listing-backed firmware behavior. It establishes evidence-linked CAL42/OSC,
CAL56/CYL, CAL61/MAP, CAL59/VIGN and ESC/KNOCK paths while keeping U11/U12
internal transfer functions and resistor-only semantics explicitly unresolved.
Step 131 traces the visible ignition and injection chain around U9, U11 and
U12. It attributes the `$3FC0-$3FFF` window to U9 at the device level, closes
the supported processor-visible reference/RPM, spark/EST and injector-command
paths, and leaves exact pin-to-register mapping, polarity, phase and current
control as custom-device boundaries.
Step 132 separates the physical MAP/MAP2 and CAL61/U11 paths from the supplied
image's actual MAF/reference-period load producer. It establishes `$0063` as a
MAF-derived load quantity for this image and preserves U11 MAP/CAL61 behavior
and variant-dependent MAP use as unresolved.
Step 133 maps the visible U10 A/D channels to schematic signals, normal
firmware selectors, factory-test capture, raw RAM state and processed behavior.
It distinguishes MAP2, VOLT, O2, MAP, CTS, TPS, PUMPVOLT, DIAG, MAT, ESC and
VMAF channels and records which selectors the normal image actually requests.
Step 134 separates CAL42/U11 OSC, CAL56/U12 CYL and the distributor-reference
path. It cross-references reference occurrence, period and RPM processing while
preserving U11/U12/U9 internal behavior and CAL56 voltage encoding as explicit
unknowns.
Step 135 separates the CAL29/U10 analog ESC monitor from the CAL32/U12/U9
knock-event path and traces the executable knock-retard, recovery, spark
subtraction and Error-43 behavior. The U9 KNOCK-pin-to-`$3FCA` relationship is
kept as a strong inference rather than promoted to undocumented direct proof.

The project targets the 1986 Corvette L98 GM P4 ECM, service number 1227165, using the
supplied 9340 / 16059335 PROM material.  `BUA` remains a source label, not an
independently proven calibration identity.

## Build

The project deliberately builds as one translation unit:

```text
gcc -std=c89 -Wall -Wextra -pedantic main.c -o build/bua_step121
```

The included implementation fragments must not be compiled separately.
`main.c` includes them in the frozen declaration order so all existing
file-local (`static`) linkage and arithmetic behavior remain unchanged.

- Windows compile: run `build_gcc.bat`
- Windows compile and run: run `build_run_gcc.bat`
- Linux/macOS compile and run: run `sh build_gcc.sh`
- Make: run `make test`

For OnlineGDB, import the complete project tree and compile only `main.c`.
Every source fragment is well below the previously observed approximately
11,000-line editor/save limit.  `main.c` itself contains `int main(void)`.

## Directory layout

| Path | Purpose |
| --- | --- |
| `main.c` | Readable include map and program entry point |
| `src/` | Translated ECM algorithms, scheduler, RAM/MPU model, and feature wiring |
| `simulation/` | Original scripted drive plus the PC-only transmission-aware plant |
| `tests/` | Regression models and regression functions |
| `docs/` | Frozen evidence and numbered step audits |
| `docs/reference/` | Living evidence register, hardware/firmware cross-reference, MEMCAL model, and theory-of-operation index |
| `evidence/firmware/` | Authoritative assembled listing and corrected-source/build provenance record |
| `evidence/hardware/` | Hash-identified ECM schematic evidence with provenance and interpretation limits |
| `evidence/memcal/` | Hash-identified MEMCAL photograph, connection notes, resistor evidence, and reconstruction records |
| `build/` | Generated executables, output, and compiler diagnostics |

Files ending in `.inc.h` are implementation fragments, not independent public
headers.  The naming makes that constraint explicit while preventing online
compilers from treating them as separate C source files.

## Behavioral baselines

- Frozen Step-104 behavioral signature: `4BA6B7C6`
- Step-105 transmission trace signature: `9732D09B`
- Step-106 listing-coolant regression: 12/12
- Step-107 diagnostic-stage regression: 17/17
- Step-108 diagnostic-qualification regression: 22/22
- Step-109 integrated Segment-D regression: 40/40
- Step-109 integrated Segment-D signature: `F357A5F2`
- Step-110 Segment-1 output regression: 22/22
- Step-110 integrated output-stage signature: `FAADF8A6`
- Step-111 ignition shutdown/IAC homing regression: 40/40
- Step-111 ignition lifecycle signature: `16D17C9C`
- Step-112 source-ordered reset/startup regression: 19/19
- Step-113 retained-memory/startup regression: 10/10
- Step-114 source-ordered normal-startup regression: 14/14
- Step-115 8192-baud SCI/message regression: 20/20
- Step-116 Mode-4 lifecycle/scheduler regression: 16/16
- Step-117 factory-test boot/control regression: 24/24
- Step-118 factory-test execution regression: 31/31
- Step-119 vector/exception-boundary regression: 19/19
- Step-120 unified power-on-dispatch regression: 24/24
- Step-121 end-to-end ignition-lifecycle integration: 18/18
- Step-158 normal-operation FMD raw HAL regression: 7/7
- Step-159 raw-HAL lifecycle integration regression: 8/8
- Strict C89 compile: no warnings
- Integrated drive regression: 26/26
- Step-104 freeze regression: 10/10
- Step-105 transmission-aware regression: 12/12

The original Step-104 scenario remains present and unchanged.  Step 105 runs
after a fresh reset and derives Drive RPM from vehicle speed, selected ratio,
converter slip, and physical TCC state.  Its shift points, slip, and combined
axle/tire factor are explicitly simulator parameters, not factory claims.

The listing proves that LF42A is an RTS at $F42A.  It also proves that the
assembled ERR14/15 fallback at $F418 branches to LF42A; the `COOLS8` spelling
in the supplied text is not an unresolved executable destination.

See `docs/STEP121_LIFECYCLE_INTEGRATION_AUDIT.txt` for the end-to-end lifecycle
integration audit and `docs/STEP122_WHOLE_IMAGE_CLOSURE_AUDIT.txt` for the
post-integration coverage conclusion. See
`docs/STEP123_LIBRARY_EVIDENCE_INVENTORY.txt` for the pre-cleanup archive gate.
See `docs/STEP124_CONTROLLED_EVIDENCE_IMPORT.txt` for the imported artifact
manifest and duplicate proof. See
`docs/STEP125_HISTORICAL_PACKAGE_AUDIT.txt` for the package-level
manifest and preservation decisions. See
`docs/STEP126_MEMCAL_AND_SOURCE_PROVENANCE_CLOSURE_AUDIT.txt` for the MEMCAL,
corrected-source, build-output, and historical-PROM provenance closure. See
`docs/STEP127_HARDWARE_HAL_FIDELITY_AUDIT.txt` for the hardware/HAL fidelity
classification. See `docs/STEP128_EXPLICIT_HAL_INTERFACE_AUDIT.txt` for the
explicit raw HAL boundary and `docs/STEP129_MEMCAL_FUNCTIONAL_NETWORK_AUDIT.txt`
for the functional MEMCAL reconstruction and physical carrier mapping. See
`docs/STEP130_MEMCAL_MOTHERBOARD_FIRMWARE_CROSS_REFERENCE_AUDIT.txt` through
`docs/STEP135_ESC_KNOCK_SIGNAL_CHAIN_AUDIT.txt` for the current motherboard,
custom-device, sensor-acquisition, timing and ESC/knock theory-of-operation
cross-reference work. See `docs/STEP158_NORMAL_FMD_RAW_HAL_AUDIT.txt` for the
normal-operation FMD/SPI raw-input boundary and
`docs/STEP159_RAW_HAL_LIFECYCLE_INTEGRATION_AUDIT.txt` for its integration with
the raw VOLT ignition path. Earlier audits remain checkpoints.

Normal simulator execution now runs Segment D once per 16 ordinary IRQs. The
PC-only `sim_legacy_segment_d_freeze` switch is enabled only inside the frozen
Step-104 and Step-105 replay scenarios, so their historical signatures remain
comparable. It is not source RAM and is not part of the target ECM behavior.

Normal simulator execution now also performs Segment 1's raw writes to MPU
addresses `$3FCC/$3FD2/$3FD4/$3FD6/$3FD8` and parallel-I/O `$4004`. Electrical
polarity and the physical output devices remain HAL responsibilities. The
PC-only Segment-1 replay switch is used only by frozen historical signatures.

Ordinary odd minor loops now run the literal ignition-on/key-off front before
the existing load/fuel chain. Ordinary even minor loops run the IAC homing
producer after reference/RPM/spark work, while the existing motor consumer
continues every 6.25 ms. At `$D6EA`, the PC HAL records one software-powerdown
event and stops subsequent IRQ dispatch; physical keep-alive power remains a
hardware integration responsibility.
