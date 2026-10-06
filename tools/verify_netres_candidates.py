"""Independent netlist check using full-Laplacian pseudoinverse resistance.

Checks emitted SPICE connectivity against all recorded positive, zero and
open-pair entries. Does not use the fitter's grounded solve.
"""
import csv
import re
import json
import numpy as np
from summarize_netres_fit import OUT
from fit_netres import measurements

def main():
    original = measurements()
    for path in sorted(OUT.glob("*.cir")):
        name=path.stem
        part=name.split("_")[0]
        pins=list(range(1,15 if part=="16055375" else 17))
        parent={n:n for n in pins}
        def find(n):
            while parent[n]!=n:n=parent[n]
            return n
        lines=path.read_text().splitlines()
        for line in lines:
            m=re.match(r"VSHORT\d+ P(\d+) P(\d+) 0$",line)
            if m:parent[find(int(m[2]))]=find(int(m[1]))
        edges=[]
        for line in lines:
            m=re.match(r"R\d+ P(\d+) P(\d+) ([\d.]+)k$",line)
            if m:
                r=float(m[3]);assert r>0
                edges.append((find(int(m[1])),find(int(m[2])),r))
        roots=sorted(set(find(n) for n in pins));index={n:i for i,n in enumerate(roots)}
        L=np.zeros((len(roots),len(roots)))
        adjacency={n:set() for n in roots}
        for a,b,r in edges:
            i,j=index[a],index[b];g=1/r
            L[i,i]+=g;L[j,j]+=g;L[i,j]-=g;L[j,i]-=g
            adjacency[a].add(b);adjacency[b].add(a)
        inverse=np.linalg.pinv(L,hermitian=True)
        def resistance(a,b):
            a,b=find(a),find(b)
            seen={a}
            while True:
                new=seen | {v for u in seen for v in adjacency[u]}
                if new==seen:break
                seen=new
            if b not in seen:return "OPEN"
            i,j=index[a],index[b]
            return max(0,float(inverse[i,i]+inverse[j,j]-2*inverse[i,j]))
        rows=list(csv.DictReader((OUT/(name+"_comparison.csv")).open()))
        assert len(rows)==len(original[part])
        errors=[]
        for row in rows:
            a,b=int(row["terminal_a"]),int(row["terminal_b"])
            source=original[part][a,b]
            expected="OPEN" if source=="-" else float(source)
            recorded=row["original_measured_kohm"]
            assert recorded=="OPEN" if expected=="OPEN" else float(recorded)==expected
            computed=resistance(a,b)
            reported=row["e96_model_kohm"]
            if expected=="OPEN":
                assert computed==reported=="OPEN"
            else:
                assert computed!="OPEN"
                assert np.isclose(computed,float(reported),rtol=1e-9,atol=1e-8)
                if expected>0:errors.append(abs(computed-expected)/expected*100)
                else:assert computed<1e-8
        assert len(edges)==int(re.search(r"_(\d+)R",name)[1])
        print(name,len(rows),"recorded pairs verified; max %.6f%%"%max(errors))

if __name__=="__main__":
    main()
