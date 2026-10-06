# Step 182 — power/reset/LIMP and injector electrical theory

2026-10-06. Revision 1.2 extends the six-part schematic/firmware format to
U1 rails/reset/retention, U12 LIMP routing and Q1 injector drive/feedback.
Directly reviewed archived schematic sheets 1, 3, 4 and 6 and current lifecycle/
injection chapters, with emitted listing anchors for watchdog service, startup,
shutdown, injector status and timing writes.

New clarifications: U12 pin 20 is the external LIMP source; its internal
assertion criteria remain unknown. U1 POWEROFF connects to that net, but rail
switching rules are undocumented. U11 has no drawn LIMP input. The U5 RC branch
has an undrawn output destination. U12 INJLIMP pin 48 is labeled but unconnected
on sheet 4; U11's INJLIMP is routed to J3. Do not invent a missing wire.

Independent arithmetic checks: 365k*2.7u = 0.9855s; 10k*2.7u = 0.027s;
ESENSE unloaded gain 20/(68.1+20) = 0.2270148; shunt 0.103V/A. These describe
external components, not chip thresholds, regulator equations or mode delays.

Validation: regenerated PDF, extracted revision/section text, rendered expanded
sections and inspected layout, checked local Markdown links and staged whitespace.
No C firmware, tests, candidate resistor values or behavioral baselines changed.
No powered or continuity tests performed; custom-device functions remain bounded.
