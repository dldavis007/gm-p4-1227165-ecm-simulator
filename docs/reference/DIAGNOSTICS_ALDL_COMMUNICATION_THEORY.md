# Diagnostics and ALDL communication theory

## Scope

This chapter integrates the evidence-supported diagnostic qualification, fault logging, service-lamp/flash-code behavior, normal 160-baud ALDL display stream, 8192-baud SCI message core, and Mode-4 lifecycle of the supplied 1227165 / 9340 firmware.

The assembled listing remains authoritative. The C translations and regressions are used as cross-checks of listing behavior; they do not replace the listing or silently fill hardware gaps.

The same fidelity vocabulary used throughout the reference applies here:

- **F0** — listing-backed software state, arithmetic, ordering, protocol rules and raw register writes.
- **F1** — directly supported schematic/connector hardware path.
- **F2** — processor-visible host/HAL stimulus or observation.
- **F3** — scan-tool timing, line timing, analog/electrical or other simulation assumptions.
- **F4** — unresolved ALDL transceiver, external line, custom-device or other hardware behavior.

## Diagnostic architecture

Diagnostics are not one monolithic fault routine. The supplied image separates at least four responsibilities:

1. subsystem-specific qualification produces current error bits and counters;
2. Segment-D front-end/logging logic debounces and promotes qualified errors into stored error words;
3. lamp-selection logic decides the raw service-lamp request from running/field-service/diagnostic state;
4. field-service flash sequencing converts stored error words into lamp pulses.

This separation is preserved by `src/diagnostic_qualification.inc.h`, `src/diagnostics.inc.h`, and `src/diagnostic_flash.inc.h`.

## Qualification versus stored history

The qualification block at `$E4F7-$E75C` uses source-ordered tests, timers and state for individual diagnostic paths. Existing translations include Error 13 O2 qualification, Error 21 TPS, Error 24 VSS, Error 32 EGR, Error 33/34 MAF, Error 41 cylinder/configuration consistency, Error 42 spark-feedback consistency, Error 43 knock failure, and Error 44/45 lean/rich O2 behavior.

The important architectural point is that a sensor or subsystem condition does not automatically become a stored trouble code. Current qualification state is first accumulated in the current-error bytes/counters. The logger at `$EFFE-$F10D` masks and debounces those current errors before ORing them into the stored error bytes `$0005-$0009` and recomputing the checksum at `$0018`.

The exact emitted branch senses and fixed-width counter behavior take priority over semantic error names or comments where those disagree.

## Fault retention and clearing

Stored errors live in the five retained error bytes `$0005-$0009`, with a checksum maintained at `$0018`. Normal diagnostic logging can age/clear state according to the listing-backed counters and service conditions.

Mode 4 provides an explicit external command path to clear the five error bytes. On first Mode-4 entry, command byte `$0154` bit 6 clears `$0005-$0009` and immediately recomputes the stored-error checksum. This is distinct from ordinary diagnostic qualification and from the field-service lamp/flash sequencer.

The retained-memory/startup/shutdown chapters remain responsible for the broader key-cycle and keep-alive consequences of those stored bytes.

## Service lamp boundary

The translated lamp helpers manipulate the processor-visible `$3FFC` state according to the listing. This raw write is F0.

The physical malfunction-indicator/service-engine lamp polarity, driver topology and vehicle-side electrical behavior are not inferred from the helper names. A software helper named `lamp_on` describes the listing-backed requested state; the external electrical consequence remains F1/F4 unless the complete hardware path independently establishes it.

Engine-running field-service logic at `$EF40-$EF90` selects among diagnostic/lamp states using RAM control bits and cadence state. That behavior belongs to the firmware layer, not to a scan-tool or dashboard simulation.

## Field-service flash codes

The flash sequencer at `$F10E-$F1BB` is translated separately from qualification and logging. The source starts a new sequence with code 12, repeats each displayed code three times, then scans the masked stored-error words `$0005-$0009` for subsequent codes.

