# Step 183 — analog input circuit theory

2026-10-06. Revision 1.3 adds six-part block treatments for CTS/MAT, TPS/VMAF
and VOLT/PUMPVOLT. Reviewed sheet-2 printed resistor/capacitor connections,
sheet-1 converter/channel map, the existing sensor audit and emitted listing
paths F3B9–F3EA, CC24, F7AC–F7B6, E816–E81B, EB05 and EBB3.

CTS software hysteresis is reported as exact processed-byte boundaries, not
assumed measured temperatures. High-impedance CTS bias sum is 3998 ohms;
active-high CTSHI drive is explicitly conditional. Sensor transfer curves,
internal output loading and unlabeled capacitor values remain boundaries.

Independent arithmetic: divider gain 8.06/41.26 = 0.19534658; parallel
resistance 6.485507k; RC products 6.485507ms and 30.481881ms. These are ideal
external circuit predictions, not verified U10 counts or measured time constants.

Corrected older ADC reference statement that CAL61 carries actual MAP: the
owner-measured 376 link routes TPS to that U11 terminal on the mapped MEMCAL.
Original schematic and measured artifacts unchanged.

Validation: regenerated PDF; extracted revised sections/revision; rendered
new sections and reviewed layout; verified local Markdown links and whitespace.
No C changes, behavioral baseline changes, candidate-value changes or bench tests.
