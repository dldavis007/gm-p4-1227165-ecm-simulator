# MEMCAL 16055376 as-built photograph audit

## Authority

The completed reverse-engineered MEMCAL close-up is the controlling evidence for the as-built hand reconstruction wherever a component marking, jumper, open position, or orientation is legible. KiCad, LTspice, and historical notes remain supporting design/model evidence.

Authoritative image retained in the project/library:

- `reverse_engineered_memcal_resistor_board_closeup.jpeg`

## Complete nonzero resistor population visible in photograph

All nine nonzero resistor markings on the 16055376 reconstruction are legible in the completed-board photograph.

| Visible marking | Decoded resistance | LTspice nominal set | As-built status |
|---|---:|---:|---|
| `1503` | 150 kOhm | 150 kOhm | confirmed from photograph |
| `1501` | 1.5 kOhm | 1.5 kOhm | confirmed from photograph |
| `1303` | 130 kOhm | 130 kOhm | confirmed from photograph |
| `2402` | 24 kOhm | 24 kOhm | confirmed from photograph |
| `1002` | 10 kOhm | 10 kOhm | confirmed from photograph |
| `8201` | 8.2 kOhm | 8.2 kOhm | confirmed from photograph |
| `5101` | 5.1 kOhm | 5.1 kOhm | confirmed from photograph |
| `7501` | 7.5 kOhm | 7.5 kOhm | confirmed from photograph |
| `1002` | 10 kOhm | 10 kOhm | confirmed from photograph |

The photographed as-built resistor set therefore matches the complete nominal LTspice set exactly:

`150 k, 1.5 k, 130 k, 24 k, 10 k, 8.2 k, 5.1 k, 7.5 k, 10 kOhm`.

## Consequence for KiCad differences

The KiCad reconstruction record contains several values that differ from the finished board, including approximately 1.45 kOhm, 5.0 kOhm, 7.4 kOhm, and the previously noted 25.5-Ohm entry. Those values must not be used as the as-built population.

In particular, the completed photograph clearly shows `2402` at the formerly disputed adjacent-chain position. The as-built value is therefore 24 kOhm, and the KiCad 25.5-Ohm value is a design/reconstruction-record error.

Likewise, the completed photograph promotes 1.5 kOhm, 5.1 kOhm, and 7.5 kOhm to authoritative as-built values rather than the nearby KiCad alternatives.

## Zero-ohm routing population

The completed photograph visibly contains numerous black SMD links marked `0`, plus a direct bridged/routed adjacent-chain connection. These are routing/configuration elements of the generic grid and are not calibration resistor values.

The photograph is authoritative for whether a visible routing site is populated. However, this audit does not assign every visible `0` part to a KiCad reference designator unless the board/netlist location can be traced unambiguously. That prevents a photo-site-to-designator guess from being promoted to evidence.

## Functional simplification status

The 16055376 value-set question is now closed:

- nine nonzero components are present;
- all nine are readable;
- all nine match the LTspice nominal resistor values exactly;
- the differences in the KiCad value record are not as-built substitutions.

This strongly supports using the LTspice network as the candidate functional-equivalent topology, provided the photographed zero-ohm routing and terminal connections are shown to produce the same node relationships.

The remaining work is therefore **topology**, not resistor-value identification:

1. trace each populated zero-ohm route into electrical nodes;
2. associate those nodes with local J1-J16 terminals;
3. collapse the routing grid;
4. compare the resulting nine-resistor equivalent network with the LTspice topology;
5. express the confirmed equivalent network in absolute J4/CAL terms.

## Confidence

High confidence: complete as-built nonzero resistor value set.

High confidence: zero-ohm parts are routing/configuration links rather than calibration values.

Not yet fully closed: exact node-by-node collapse of every zero-ohm jumper into the simplest terminal-level equivalent circuit.