The pulse timing/state is maintained in RAM (`$0042`, `$00B9-$00BD`) and drives the same raw lamp request path. This is a firmware display protocol; the physical brightness, bulb delay and electrical lamp polarity are outside F0.

The code-number traversal follows the emitted bit scan and group boundaries. No extra code table should be invented when the stored-bit-to-code relation can be derived directly from the listing-backed scan.

## Diagnostic-mode input

Chapter 7 establishes `DIAG` as U10 AN7, requested with selector `$70` during startup and Segment-3 processing. Literal raw-count comparisons select ordinary, diagnostic, ALDL or factory-related paths.

Those raw comparisons are F0. The external ALDL-pin resistor arrangement or voltage that produces a particular A/D count is an electrical F1/F3/F4 question and must remain separate unless directly supported by the hardware evidence.

## Normal 160-baud ALDL display stream

The normal low-speed ALDL path is not modeled as a conventional asynchronous UART byte stream. The listing-backed path represented by `src/scheduler_serial.inc.h` uses the ordinary 6.25-ms IRQ plus two compare events:

- the ordinary IRQ schedules the first match at +11 timer counts, sets serial flags and pulls the raw serial output low;
- the +11 compare clears the first flag, schedules the +143 compare and invokes the serial byte/bit manager;
- a shifted data zero raises the line at +11, while a shifted data one leaves it low;
- the +143 compare clears the second flag and raises the line unconditionally.

The resulting firmware-visible behavior is therefore pulse-width-coded cells. The independent PC receiver deliberately observes only line transitions/timestamps; it does not inspect the transmitter's private byte/bit state.

The normal byte manager loads the listing-backed display values only when the diagnostic/factory alternatives are not selected. Some diagnostic/factory branches of the low-speed manager remain intentionally bounded rather than guessed.

## 8192-baud SCI core

Step 115 translated the 8192-baud software message core selected by the SCI IRQ front near `$C9F4` and routines `$FA58-$FC71`. The protocol exposes one top-level device ID, `$80`, and Modes 0 through 4.

The translated F0 protocol includes:

- device-ID matching;
- biased length decoding;
- mode-specific input-length validation;
- modulo-256 receive checksum;
- input and mirror-buffer movement;
- two's-complement transmit checksum;
- reply selection and transmitter/wakeup register states;
- the source-specific Mode 0, 1, 2, 3 and 4 payload rules.

Mode 1 returns the mode plus 63 table-selected bytes. Mode 2 returns the mode plus 63 sequential bytes beginning at the requested address. Modes 3 and 4 derive output count from received length according to the listing arithmetic.

The translation supports RAM `$0000-$04FF` and the listing-proven EPROM ID bytes at `$C000/$C001`. Arbitrary ROM reads that are not represented by source-backed data return an explicit address boundary rather than invented contents.

## Mode 4 command lifecycle

Mode 4 is not activated solely because the SCI receiver accepts a Mode-4 frame. Reception sets the completed-message/serial state and copies the accepted mode/control bytes. A later scheduler pass interprets `$0151` and establishes the Mode-4 state.

On first entry, command byte `$0154` controls three established one-shot actions:

- bit 6 clears stored diagnostic error bytes and refreshes their checksum;
- bit 5 requests the translated IAC reset behavior and writes 255 to the current IAC-position state;
- bit 4 reinitializes the SAM/BLM state through the established BLM initialization path.

The prior-Mode-4 flag prevents those entry commands from repeating on every scheduler pass.

When the completed mode is no longer 4, the scheduler clears the Mode-4 state, associated spark/diagnostic state, and the spark-feedback counter as established at `$CBA4` and surrounding code.

The one-second timeout path increments `$0171`; only a value **greater than 30** forces `$0151` to Mode 0. Exit then occurs on the following scheduler pass. Equality at 30 is intentionally preserved because that is what the listing emits.

## Mode 4 and normal control consumers

