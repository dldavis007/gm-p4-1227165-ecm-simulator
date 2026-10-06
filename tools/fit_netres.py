"""Fit passive terminal resistor networks to owner-confirmed original measurements.

Requires NumPy and SciPy. Original workbook is read without modification.
Optimizes relative resistance error, then prunes branches and refits.
"""
from pathlib import Path
import csv
import json
import itertools
import zipfile
import xml.etree.ElementTree as ET
import numpy as np
from scipy.optimize import least_squares

ROOT = Path(__file__).resolve().parents[1]
NS = {"t":"urn:oasis:names:tc:opendocument:xmlns:table:1.0",
      "x":"urn:oasis:names:tc:opendocument:xmlns:text:1.0"}

def measurements():
    path = ROOT/"evidence/memcal/originals/NetRes 16055375 and 16055376.ods"
    with zipfile.ZipFile(path) as z:
        root = ET.fromstring(z.read("content.xml"))
    result = {}
    for table in root.findall(".//t:table",NS):
        name = table.get("{"+NS["t"]+"}name")
        rows = []
        for row in table.findall("t:table-row",NS):
            cells = []
            for c in row:
                text = " ".join("".join(p.itertext()) for p in c.findall("x:p",NS))
                repeat = min(int(c.get("{"+NS["t"]+"}number-columns-repeated","1")),20)
                cells.extend([text]*repeat)
            rows.append(cells)
        columns = rows[0]
        data = {}
        for cells in rows[1:]:
            if not cells or not cells[0].isdigit():
                continue
            a = int(cells[0])
            for i,value in enumerate(cells[1:],1):
                if value and i < len(columns) and columns[i].isdigit():
                    b = int(columns[i])
                    if b > a:
                        data[a,b] = value if value == "-" else float(value)
        result[name] = data
    return result

def setup(data):
    parent = {n:n for pair in data for n in pair}
    def find(n):
        while parent[n] != n:
            n = parent[n]
        return n
    for (a,b),r in data.items():
        if r == 0:
            parent[find(b)] = find(a)
    positive = [(find(a),find(b),r,a,b) for (a,b),r in data.items()
                if isinstance(r,float) and r > 0]
    # Each disconnected finite-resistance component is fitted independently.
    groups = []
    remaining = set(n for a,b,*_ in positive for n in (a,b))
    while remaining:
        group = {min(remaining)}
        while True:
            expanded = group | {n for a,b,*_ in positive if a in group or b in group
                                for n in (a,b)}
            if expanded == group:
                break
            group = expanded
        groups.append(sorted(group))
        remaining -= group
    return find,positive,groups

