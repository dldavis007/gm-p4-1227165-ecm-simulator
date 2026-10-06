"""Compare the retained 16055375 table with the documented candidate network.

Uses Python standard library only. The owner confirmed on 2026-10-06 that
the workbook records original-device pin-to-pin measurements and is the
replacement target. Instrument/setup details remain unspecified.
"""
import csv
import pathlib
import xml.etree.ElementTree as ET
import zipfile

ROOT = pathlib.Path(__file__).resolve().parents[1]
NS = {"t": "urn:oasis:names:tc:opendocument:xmlns:table:1.0",
      "x": "urn:oasis:names:tc:opendocument:xmlns:text:1.0"}
# Candidate topology from MEMCAL_16055375_TOPOLOGY_AUDIT.md, including
# its retained-parts-list 220 kOhm branch; not an asserted as-built measurement.
BRANCHES = [(1,2,13),(2,3,39),(4,5,36),(5,6,47),(1,14,75),
            (2,14,330),(3,14,15),(4,14,510),(5,14,270),(6,14,470),
            (13,14,18),(12,14,10),(11,14,91),(10,14,91),
            (9,14,220),(9,7,75)]

def node(n):
    return 7 if n == 8 else n

def resistance(a, b):
    a, b = node(a), node(b)
    if a == b:
        return 0.0
    nodes = [n for n in range(1,15) if n not in (8,b)]
    indices = {n:i for i,n in enumerate(nodes)}
    matrix = [[0.0] * (len(nodes)+1) for _ in nodes]
    for x,y,r in BRANCHES:
        g = 1.0/r
        for u,v in ((x,y),(y,x)):
            if u != b:
                i = indices[u]
                matrix[i][i] += g
                if v != b:
                    matrix[i][indices[v]] -= g
    matrix[indices[a]][-1] = 1.0
    for col in range(len(nodes)):
        pivot = max(range(col,len(nodes)), key=lambda i:abs(matrix[i][col]))
        matrix[col],matrix[pivot] = matrix[pivot],matrix[col]
        divisor = matrix[col][col]
        if abs(divisor) < 1e-12:
            raise ValueError("Disconnected model")
        matrix[col] = [v/divisor for v in matrix[col]]
        for i in range(len(nodes)):
            if i != col:
                factor = matrix[i][col]
                matrix[i] = [v-factor*w for v,w in zip(matrix[i],matrix[col])]
    return matrix[indices[a]][-1]

def comparisons():
    path = ROOT/"evidence/memcal/originals/NetRes 16055375 and 16055376.ods"
    with zipfile.ZipFile(path) as archive:
        root = ET.fromstring(archive.read("content.xml"))
    table = next(t for t in root.findall(".//t:table",NS)
                 if t.get("{"+NS["t"]+"}name") == "16055375")
    for row in table.findall("t:table-row",NS)[1:]:
        cells = []
        for cell in row:
            text = " ".join("".join(p.itertext()) for p in cell.findall("x:p",NS))
            repeat = min(int(cell.get("{"+NS["t"]+"}number-columns-repeated","1")),14)
            cells.extend([text]*repeat)
        if not cells or not cells[0].isdigit():
            continue
        a = int(cells[0])
        for b,text in enumerate(cells[1:14],2):
            if text:
                observed = float(text)
                calculated = resistance(a,b)
                difference = calculated-observed
                yield [a,b,observed,round(calculated,6),round(difference,6),
                       round(abs(difference)/observed*100,6) if observed else ""]

if __name__ == "__main__":
    rows = list(comparisons())
    assert len(rows) == 91
    assert abs(resistance(7,8)) < 1e-9
    assert abs(resistance(9,14)-220) < 1e-6
    output = ROOT/"evidence/memcal/16055375_WORKBOOK_MODEL_COMPARISON.csv"
    with output.open("w",newline="") as stream:
        writer = csv.writer(stream)
        writer.writerow(["terminal_a","terminal_b","original_measured_kohm",
                         "candidate_kohm","difference_kohm","absolute_percent"])
        writer.writerows(rows)
    print("Compared 91 recorded pairs; output:", output.relative_to(ROOT))
    print("6-9: workbook 187; candidate %.6f kOhm" % resistance(6,9))
