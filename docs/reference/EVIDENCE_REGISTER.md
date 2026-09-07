# Evidence register

## Status vocabulary

| Status | Meaning |
| --- | --- |
| Confirmed | Directly established by emitted instructions, exact address use, schematic connectivity, continuity measurement, or a repeatable test of the stated behavior. |
| Strongly supported | Independent evidence agrees, but one direct proof step remains. |
| Provisional | Best current interpretation; competing explanations remain possible. |
| Variant-dependent | May legitimately differ by ECM, MEMCAL, calibration, or board revision. |
| Unknown | Evidence is presently insufficient. |
| Boundary | Behavior belongs to hardware or code outside the supplied PROM image. |

Confidence applies to the specific claim, not to an entire source. For
example, source instructions can be authoritative while a nearby comment is
wrong.

## Source inventory

The authoritative assembled listing is stored under `evidence/firmware/`.
Other primary artifacts remain external until their provenance and applicable
variant are verified. The hashes below identify the material used for the
current audit and allow later imports to be verified.

| Artifact | Size | SHA-256 | Role |
| --- | ---: | --- | --- |
| `bua-hac.txt` | 429,349 bytes | `36f20ba8e64cce07d41c7c5eaf3d74f4507c852adf5e1cd239cd6a14fbaf23dd` | Excluded from firmware evidence: its corrected-revision status is not established. Do not use it. |
| [`evidence/firmware/bua-hac.lst`](../../evidence/firmware/bua-hac.lst) | 938,156 bytes | `5dd9df745532fae26ef6a19deb4402690e64bb8a254aeb33d10c14a1ad6dce83` | Sole primary firmware source: zero-error assembled listing used for emitted bytes, addresses, targets, tables, and execution order. |
| [`evidence/memcal/Resistors.txt`](../../evidence/memcal/Resistors.txt) | 219 bytes | `350fbc5a0e1888a86599cd451e4db1c574be500c745d4b365dacda70465ef4de` | Two measured/reconstructed MEMCAL resistor lists. |
| [`evidence/memcal/MemCal, Cal connections.docx`](../../evidence/memcal/MemCal,%20Cal%20connections.docx) | 7,121 bytes | `f9983509b3f7e742d889278f2d913aa4f97a3896d1a356eaa2c9af8192edf6a6` | User connection notes from CAL29 through CAL61. Contains at least one apparent duplicated-label transcription error. The user's original contact enumeration explicitly covered only contacts 1-33; contacts 34-66 were intended to be inferred from the corresponding positions of the 66-contact carrier, not omitted as nonexistent. |
| `Usefull Info.pdf` | four pages | external/user-supplied research notes | Historical research compilation. Corroborates 1986-1988 TPI use of 16055375/16055376, describes them as 7-pin-per-side and 8-pin-per-side networks, records BUA/9340/16059335/1227165 application information, and preserves external research leads. Treat tentative replacement-part claims as leads rather than direct proof. |
| [1227165 schematic GIF set](../../evidence/hardware/1227165-schematics/) | six sheets, 194,091 bytes total | See `docs/STEP124_CONTROLLED_EVIDENCE_IMPORT.txt`. | Processor, inputs, outputs, ignition/injection, connectors, and power supply. Imported with provenance/rights limitations explicit. |
| [`evidence/memcal/MemCal photo 86 Vette BUA.png`](../../evidence/memcal/MemCal%20photo%2086%20Vette%20BUA.png) | 255,584 bytes | `3010972285864033e73fcb63b9eec84f9f2e616e95cc5c0dccb5563aa57bfd25` | Physical-layout image; exact source-unit applicability remains variant-dependent. |
| User reverse-engineering MEMCAL photographs (overview and resistor-board close-up) | two JPEG photographs | repository import pending | User-supplied primary physical evidence of the MEMCAL installed in the ECM and the reverse-engineered resistor-board construction. Retain with project evidence. |
| [`evidence/memcal/MEMCAL_16055375_RECONSTRUCTION_RECORD.md`](../../evidence/memcal/MEMCAL_16055375_RECONSTRUCTION_RECORD.md) | text record | repository-controlled | Consolidates the 14-terminal 16055375 LTspice model, generic 16-position KiCad carrier, jumper interpretation, and pending physical continuity verification. |
| [`evidence/memcal/MEMCAL_16055376_RECONSTRUCTION_RECORD.md`](../../evidence/memcal/MEMCAL_16055376_RECONSTRUCTION_RECORD.md) | text record | repository-controlled | Consolidates the 16-terminal 16055376 LTspice/KiCad reconstruction and nominal-vs-fitted resistor values. |
| `GM-8192-160-Baud-ALDL-Interface.pdf` | approximately 595 kB | not yet recorded | External ALDL electrical and timing reference; useful for the HAL, not primary proof of PROM behavior. |

