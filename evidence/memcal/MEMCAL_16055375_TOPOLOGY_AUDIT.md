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

After zero-ohm collapse, the generic-grid reconstruction reduces to the following resistor branches. Values shown are the reconstruction-file values and must defer to a clearly legible photographed installed value if a conflict is found.

| Original terminals | Generic positions | Reconstructed branch |
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
| 9-14 | J11-J16 | 100 kOhm |
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

## Important non-equivalence in the terminal-6 / terminal-9 section

The remaining section does **not** collapse into the same resistor-for-resistor topology as LTspice.

The generic-grid reconstruction simplifies the area to:

- terminal 6 -> terminal 14: 470 kOhm
- terminal 9 -> terminal 14: 100 kOhm
- terminal 9 -> terminal 7/8 common: 75 kOhm

The saved LTspice model instead contains a larger internal-node structure in this area, including explicit **620 kOhm**, **470 kOhm**, **220 kOhm**, **100 kOhm**, and **75 kOhm** elements.

Therefore the reconstructed network must **not** yet be called topologically identical to the LTspice model. The difference may represent:

- an intentional equivalent-network transformation,
- a reconstruction simplification derived from terminal-resistance measurements,
- a missed or incorrect jumper/component in the physical reconstruction,
- an error in one of the historical model/reconstruction files, or
- a non-equivalent reconstruction.

The photograph determines what was actually built; terminal-resistance comparison or targeted continuity/resistance measurements are needed to determine whether the simplified as-built network is electrically equivalent at the external terminals.

## CAL translation

Using the established package-to-CAL mapping, the simplified reconstructed branches become:

| Original terminals | CAL nodes | Reconstructed branch |
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
| 9-14 | CAL44-CAL34 | 100 kOhm |
| 9-(7/8) | CAL44-(CAL45/CAL46) | 75 kOhm |

And CAL45/CAL46 are electrically common in the reconstruction.

## Conclusion

The 16055375 reconstruction is now much simpler to describe electrically than as a generic grid. Most of the network aligns cleanly with LTspice, and the terminal-7/terminal-8 common node is independently reproduced. However, the terminal-6 / terminal-9 / terminal-14 / terminal-7/8 region is a genuine audit discrepancy and remains open pending external-terminal resistance comparison or targeted physical measurement.
