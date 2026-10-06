"""Emit practical E96 candidates, SPICE subcircuits and pair comparisons."""
import csv
import json
import math
from pathlib import Path
import numpy as np
from fit_netres import ROOT, measurements

E96 = [100,102,105,107,110,113,115,118,121,124,127,130,133,137,140,143,
       147,150,154,158,162,165,169,174,178,182,187,191,196,200,205,210,
       215,221,226,232,237,243,249,255,261,267,274,280,287,294,301,309,
       316,324,332,340,348,357,365,374,383,392,402,412,422,432,442,453,
       464,475,487,499,511,523,536,549,562,576,590,604,619,634,649,665,
       681,698,715,732,750,768,787,806,825,845,866,887,909,931,953,976]
VALUES = np.array(sorted(v*10**d/100 for d in range(-3,7) for v in E96))
OUT = ROOT/"evidence/memcal/fitted_models"

def predictions(nodes,observations,edges):
    index = {n:i for i,n in enumerate(nodes[:-1])}
    def incidence(a,b):
        v = np.zeros(len(index))
        if a in index: v[index[a]] += 1
        if b in index: v[index[b]] -= 1
        return v
    B = np.array([incidence(a,b) for a,b,r in edges])
    D = np.array([incidence(a,b) for a,b,*_ in observations])
    L = B.T @ (np.array([1/r for a,b,r in edges])[:,None]*B)
    voltages = np.linalg.solve(L,D.T)
    return np.sum(D*voltages.T,axis=1)

def statistics(observations,pred):
    target = np.array([r for a,b,r,*_ in observations])
    err = (pred-target)/target*100
    return dict(rms_percent=float(np.sqrt(np.mean(err**2))),
                max_percent=float(max(abs(err))),
                tolerance_1_percent_max=float(max(
                    np.max(abs((pred*.99-target)/target*100)),
                    np.max(abs((pred*1.01-target)/target*100)))))

def practical(nodes,observations,edges):
    nominal = [list(e) for e in edges]
    indices = [int(np.argmin(abs(np.log(VALUES/r)))) for a,b,r in edges]
    for e,k in zip(nominal,indices): e[2] = float(VALUES[k])
    def score(edges):
        s = statistics(observations,predictions(nodes,observations,edges))
        return s["rms_percent"] + .25*s["max_percent"]
    best = score(nominal)
    for _ in range(8):
        changed = False
        for i,k in enumerate(indices):
            options = []
            for delta in [-1,1]:
                nominal[i][2] = float(VALUES[k+delta])
                options.append((score(nominal),k+delta))
            nominal[i][2] = float(VALUES[k])
            candidate,new = min(options)
            if candidate < best-1e-10:
                best = candidate;indices[i]=new;nominal[i][2]=float(VALUES[new])
                changed = True
        if not changed:break
    return nominal

def main():
    fits = json.loads((OUT/"fit_results.json").read_text())
    summary = {}
    table = []
    for name,counts in [("16055375",[14,15,16]),("16055376",[8])]:
        comp = fits[name]["components"][0]
        obs = comp["observations"]
        summary[name] = {}
        for count in counts:
            h = next(h for h in comp["history"] if h["count"] == count)
            ideal = h["edges"]
            actual = practical(comp["nodes"],obs,ideal)
            suffix = "%s_%dR" % (name,count if name=="16055375" else count+1)
            pred = predictions(comp["nodes"],obs,actual)
            stats = statistics(obs,pred)
            ideal_stats = h["stats"]
            if name == "16055376":
                complete_obs = obs+[[11,12,10,11,12]]
                stats = statistics(complete_obs,np.append(pred,10))
                ideal_stats = statistics(complete_obs,np.append(h["predicted"],10))
                ideal = ideal+[[11,12,10]]
                actual = actual+[[11,12,10]]
            summary[name][suffix] = dict(ideal_edges=ideal,e96_edges=actual,
                ideal_stats=ideal_stats,e96_stats=stats,
                shorts=fits[name]["shorts"])
            table.append([suffix,len(actual),ideal_stats["rms_percent"],
                          ideal_stats["max_percent"],stats["rms_percent"],
                          stats["max_percent"],stats["tolerance_1_percent_max"]])
            with (OUT/(suffix+"_resistors.csv")).open("w",newline="") as f:
                writer = csv.writer(f,lineterminator="\n")
                writer.writerow(["reference","terminal_a","terminal_b",
                                 "ideal_kohm","e96_kohm"])
                for i,(a,b,r) in enumerate(actual):
                    writer.writerow(["R"+str(i+1),a,b,ideal[i][2],r])
            with (OUT/(suffix+"_comparison.csv")).open("w",newline="") as f:
                writer=csv.writer(f,lineterminator="\n")
                writer.writerow(["terminal_a","terminal_b","original_measured_kohm",
                                 "ideal_model_kohm","e96_model_kohm",
                                 "e96_difference_kohm","e96_absolute_percent"])
                for o,v,ideal_v in zip(obs,pred,h["predicted"]):
                    a,b,r,orig_a,orig_b=o
                    writer.writerow([orig_a,orig_b,r,ideal_v,v,v-r,abs(v-r)/r*100])
                for a,b in fits[name]["shorts"]:
                    writer.writerow([a,b,0,0,0,0,0])
                if name=="16055376":writer.writerow([11,12,10,10,10,0,0])
                for a,b in fits[name]["open_pairs"]:
                    writer.writerow([a,b,"OPEN","OPEN","OPEN","",""])
            # All package pins appear in original order; no reference terminal
            # is tied to global ground. Zero-volt sources enforce pin shorts.
            lines=["* Owner-measured matrix fit; values are kOhm.",
                   "* Candidate DC resistor model; no undocumented device dynamics.",
                   ".subckt NETRES_"+suffix+" "+" ".join("P%d"%n for n in
                       range(1,15 if name=="16055375" else 17))]
            for i,(a,b,r) in enumerate(actual):
                lines.append("R%d P%d P%d %.8gk"%(i+1,a,b,r))
            for i,(a,b) in enumerate(fits[name]["shorts"]):
                lines.append("VSHORT%d P%d P%d 0"%(i+1,a,b))
            lines.append(".ends NETRES_"+suffix)
            (OUT/(suffix+".cir")).write_text("\n".join(lines)+"\n")
    (OUT/"practical_models.json").write_text(json.dumps(summary,indent=2)+"\n")
    with (OUT/"ACCURACY_COMPONENT_COUNT.csv").open("w",newline="") as f:
        w=csv.writer(f,lineterminator="\n")
        w.writerow(["model","resistor_count","ideal_rms_percent","ideal_max_percent",
                    "e96_rms_percent","e96_max_percent",
                    "e96_1_percent_tolerance_max_percent"])
        w.writerows(table)
    # Save the complete component-count error curve without the bulky optimizer
    # iterate data; full selected models remain reproducible in fit_results.json.
    print(json.dumps(table,indent=2))

if __name__ == "__main__":
    main()