## Confirmed or strongly supported system facts

| ID | Status | Statement | Principal evidence |
| --- | --- | --- | --- |
| SYS-001 | Confirmed | The target ECM service number is 1227165. | Project hardware identification and schematic title blocks. |
| SYS-002 | Confirmed | The supplied program window is 32 KiB at `$8000-$FFFF`; the PROM interface exposes `A0-A14`, `D0-D7`, `/ROMCS`, and `/ROMOE`. | Connector/processor schematics and assembled image addresses. |
| SYS-003 | Confirmed | Ordinary IRQ cadence is 6.25 ms. Odd and even minor branches each execute at 12.5 ms, and all sixteen major segments are selected once per sixteen IRQs. | Listing-backed scheduler translation and regression. |
| SYS-004 | Confirmed | IAC motor service executes every 6.25 ms; the IAC control producer executes at 50 ms where established. | Executable call graph and scheduler regressions. |
| SYS-005 | Confirmed | `LC014 = $B4` has bit 1 clear, selecting normal double-fire behavior in the translated branch. | Calibration byte and executable test. |
| SYS-006 | Strongly supported | V8 geometry uses four distributor reference pulses per crankshaft revolution; one reference interval represents 90 crank degrees. | Executable arithmetic, calibration usage, and cross-checks. |
| SYS-007 | Confirmed | Normal physical batch-injection bookkeeping represents one simultaneous all-eight-injector batch per crankshaft revolution. | Executable scheduling and translated injector regression. |
| SYS-008 | Confirmed | The Step-104 normal-operation behavioral signature is `4BA6B7C6`. | Frozen serialization contract and regression. |
| SYS-009 | Confirmed | The Step-105 transmission-aware signature is `9732D09B`; its plant parameters are simulator assumptions, not factory claims. | Step-105 audit and regression. |
| SYS-010 | Confirmed | Step 111 exposes software powerdown as a HAL event and does not claim to model physical keep-alive power. | `$D6D1-$D769` translation and Step-111 regression. |
| SYS-011 | Confirmed | Step 112 models the source-ordered `$C800-$C9F3` startup decisions while leaving factory-test, optional-ROM, SWI, MPU electrical behavior, and 8192-baud internals at explicit boundaries. | Verified `bua-hac.lst` and Step-112 regression. |
| SYS-012 | Confirmed | LF3A7 computes `1 + L0005 + ... + L0009`; LF434 stores `$8000` in both SAM words and initializes all sixteen BLM cells to `$80` (128). | `bua-hac.lst` `$F3A7-$F3B4`, `$F434-$F446`; Step-113 regression. |
| SYS-013 | Confirmed | Step 115 translates the sole device `$80` SCI initialization, receive validation/checksum, Modes 0-4 response construction, and transmit-state core at `$C9F4` and `$FA58-$FC71`. | Verified `bua-hac.lst` and Step-115 regression. Physical byte timing and unavailable ROM reads remain boundaries. |
| SYS-014 | Confirmed | Step 116 defers Mode-4 activation until `$CB67-$CB72`, executes entry-only error/IAC/BLM reset commands, clears exit state at `$CBA4-$CBB1`, and forces received Mode 0 only when the one-second communication timer becomes greater than 30. | Verified `bua-hac.lst` and Step-116 regression. |
| SYS-015 | Confirmed | Step 117 integrates the factory boot and IRQ control path through `$FD28`: optional `$AA` fill, the 24-word diagnostic pointer table, FMD exchange boundary, 96-count loop, serial cadence selection, emitted battery/ignition branches, and COP restart. | Verified `bua-hac.lst` and Step-117 regression. Custom-control and physical output exercises from `$FD29` remain deferred. |
| SYS-016 | Confirmed | Step 118 completes the factory-test software loop at `$FD29-$FEA3`, preserving raw mode-bit branches, the 46-byte RAM checksum, A/D capture, PWM/lamp/fan/IAC writes, and reference-derived fuel/spark timer values. | Verified `bua-hac.lst` and Step-118 regression. Electrical consequences and the wait loop remain HAL boundaries. |
| SYS-017 | Confirmed | The eight emitted vector words at `$FFF0-$FFFE` target `$6000`, `$C9F4`, `$F27B`, `$6000`, and four copies of `$C800`; `$F27B` is an immediate `RTI`. | Verified `bua-hac.lst` and Step-119 regression. `$6000`, reset consequences, and the processor-specific SWI slot remain boundaries. |
| SYS-018 | Confirmed | The live Step-120 power-on path executes `$C800-$C9F3` in source order: exact volatile clears, socket result, factory selection, optional-ROM boundary, retained validation/recovery, Error 51/SWI decision, and normal initialization. | Verified `bua-hac.lst` and Step-120 regression. Socket/checksum samples and absent HUD ROM behavior remain explicit boundaries. |
| SYS-019 | Confirmed | Step 121 composes reset-vector power-on, normal IRQ execution, `$D6D1-$D769` ignition shutdown, LF447 BLM commit, IAC homing, the `$D6EA` software-powerdown boundary, and retained restart without bypassing their established source order. | Step-111, Step-119, Step-120 implementations and Step-121 end-to-end regression. Physical power/reset effects remain a HAL boundary. |
| SYS-020 | Strongly supported | After reconciliation of the Step-110 whole-image gap list against Steps 111-121, no known reachable and internally defined firmware region remains classified as unported. | `bua-hac.lst`, Step-122 closure audit, current module/call-graph inventory, and regressions through Step 121. This does not assert instruction-level or whole-machine equivalence. |

