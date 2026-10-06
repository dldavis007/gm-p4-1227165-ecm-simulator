import numpy as np
from pathlib import Path
# High-impedance custom-chip assumption; these are illustrative DC voltages.
root=Path(__file__).resolve().parents[1]
rs=[]
for l in (root/'evidence/memcal/fitted_models/16055375_15R.cir').read_text().splitlines():
 if l.startswith('R'):
  _,a,b,r=l.split(); rs.append((int(a[1:]),int(b[1:]),float(r[:-1])))
def solve(extra=[]):
 edges=rs+extra; fixed={4:0,7:0,8:0,14:5,15:12}; nodes=sorted(set(x for a,b,r in edges for x in (a,b))-fixed.keys()); ix={x:i for i,x in enumerate(nodes)}; A=np.zeros((len(nodes),len(nodes))); y=np.zeros(len(nodes))
 for a,b,r in edges:
  for n,m in [(a,b),(b,a)]:
   if n in ix:
    i=ix[n]; A[i,i]+=1/r
    if m in ix:A[i,ix[m]]-=1/r
    else:y[i]+=fixed[m]/r
 return {n:round(float(v),6) for n,v in zip(nodes,np.linalg.solve(A,y))}
print('375 network only, VCC5V:',solve())
print('375 plus ECM51.1kVIGN and5.49kground, VCC5V VIGN12V:',solve([(9,15,51.1),(9,4,5.49)]))
print('376 pin2 / VCC=',1.4/(150+1.4))
print('376 pin4 / VIGN=',130/(130+24.9+.1))
