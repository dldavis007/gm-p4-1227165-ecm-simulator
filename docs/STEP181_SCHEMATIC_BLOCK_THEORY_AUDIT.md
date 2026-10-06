# Step 181 — integrated schematic-block theory, first expansion

2026-10-06. Theory of Operation revision 1.1 introduces the six-part electrical/
firmware block structure and an index spanning all six archived schematic sheets.
Expanded sections cover measured NetRes/U11/U12 configuration and DIAG mode
selection, serial payloads, idle effects, and the owner's reported solder repair.
Detailed reference chapters carry the same reviewed sections.

Read schematic sheets and emitted listing instructions; verified Segment-3
thresholds at EA28–EA47, serial selection F8B6–F8E8, display table C6FD–C70C,
IAC mode path D448–D450 and fuel accumulator additions E4EE–E4F3/F733–F739.
The observed ~1000-RPM idle is not represented as a fixed ALDL target: the
examined firmware command path substitutes 80 in RPM/25 units (2000 RPM).

The historical source is the owner's CorvetteForum thread, linked in the theory;
owner confirmed recollection in session. Pin-disconnection voltage and dashboard
parser internals remain inferences/boundaries, not established measurements.

Validation: regenerated publication PDF; checked extracted text for revision,
expanded sections and Appendix D; checked internal Markdown file links and
staged whitespace. Firmware, tests, measured candidate values and behavioral
baselines are unchanged. No hardware testing performed. Remaining electrical
expansion is explicitly indexed, not claimed complete.