## MEMCAL findings

| ID | Status | Statement | Notes |
| --- | --- | --- | --- |
| MEM-001 | Confirmed | The motherboard schematic provides a 66-contact J4 MEMCAL interface, 33 contacts per side. | Connector schematic. The user's original reverse-engineering list enumerated only contacts 1-33 because the corresponding 34-66 side was intended to be extrapolated; it was not a claim that the MEMCAL has only 33 contacts. |
| MEM-002 | Confirmed | The MEMCAL supplies the external PROM address/data/control connections and CAL29-CAL61 hardware connections. | Connector and processor/ignition schematics. |
| MEM-003 | Strongly supported | The relevant MEMCAL architecture includes a 28-pin EPROM, a 14-terminal 16055375 resistor network, and a 16-terminal 16055376 resistor network on a pin-corresponding 66-contact carrier with 33 positions per side. | User physical inspection, reverse-engineering photographs, LTspice models, KiCad reconstructions, resistor records, and historical research notes describing the networks as seven and eight pins per side. Exact carrier-to-J4 continuity remains to be physically measured. |
| MEM-004 | Confirmed | Many CAL connections configure custom ignition/injection circuitry rather than mapping directly to processor-readable bits. | `MemCal, Cal connections.docx` cross-checked against the ignition/injection schematic. |
| MEM-005 | Confirmed | CAL56 reaches the custom device cylinder-selection input; CAL42 participates in an oscillator connection; CAL61 reaches the MAP-related custom-device input. | ECM schematic connectivity. Functional consequences beyond the visible connections require additional proof. |
| MEM-006 | Variant-dependent | Physical network pin count, part marking, placement, and resistor population can differ across MEMCAL models. | User warning and uncertain historical photograph provenance. |
| MEM-007 | Strongly supported | The 16055375 reconstruction is a 14-terminal electrical network implemented on a generic 16-position board; its zero-ohm parts are routing/configuration jumpers, with J8/J9 unused in the local package-position correspondence. | LTspice topology, KiCad PCB, resistance-analysis artifacts, user construction description, physical close-up, and historical seven-pin-per-side note. Final physical carrier continuity remains outstanding. |
| MEM-008 | Strongly supported | The 16055376 reconstruction is a 16-terminal network whose nominal LTspice resistor set matches the simple nine-value `Resistors.txt` group; the KiCad implementation uses configurable jumper/open positions and some slightly adjusted fitted values. | `NetRes 16055376.asc`, `NetRes_16055376.kicad_pcb`, `Resistors.txt`, reconstruction record, and historical eight-pin-per-side note. |
| MEM-009 | Strongly supported | The surviving SPICE/KiCad artifacts resolve the earlier ambiguity over which historical resistor set belongs to which network: the simple nine-value set corresponds to 16055376, while the more complex reconstruction corresponds to 16055375. | Independent project artifacts with matching pin counts, values, and topology. Direct measurement of every installed resistor remains a separate physical verification task. |
| MEM-010 | Strongly supported | Historical research notes independently associate the 16055375/16055376 pair with 1986-1988 5.0/5.7 TPI MEMCALs and record 16055376 as the network near the end of the MEMCAL. | `Usefull Info.pdf`; retained as corroboration rather than sole proof of variant identity. |