def fit_component(nodes,observations):
    size = len(nodes)-1
    index = {n:i for i,n in enumerate(nodes[:-1])}
    def incidence(a,b):
        v = np.zeros(size)
        if a in index: v[index[a]] += 1
        if b in index: v[index[b]] -= 1
        return v
    pairs = list(itertools.combinations(nodes,2))
    B = np.array([incidence(a,b) for a,b in pairs])
    D = np.array([incidence(a,b) for a,b,*_ in observations])
    target = np.array([r for a,b,r,*_ in observations])
    # Fit conductances in inverse kOhm; all branches are nonnegative.
    def solve(active,initial):
        incidence_edges = B[active]
        cache = {}
        def evaluate(g):
            if "g" not in cache or not np.array_equal(g,cache["g"]):
                L = incidence_edges.T @ (g[:,None]*incidence_edges)
                try:
                    voltages = np.linalg.solve(L,D.T)
                except np.linalg.LinAlgError:
                    voltages = np.linalg.solve(L+np.eye(size)*1e-14,D.T)
                values = np.sum(D*voltages.T,axis=1)
                jac = -(incidence_edges @ voltages).T**2
                cache.update(g=g.copy(),res=(values-target)/target,
                             jac=jac/target[:,None],values=values)
            return cache
        fit = least_squares(lambda g:evaluate(g)["res"],np.maximum(initial,1e-10),
                            jac=lambda g:evaluate(g)["jac"],bounds=(0,np.inf),
                            x_scale="jac",ftol=1e-10,xtol=1e-10,gtol=1e-10,
                            max_nfev=400)
        values = evaluate(fit.x)["values"].copy()
        errors = (values-target)/target*100
        return fit.x,values,dict(rms_percent=float(np.sqrt(np.mean(errors**2))),
                               max_percent=float(max(abs(errors))))
    # Grounded impedance estimated directly from measured terminal resistances.
    lookup = {}
    for a,b,r,*_ in observations:
        lookup.setdefault(tuple(sorted((a,b))),[]).append(r)
    R = np.zeros((len(nodes),len(nodes)))
    for i,a in enumerate(nodes):
        for j,b in enumerate(nodes):
            if i != j: R[i,j] = np.mean(lookup[tuple(sorted((a,b)))])
    Z = (R[:-1,-1,None]+R[-1,None,:-1]-R[:-1,:-1])/2
    eig = np.linalg.eigvalsh(Z)
    inverse = np.linalg.pinv(Z)
    start = []
    for a,b in pairs:
        if b == nodes[-1]:
            start.append(max(1e-6,float(sum(inverse[index[a]]))))
        else:
            start.append(max(1e-6,float(-inverse[index[a],index[b]])))
    active = np.arange(len(pairs))
    g,values,stats = solve(active,np.array(start))
    history = []
    # Remove the weakest conductance, refit, retain the whole accuracy/count path.
    while len(active) >= size:
        history.append(dict(count=len(active),stats=stats.copy(),
                            edges=[(pairs[i][0],pairs[i][1],float(1/v))
                                   for i,v in zip(active,g) if v > 1e-12],
                            predicted=values.tolist()))
        if len(active) == size: break
        choices = []
        # At small component counts, test every possible single-edge removal.
        # This is a heuristic topology search, not a proof of minimality.
        candidates = range(len(g)) if len(g) <= 16 else [int(np.argmin(g))]
        for k in candidates:
            next_active = np.delete(active,k)
            if np.linalg.matrix_rank(B[next_active]) < size:
                continue
            next_g = np.delete(g,k)
            try:
                fitted,next_values,next_stats = solve(next_active,next_g)
                choices.append((next_stats["rms_percent"],next_active,
                                fitted,next_values,next_stats))
            except (ValueError,np.linalg.LinAlgError):
                continue
        if not choices:
            break
        _,next_active,fitted,next_values,next_stats = min(choices,key=lambda x:x[0])
        if next_stats["max_percent"] > 30:
            break
        active,g,values,stats = next_active,fitted,next_values,next_stats
    return history,dict(grounded_impedance_min_eigenvalue=float(eig.min()),
                        negative_implied_branches=int(sum(np.array(start)<=1e-6)))

def main():
    out = ROOT/"evidence/memcal/fitted_models"
    out.mkdir(exist_ok=True)
    results = {}
    for name,data in measurements().items():
        find,positive,groups = setup(data)
        components = []
        for group in groups:
            observations = [o for o in positive if o[0] in group]
            history,diagnostics = fit_component(group,observations)
            print(name,group,diagnostics,flush=True)
            print([(h["count"],round(h["stats"]["rms_percent"],3),
                    round(h["stats"]["max_percent"],3)) for h in history],flush=True)
            components.append(dict(nodes=group,observations=observations,
                                   history=history,diagnostics=diagnostics))
        results[name] = dict(components=components,
                             shorts=[list(pair) for pair,r in data.items() if r==0],
                             open_pairs=[list(pair) for pair,r in data.items() if r=="-"])
    (out/"fit_results.json").write_text(json.dumps(results,indent=2)+"\n")

if __name__ == "__main__":
    main()
