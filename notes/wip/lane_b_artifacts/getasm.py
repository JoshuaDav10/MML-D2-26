import sys,re,os,json
ROOT="/home/X/Documents/MML-D2-26"
d=json.load(open("/tmp/lane_b/m2c_raw.json"))
def get(name):
    v=d[name]
    p=os.path.join(ROOT,"asm/rock_neo",v["asm"])
    lines=open(p).read().splitlines()
    out=[];on=False
    for l in lines:
        if l.startswith("glabel "):
            on = (l.split()[1]==name)
            if on: out.append(l); continue
            if out: break
        elif on:
            if l.startswith("nonmatching ") or l.startswith("glabel "): break
            out.append(l)
    return "\n".join(out).rstrip()
for n in sys.argv[1:]:
    print("### "+n+"  ("+d[n]["asm"]+")")
    print(get(n)); print()