## Open evidence items

| ID | Status | Question or required evidence |
| --- | --- | --- |
| OPEN-001 | Unknown | Exact EPROM/network-pin-to-absolute-J4/CAL mapping on the verified 9340 MEMCAL. Required closure step: physically continuity-check every accessible EPROM, 16055375, and 16055376 pin against the 66-contact header and record the full table rather than relying on inferred 34-66 symmetry. |
| OPEN-002 | Strongly supported | Historical resistor-value ambiguity is largely resolved by the surviving 16055375/16055376 LTspice and KiCad artifacts. Remaining question: confirm that the values actually fitted to the physical reverse-engineered carrier agree with the intended reconstruction, especially where KiCad substitutions differ slightly from nominal SPICE values. |
| OPEN-003 | Variant-dependent | Whether the previously available MEMCAL photograph is the exact source unit or another model with similar construction. The newly supplied reverse-engineering photographs are user-identified as the actual MEMCAL being reverse engineered and therefore provide stronger physical-layout provenance. |
| OPEN-004 | Boundary | Internal behavior and undocumented register bits of the custom MPU/peripheral devices beyond what executable use and schematics expose. |
| OPEN-005 | Boundary | Optional heads-up-display ROM behavior in the `$5800` region, whose bytes are absent from the supplied PROM. |
| OPEN-006 | Boundary | Step 115 completes the listing-backed software message core. Exact electrical byte timing, transceiver behavior, and arbitrary ROM-dump bytes are not modeled by the C core. |
| OPEN-007 | Boundary | Step 120 composes the source-ordered software startup path; physical reset causes, socket checksum acquisition, and optional HUD ROM execution remain outside the supplied PROM/software model. |
| OPEN-008 | Unknown | The original assembled S-record, working-PROM image, and saved byte-comparison report supporting the historical near-perfect image-match claim were not located by the Step-123 targeted search. |

## Identity boundary

The text `BUA` appears in the supplied source labeling and on an available
MEMCAL photograph alongside `9340`. That is useful corroboration, but the
project does not use the label alone to claim calibration identity or to prove
that every photographed hardware variant contains the supplied PROM image.
