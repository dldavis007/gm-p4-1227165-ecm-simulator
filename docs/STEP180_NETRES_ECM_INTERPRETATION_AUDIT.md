# Step 180 — measured NetRes candidates against ECM schematics

2026-10-06. Directly inspected archived ECM sheets 2, 4, 5 and 6, MEMCAL
connection DOCX, BUA photograph and Step-179 SPICE candidates. Resolved J4
supply/ground positions; identified motherboard bias loading and the measured
TPS-to-U11-MAP jumper path. Full findings and caveats are in
[NetRes circuit interpretation](NETRES_ECM_CIRCUIT_INTERPRETATION.md).

Added a reproducible high-impedance DC nodal calculation. Checked its results
against divider equations and inspected both emitted candidate netlists.
No resistor candidates, original evidence, firmware or simulator behavior changed.
Earlier unresolved-reference statements are superseded by the sheet-5 mappings;
older audit records remain historical. No physical continuity or powered tests
were performed. Custom-chip equations remain unproven.
