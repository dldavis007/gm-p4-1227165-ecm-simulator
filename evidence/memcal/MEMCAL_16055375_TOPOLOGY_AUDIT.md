# MEMCAL 16055375 topology audit

## Scope

This audit collapses the generic-grid reconstruction of the 16055375 MEMCAL network into terminal-level electrical nodes and compares that result with the saved LTspice model. The completed MEMCAL photograph remains authoritative for as-built populated parts where markings and jumper placement are legible. KiCad is used here as connectivity evidence for copper/net relationships that cannot be read reliably from the photograph alone.

## Package-to-grid mapping

The original 16055375 is a 14-terminal network implemented on a generic 16-position board:

- original terminals 1-7 -> J1-J7
- J8 and J9 -> unused package positions
- original terminals 8-14 -> J10-J16

Therefore J7 and J10 correspond to original terminals 7 and 8, respectively, and J16 corresponds to original terminal 14.

## Zero-ohm collapse of the generic grid

Tracing the KiCad netlist and collapsing every populated zero-ohm link produces two important results:

1. **J7 and J10 are the same electrical node.** This reproduces the LTspice common-node relationship between original terminals 7 and 8.
2. Internal grid nets associated with the J16 side collapse into one common node connected directly to J16. This makes original terminal 14 the major common/reference node of the reconstructed network.

J8 and J9 remain unused package positions and do not become original-network terminals.

## Simplified reconstructed terminal network

After zero-ohm collapse, the generic-grid reconstruction reduces to the following resistor branches. Values shown here are the KiCad reconstruction-file values unless otherwise noted; a clearly legible photographed installed value is authoritative for the as-built board.

| Original terminals | Generic positions | KiCad branch |
|---|---|---:|
| 1-2 | J1-J2 | 13 kOhm |
| 2-3 | J2-J3 | 39 kOhm |
| 4-5 | J4-J5 | 36 kOhm |
| 5-6 | J5-J6 | 47 kOhm |
| 1-14 | J1-J16 | 75 kOhm |
| 2-14 | J2-J16 | 330 kOhm |
| 3-14 | J3-J16 | 15 kOhm |
| 4-14 | J4-J16 | 510 kOhm |
| 5-14 | J5-J16 | 270 kOhm |
| 6-14 | J6-J16 | 470 kOhm |
| 13-14 | J15-J16 | 18 kOhm |
| 12-14 | J14-J16 | 10 kOhm |
| 11-14 | J13-J16 | 91 kOhm |
| 10-14 | J12-J16 | 91 kOhm |
| 9-14 | J11-J16 | 100 kOhm in KiCad; retained reconstruction parts list records 220 kOhm for R15 |
| 9-(7/8) | J11-(J7/J10) | 75 kOhm |

The zero-ohm routing additionally enforces original terminal 7 = original terminal 8.

## Agreement with LTspice

Most of the reconstructed branches correspond directly to branches present in the LTspice model, including:

- 1-2 = 13 kOhm
- 2-3 = 39 kOhm
- 4-5 = 36 kOhm
- 5-6 = 47 kOhm
- 1-14 = 75 kOhm
- 2-14 = 330 kOhm
- 3-14 = 15 kOhm
- 4-14 = 510 kOhm
- 5-14 = 270 kOhm
- 13-14 = 18 kOhm
- 12-14 = 10 kOhm
- 11-14 = 91 kOhm
- 10-14 = 91 kOhm
- terminal 7 and terminal 8 common

This is strong evidence that the reconstruction orientation and most terminal assignments are correct.

## Non-identical internal topology in the terminal-6 / terminal-9 section

The remaining section does **not** collapse into the same resistor-for-resistor topology as LTspice.

The generic-grid reconstruction simplifies the area to:

- terminal 6 -> terminal 14: 470 kOhm
- terminal 9 -> terminal 14: one direct branch
- terminal 9 -> terminal 7/8 common: 75 kOhm

The saved LTspice model instead contains a larger internal-node structure in this area, including explicit **620 kOhm**, **470 kOhm**, **220 kOhm**, **100 kOhm**, and **75 kOhm** elements.

Therefore the reconstructed network must not be described as resistor-for-resistor or internally topologically identical to LTspice.

## Targeted terminal-resistance comparison

The saved LTspice drawing contains unlabeled numeric comments positioned adjacent to the external terminals. Their placement is consistent with the historical terminal-resistance comparison work. For this audit they are treated as **terminal-adjacent recorded resistance values**, with that interpretation explicitly kept separate from the resistor-model topology because the comments themselves do not spell out the measurement procedure.

The values relevant to the disputed section are approximately:

- terminal 6 relative to terminal 14: **158 kOhm**
- terminal 7 relative to terminal 14: **291 kOhm**
- terminal 8 relative to terminal 14: **291 kOhm**
- terminal 9 relative to terminal 14: **220 kOhm**

A nodal equivalent-resistance calculation of the full LTspice resistor model gives:

| Terminal pair | Recorded value | Full LTspice equivalent | Difference |
|---|---:|---:|---:|
| 6-14 | 158 kOhm | 157.9 kOhm | -0.1 kOhm |
| 7/8-14 | 291 kOhm | 261.5 kOhm | -29.5 kOhm |
| 9-14 | 220 kOhm | 223.1 kOhm | +3.1 kOhm |

Using the **current KiCad value of 100 kOhm** for the simplified terminal-9-to-14 branch produces approximately:

| Terminal pair | Recorded value | Simplified KiCad equivalent |
|---|---:|---:|
| 6-14 | 158 kOhm | 153.4 kOhm |
| 7/8-14 | 291 kOhm | 175.0 kOhm |
| 9-14 | 220 kOhm | 100.0 kOhm |

That version is plainly inconsistent with the retained terminal-resistance values for terminals 7/8 and 9.

However, the retained `Resistors.txt` reconstruction parts list records **R15 = 220 kOhm**, not 100 kOhm. Treating the simplified as-built branch as 220 kOhm changes the calculated terminal equivalents to approximately:

| Terminal pair | Recorded value | Simplified reconstruction with 220 kOhm branch | Difference |
|---|---:|---:|---:|
| 6-14 | 158 kOhm | 153.4 kOhm | -4.6 kOhm |
| 7/8-14 | 291 kOhm | 295.0 kOhm | +4.0 kOhm |
| 9-14 | 220 kOhm | 220.0 kOhm | 0.0 kOhm |

This is a much stronger match to the retained terminal-resistance evidence than the stale KiCad 100-kOhm value and, for terminals 7/8, is also closer than the nominal LTspice internal model.

## Interpretation

The evidence now supports a more specific interpretation:

- the as-built 16055375 reconstruction was likely designed as an **external-terminal electrical equivalent**, not as a literal reproduction of the inaccessible internal resistor topology;
- the generic-grid simplification deliberately removes the LTspice internal-node 620-kOhm/220-kOhm structure;
- the retained 220-kOhm reconstruction value is essential to that equivalence and explains why the KiCad 100-kOhm record should not be treated as the as-built branch value;
- the available terminal-resistance comparison strongly supports the simplified reconstruction at terminals 6, 7/8, 9, and 14.

This does not prove equality for every possible terminal pair. A full external-terminal resistance matrix would be the strongest mathematical closure if the original measurement table can be recovered or the physical network can be remeasured.

## CAL translation

Using the established package-to-CAL mapping, the simplified reconstructed branches become:

| Original terminals | CAL nodes | Simplified branch |
|---|---|---:|
| 1-2 | CAL33-CAL35 | 13 kOhm |
| 2-3 | CAL35-CAL37 | 39 kOhm |
| 4-5 | CAL39-CAL41 | 36 kOhm |
| 5-6 | CAL41-CAL43 | 47 kOhm |
| 1-14 | CAL33-CAL34 | 75 kOhm |
| 2-14 | CAL35-CAL34 | 330 kOhm |
| 3-14 | CAL37-CAL34 | 15 kOhm |
| 4-14 | CAL39-CAL34 | 510 kOhm |
| 5-14 | CAL41-CAL34 | 270 kOhm |
| 6-14 | CAL43-CAL34 | 470 kOhm |
| 13-14 | CAL36-CAL34 | 18 kOhm |
| 12-14 | CAL38-CAL34 | 10 kOhm |
| 11-14 | CAL40-CAL34 | 91 kOhm |
| 10-14 | CAL42-CAL34 | 91 kOhm |
| 9-14 | CAL44-CAL34 | **220 kOhm in retained reconstruction parts list; KiCad 100 kOhm is stale/conflicting** |
| 9-(7/8) | CAL44-(CAL45/CAL46) | 75 kOhm |

CAL45 and CAL46 are electrically common in the reconstruction.

## Conclusion

The 16055375 reconstruction is much simpler electrically than its generic-grid appearance. It is not resistor-for-resistor identical to the LTspice internal model, but the targeted external-terminal resistance comparison now gives substantial evidence that the simplification was intentional and electrically valid at the disputed terminals when the retained **220 kOhm** reconstruction value is used.

The main remaining closure item is no longer "why are the topologies different?" but rather whether the simplified as-built network matches the original across the **full terminal-resistance matrix**, not only the presently recoverable terminal-14 comparisons.
