# MEMCAL as-built evidence hierarchy

## Scope

This document defines the evidence priority for the hand-reconstructed BUA MEMCAL and its resistor-network boards.

The project distinguishes two different questions:

1. What was actually installed on the reconstructed MEMCAL?
2. What was the intended or equivalent electrical behavior of the original molded networks?

Those questions use related evidence, but they do not have the same authority order.

## Authority for the as-built reconstruction

For the physical hand-reconstructed MEMCAL, the user-supplied close-up photograph of the completed MEMCAL is the controlling evidence wherever the installed component, marking, jumper, open position, or orientation is visually legible.

Evidence priority for the as-built reconstruction is:

1. **Completed MEMCAL photograph / direct owner inspection** — authoritative for what was actually installed, including resistor markings, zero-ohm jumper population, open positions, orientation, and physical placement when legible.
2. **Direct electrical measurement of that same physical reconstruction** — authoritative for continuity or resistance behavior that cannot be determined visually, subject to normal in-circuit measurement limitations.
3. **KiCad reconstruction files** — design/construction records that may represent an earlier plan, transcription, or revision and therefore do not override a clearly legible installed component.
4. **LTspice models** — electrical/theoretical reconstruction evidence used to understand intended topology and terminal behavior; they do not override the as-built photograph.
5. **Historical spreadsheets, text notes, and secondary research** — supporting evidence only.

If the photograph is unclear at a particular location, the ambiguity remains open until resolved by a better image, direct inspection, or targeted measurement. The project must not silently substitute a KiCad or LTspice value merely because it is cleaner or theoretically expected.

## Authority for original-network theory

The photograph is authoritative for the discrete replacement that was actually built. It is not, by itself, proof of the inaccessible internal component-by-component construction of the original 16055375 or 16055376 molded networks.

For reconstructing the original/equivalent network behavior, use the physical replacement together with terminal-resistance measurements, LTspice topology, KiCad connectivity, motherboard traces, and theory-of-operation evidence. Agreement among independent sources raises confidence; disagreement is an audit item, not permission to overwrite the physical evidence.

## Conflict rule

When a legible installed resistor or jumper in the completed MEMCAL photograph conflicts with KiCad, LTspice, or a historical note:

- record the photographed installed part as the **as-built value/population**;
- preserve the conflicting design/model value separately;
- investigate whether the discrepancy is a construction revision, transcription error, substitute value, topology misunderstanding, or reverse-engineering error;
- do not normalize the records to force agreement.

This rule applies in particular to the current 16055376 J5-J6 / nominal-model discrepancy. The next determination must come from the photograph or direct inspection of that physical location, not from choosing between KiCad and LTspice.
