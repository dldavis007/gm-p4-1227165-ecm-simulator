# Step 179 — Measured NetRes reconstruction

## Owner confirmation

On 2026-10-06 the owner confirmed that the original ODS contains measurements
between original NetRes pins and is the target for the final replacement.
The owner also explained that the earlier hand reconstruction was imperfect
and did not reproduce every reading. This resolves original-device provenance;
instrument, conditions and uncertainty remain unspecified.

The workbook governs original terminal resistance targets. Photographs govern
the actual installed values/jumpers on the earlier hand reconstruction.
No archived original, photographed part, C algorithm or calibration was changed.

## New work

- Fit nonnegative passive terminal networks directly from the measured matrices,
  starting with every terminal branch rather than the old reconstructed topology.
- Preserve measured shorts, positive readings and disconnected components.
- Prune/refit branches and select practical E96 values.
- Produce original-pin schematics, resistor lists, SPICE subcircuits and full
  pair comparisons for 16055375 14/15/16-resistor candidates and 16055376's
  nine-resistor candidate.
- Rename the earlier comparison's measurement column to
  `original_measured_kohm`; its numeric original/candidate values are unchanged.
- Update current status, evidence hierarchy and theory of operation/PDF, with
  explicit later-evidence addenda to earlier historical records.

The 16055375 preferred 15-resistor E96 candidate has 0.306% RMS and 0.635%
maximum error over its 90 positive recorded pairs. Its zero pair is linked
directly. The 14-resistor candidate has 0.947% RMS and 3.029% maximum error.
The 16055376 nine-resistor E96 candidate has 0.595% RMS and 1.908% maximum
error across its 45 positive recorded pairs. Its two shorts and 72 dash/open
entries are preserved. The blank 12–13 workbook cell is not invented as data.

These are calculated DC fits, not measured new hardware, original internal
topology identification or demonstrated engine suitability. Component-count
search and discrete E96 tuning are heuristic, not global optimality proofs.
See [engineering report](NETRES_MEASURED_MATRIX_RECONSTRUCTION.md).

## Reproduction and validation

Numerical environment: Python 3, NumPy 2.3.5, SciPy 1.17.0, Matplotlib 3.10.8.
Fitting uses bounded nonlinear least squares with analytic conductance
derivatives. E96 tuning uses local neighboring-value coordinate searches.

```text
python tools/fit_netres.py
python tools/summarize_netres_fit.py
python tools/draw_netres_candidates.py
python tools/verify_netres_candidates.py
make test
```

The independent verifier reads emitted SPICE files and uses a full-Laplacian
pseudoinverse, separately from the fitter's grounded nodal solve. It confirms
all 91 recorded 16055375 pairs and all 119 populated 16055376 entries,
including shorts and opens, and confirms counts and original workbook values.
Every computed CSV prediction agrees with the emitted subcircuit.

The diagram generator checks that every emitted candidate resistor is drawn
once; circuit previews were visually inspected. No model pin is tied to global
ground. All local Markdown file targets resolve and the source ODS retains its
manifest SHA-256. Strict C89 regression passes with frozen signatures unchanged.

## Next physical work

The preferred 15-resistor 16055375 network can now be built separately and
compared against the original matrix before replacing the earlier hardware.
The 14-resistor option trades accuracy for one fewer part.
For 16055376 the topology/count remain unchanged; proposed value changes target
the original measurements and do not retroactively redefine the earlier board.
Verify resistor tolerances, construction, isolation and matrix readings on the
new physical candidate before connecting it to an ECM.
