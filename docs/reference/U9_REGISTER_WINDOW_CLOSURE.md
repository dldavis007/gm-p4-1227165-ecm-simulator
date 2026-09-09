# U9 register-window closure

## Purpose

This document closes the current evidence-supported classification of the U9 (`16045148`) processor window at `$3FC0-$3FFF` without inventing semantics for addresses that the existing listing-backed audits have not established.

It builds on `U9_REGISTER_WINDOW_MAP.md`, Steps 131, 134, 135, 136 and 137, and the authoritative `evidence/firmware/bua-hac.lst`.

## Closure rule

An address is classified only when current repository evidence establishes a processor-visible use by executable firmware or factory test. Addresses lacking such established semantics remain **unclassified**, not "unused". The distinction is important because hardware may implement registers that this calibration never exercises.

## Established addresses

The current evidence-supported U9 map is:

| Address | Current supported role | Evidence status |
| --- | --- | --- |
| `$3FC0` | reference-period state | Direct firmware evidence |
| `$3FC8` | spark/reference-period state | Direct firmware evidence |
| `$3FCA` | changing knock-event quantity used by knock logic and Error 43 | Direct firmware evidence; U9 `KNOCK` pin 42 -> register relationship remains strong inference |
| `$3FCE` | EFI delay register exercised by factory test | Direct firmware evidence |
| `$3FD0` | synchronous injector pulse-width command | Direct firmware evidence |
| `$3FDC` | dwell-period value | Direct firmware evidence |
| `$3FE4` | next-dwell timing state | Direct firmware evidence |
| `$3FE6` | dwell delta/update state | Direct firmware evidence |
| `$3FE8` | current fire/fall delta | Direct firmware evidence |
| `$3FEC` | counter value at last reference | Direct firmware evidence |
| `$3FF6` | reference-to-fire offset | Direct firmware evidence |
| `$3FFA` | aggregate status word; reference occurrence and injector-service state are independently consumed | Direct firmware evidence; individual hardware input-to-bit mapping unresolved |
| `$3FFC` | EST/bypass and other control/status state | Direct firmware evidence; undocumented bit meanings remain bounded |

These are the only addresses for which the current project documentation has established specific semantics. No additional address is promoted merely because it lies within the same decoded hardware window.

## What remains intentionally unclassified

Every other location in `$3FC0-$3FFF` remains one of the following until new evidence appears:

- a hardware register not exercised by the supplied calibration;
- a reserved or mirrored location;
- a register used only by another ECM/MEMCAL/calibration variant;
- or a location whose use has not yet been demonstrated by the retained evidence.

The current evidence does not distinguish among those possibilities. Therefore Step 138 does **not** label any remaining address reserved, unused, read-only, write-only, mirrored, or variant-specific.

## Hardware correspondence remains bounded

U9 has directly visible external pins `IGN`, `INJS`, `INJA`, `INJREF`, `IGNREF`, `ESTLOOP`, and `KNOCK`. The register window has directly established processor semantics at the addresses above. The internal mapping between those pins and those registers remains undocumented except where the project explicitly marks a strong inference.

In particular:

- U9 pin 42 `KNOCK` -> `$3FCA` remains strongly inferred rather than directly documented;
- `INJREF` and `IGNREF` must not be assigned one-to-one to `$3FC0`, `$3FC8`, `$3FEC`, or individual `$3FFA` bits;
- `ESTLOOP` must not be assigned to a particular `$3FFA`/`$3FFC` bit without additional evidence;
- `$3FD0`/`$3FCE` do not reveal the internal production of `INJS` versus `INJA`;
- spark/dwell register values do not reveal U9's internal counter topology or output-edge polarity.

## Consequence for simulation

No C source or behavioral baseline change is justified. The current simulator should continue to model only the firmware-visible semantics already supported by the listing and expose raw hardware events through the HAL where appropriate. Unclassified addresses should remain explicit unknown hardware state rather than receiving convenience behavior.

## Evidence needed to go further

The highest-value new evidence would be:

1. independent U9 documentation or a pin/register description;
2. controlled hardware tests correlating U9 pin activity with processor reads/writes;
3. another verified 1227165 listing or calibration that accesses additional `$3FC0-$3FFF` locations; or
4. oscilloscope/logic-analyzer observations tying `INJREF`, `IGNREF`, `ESTLOOP`, `KNOCK`, `IGN`, `INJS`, or `INJA` to known processor-visible state.

Until then, the evidence-supported U9 map is closed at the address set above and the rest of the decoded window remains deliberately unclassified.
