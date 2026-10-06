"""Draw exact fitted candidate connectivity as SVG resistor circuits."""
import json
from html import escape
from summarize_netres_fit import OUT
import xml.etree.ElementTree as ET
import re
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle, Circle

def drawing(name,model):
    width,height = 940,900
    items = ['<svg xmlns="http://www.w3.org/2000/svg" width="940" height="900" viewBox="0 0 940 900">',
             '<rect width="940" height="900" fill="white"/>',
             '<style>text{font-family:Arial,sans-serif;fill:#172d43} .wire{stroke:#172d43;stroke-width:2;fill:none}</style>']
    def text(x,y,s,size=17):
        items.append('<text x="%s" y="%s" font-size="%s" text-anchor="middle">%s</text>'%(x,y,size,escape(s)))
    def wire(x1,y1,x2,y2):
        items.append('<path class="wire" d="M%s %s L%s %s"/>'%(x1,y1,x2,y2))
    def node(x,y,label):
        items.append('<circle cx="%s" cy="%s" r="4" fill="#172d43"/>'%(x,y))
        text(x,y-15,"Pin "+str(label))
    def resistor(x1,y1,x2,y2,label):
        if y1==y2:
            lo,hi=sorted((x1,x2));m=(lo+hi)/2
            wire(lo,y1,m-30,y1);wire(m+30,y1,hi,y1)
            items.append('<rect x="%s" y="%s" width="60" height="20" fill="white" stroke="#172d43" stroke-width="2"/>'%(m-30,y1-10))
            text(m,y1-42,label,15)
        elif x1==x2:
            lo,hi=sorted((y1,y2));m=(lo+hi)/2
            wire(x1,lo,x1,m-25);wire(x1,m+25,x1,hi)
            items.append('<rect x="%s" y="%s" width="20" height="50" fill="white" stroke="#172d43" stroke-width="2"/>'%(x1-10,m-25))
            text(x1+65,m+5,label,15)
        else:
            raise ValueError("Nonorthogonal resistor")
    edges={tuple(sorted((a,b))):(i+1,r) for i,(a,b,r) in enumerate(model["e96_edges"])}
    used=set()
    def edge(a,b,x1,y1,x2,y2):
        pair=tuple(sorted((a,b)));i,r=edges[pair];used.add(pair)
        resistor(x1,y1,x2,y2,"R%d: %g kΩ"%(i,r))
    text(470,35,name+" — fitted replacement candidate",24)
    text(470,62,"Nominal E96 values • Original package-pin numbering • DC resistance model",15)
    if name.startswith("16055375"):
        for n,x in [(1,130),(2,330),(3,530)]:node(x,120,n)
        edge(1,2,130,120,330,120);edge(2,3,330,120,530,120)
        for n,x in [(1,130),(3,530)]:
            edge(n,14,x,120,x,260);node(x,260,14)
        pos={4:800,5:610,6:420,9:230,7:60}
        for n,x in pos.items():node(x,410,"7 = 8" if n==7 else n)
        for a,b in [(4,5),(5,6),(6,9),(9,7)]:
            edge(a,b,pos[a],410,pos[b],410)
        for n,x in [(5,610),(9,230)]:
            edge(n,14,x,410,x,560);node(x,560,14)
        if (3,4) in edges:
            wire(530,120,800,120)
            edge(3,4,800,120,800,410)
        for n,x in [(10,130),(11,340),(12,550),(13,760)]:
            node(x,690,n);edge(n,14,x,690,x,820)
        wire(130,820,760,820);node(445,820,14)
        text(470,868,"Every Pin 14 label is the SAME electrical net. Pins 7 and 8 are directly linked.",16)
    else:
        xs={1:100,2:310,3:520,4:730,6:890}
        for n,x in xs.items():node(x,145,"4 = 5" if n==4 else n)
        for a,b in [(1,2),(2,3),(3,4),(4,6)]:
            edge(a,b,xs[a],145,xs[b],145)
        edge(1,16,100,145,100,320);node(100,320,16)
        wire(520,145,520,350)
        wire(270,350,770,350)
        for n,x in [(13,270),(14,520),(15,770)]:
            edge(3,n,x,350,x,520);node(x,520,n)
        node(310,690,11);node(630,690,12)
        edge(11,12,310,690,630,690)
        text(470,780,"Direct links: Pins 4–5 and Pins 7–9. Pins 8 and 10 are isolated.",17)
        text(470,815,"Pins 11–12 form a separate 10 kΩ branch; it connects to no other pin.",17)
        text(470,868,"Pin 3 feeds the three lower branches. No package pin is tied to global ground.",16)
    assert used==set(edges),(used,set(edges))
    items.append('</svg>')
    (OUT/(name+"_schematic.svg")).write_text("\n".join(items)+"\n")
    fig,ax=plt.subplots(figsize=(9.4,9),dpi=130)
    ax.set_xlim(0,940);ax.set_ylim(900,0);ax.set_aspect("equal");ax.axis("off")
    for element in ET.fromstring("\n".join(items)):
        tag=element.tag.split("}")[-1];a=element.attrib
        if tag=="text":
            ax.text(float(a["x"]),float(a["y"]),element.text or "",
                    ha="center",va="baseline",fontsize=float(a["font-size"])*.72,
                    color="#172d43",fontfamily="DejaVu Sans")
        elif tag=="path":
            x1,y1,x2,y2=map(float,re.findall(r"[-+]?\d+(?:\.\d+)?",a["d"]))
            ax.plot([x1,x2],[y1,y2],color="#172d43",linewidth=1.4)
        elif tag=="circle":
            ax.add_patch(Circle((float(a["cx"]),float(a["cy"])),float(a["r"]),
                                color="#172d43"))
        elif tag=="rect":
            ax.add_patch(Rectangle((float(a.get("x",0)),float(a.get("y",0))),
                float(a["width"]),float(a["height"]),facecolor=a.get("fill","white"),
                edgecolor=a.get("stroke","none"),linewidth=1.4))
    fig.subplots_adjust(0,0,1,1)
    fig.savefig(OUT/(name+"_schematic.png"),facecolor="white")
    plt.close(fig)

def main():
    models=json.loads((OUT/"practical_models.json").read_text())
    for name in ["16055375_14R","16055375_15R","16055376_9R"]:
        drawing(name,models[name.split("_")[0]][name])

if __name__=="__main__":
    main()