The ten Mode-4 control bytes copied to `$0152-$015B` remain ordinary RAM visible to existing fuel, spark, AFR, diagnostic and output consumers. The communications layer therefore does not replace the normal control algorithms with a separate host simulation. It exposes source-backed command bytes that the translated firmware consumes in the usual call paths.

Any host UI that presents friendly controls should map to these processor-visible bytes without changing their F0 semantics.

## SCI/ALDL hardware boundary

The PC HAL supplies complete received 8192-baud frames to the software protocol. Physical baud generation, bit-level receive timing, bus collisions, transceiver polarity, wakeup electrical behavior, ALDL line levels, connector loading and interrupt latency are not reconstructed by the Step-115/116 translation.

Likewise, the 160-baud path can be reproduced at the raw line-transition level without claiming the external ALDL transceiver voltage/polarity. The shared external line is therefore a hardware boundary even though the software-side timing and protocol state are substantially translated.

## Factory test boundary

Factory-test entry and execution are separate from ordinary field diagnostics and normal ALDL operation. Factory-test code exercises A/D, SPI/FMD, MPU/I/O windows, COP/output paths and SWI/reset behavior through its own translated control/execution modules.

Chapter 12 references factory-test mode only where it affects diagnostic-mode selection or communications boundaries. Detailed factory-test startup/exception behavior belongs in Chapter 14.

## Evidence-supported theory of operation

1. Sensor/subsystem routines produce current diagnostic qualification state.
2. Segment-D qualification/logging debounces current errors and promotes persistent failures into retained error words.
3. The raw service-lamp request is selected from normal, running-field-service and diagnostic state; its electrical polarity remains a hardware boundary.
4. Field-service mode flashes code 12 first, repeats each code three times, then scans stored error bits for additional codes.
5. U10 AN7 `DIAG` raw counts select ordinary/diagnostic/ALDL/factory-related startup behavior.
6. Normal 160-baud ALDL uses 6.25-ms pulse-width-coded cells generated by scheduler/compare events, not an assumed UART waveform.
7. The 8192-baud SCI core validates device `$80`, length and checksum, implements Modes 0–4, and builds source-ordered replies.
8. Mode 4 becomes active only when the scheduler interprets the completed received mode; its one-shot reset commands and >30 timeout behavior are listing-backed.
9. Physical ALDL/SCI transceiver timing, voltage, polarity and collision behavior remain F3/F4 rather than being invented in firmware code.

## C implementation and regression correspondence

Relevant current modules include:

- `src/diagnostic_qualification.inc.h` — listing `$E4F7-$E75C` qualification;
- `src/diagnostics.inc.h` — Segment-D front end, logging/debounce and running field-service path;
- `src/diagnostic_flash.inc.h` — `$F10E-$F1BB` flash sequencer;
- `src/scheduler_serial.inc.h` — normal 160-baud pulse-width-coded stream and scheduler interaction;
- `src/sci_8192.inc.h` — Step-115 8192-baud message core;
- `src/mode4_lifecycle.inc.h` — Step-116 scheduler-facing Mode-4 lifecycle;
- `src/factory_test_control.inc.h` and `src/factory_test_execution.inc.h` — separate factory-test boundary.

Existing diagnostic, scheduler/serial, Step-115 SCI, Step-116 Mode-4, retained-memory and frozen-profile regressions remain the proof for the translated behavior. This chapter does not introduce a new behavioral baseline.

## Remaining evidence needs

- Close the external 1227165 ALDL transceiver schematic path well enough to document line polarity/voltage and shared-line behavior without inference.
- Preserve unresolved low-speed diagnostic/factory branches of the 160-baud manager until executable evidence is fully translated.
- Expose additional source-backed ROM bytes to Modes 2/3/4 only when represented by authoritative image data; do not invent arbitrary ROM contents.
- Keep scan-tool presentation/engineering units separate from raw Mode-4 command bytes unless the firmware itself defines the conversion.
- Treat optional ROM, SWI/reset electrical consequences and factory hardware behavior in their dedicated theory chapters rather than folding them into ALDL semantics.
