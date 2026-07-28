import subprocess, re, os, json, sys
ROOT="/home/X/Documents/MML-D2-26"
rows=[]
for line in open("/tmp/lane_b_unsplit.txt"):
    m=re.match(r'\s*(\d+)\s+(\S+)\s+(\S+\.s)\s+copies=(\d+)\s+family=(\d+)', line)
    if m:
        rows.append(dict(insns=int(m.group(1)), name=m.group(2), asm=m.group(3),
                         copies=int(m.group(4)), family=int(m.group(5))))
print("parsed", len(rows), file=sys.stderr)
out={}
for r in rows:
    p=os.path.join(ROOT,"asm/rock_neo",r["asm"])
    if not os.path.exists(p):
        r["err"]="no asm file"; out[r["name"]]=r; continue
    try:
        res=subprocess.run(["python3", os.path.join(ROOT,"tools/m2c/m2c.py"),
            "--target","mips-gcc-c","--context","/tmp/lane_b/ctx.c","-f",r["name"],p],
            capture_output=True, text=True, timeout=180, cwd=ROOT)
        r["out"]=res.stdout
        r["stderr"]=res.stderr[-2000:]
    except Exception as e:
        r["err"]=str(e)
    out[r["name"]]=r
json.dump(out, open("/tmp/lane_b/m2c_raw.json","w"), indent=1)
print("done", len(out), file=sys.stderr)
