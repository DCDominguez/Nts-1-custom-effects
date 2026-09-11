#!/usr/bin/env python3
import json, pathlib, subprocess, shutil, sys, re, zipfile

ROOT=pathlib.Path(__file__).resolve().parents[2]
TESTDIR=ROOT/"tests"/"pre-handoff"
OUT=ROOT/"artifacts"/"pre-handoff-arm"
OUT.mkdir(parents=True,exist_ok=True)
SDK=pathlib.Path(sys.argv[1] if len(sys.argv)>1 else "logue-sdk").resolve()
profiles=json.loads((TESTDIR/"profiles.json").read_text())
templates={"osc":"dummy-osc","modfx":"dummy-modfx","delfx":"dummy-delfx","revfx":"dummy-revfx"}
rows=[]; fail=False
for p in profiles:
    uid=p["id"]; nts=ROOT/p["root"]/"nts1"; manifest=json.loads((nts/"manifest.json").read_text()); h=manifest.get("header",{})
    issues=[]
    if h.get("platform")!="nutekt-digital": issues.append("platform")
    if h.get("module")!=p["kind"]: issues.append("module")
    if h.get("api")!="1.1-0": issues.append("api")
    if not h.get("version"): issues.append("version")
    if not h.get("name"): issues.append("name")
    project_mk=(nts/"project.mk").read_text(); m=re.search(r"^\s*PROJECT\s*=\s*([^\s#]+)",project_mk,re.M)
    if not m: issues.append("PROJECT"); project="unknown"
    else: project=m.group(1)
    target=SDK/"platform"/"nutekt-digital"/("prehandoff_"+uid.replace("-","_"))
    if target.exists(): shutil.rmtree(target)
    shutil.copytree(SDK/"platform"/"nutekt-digital"/templates[p["kind"]],target)
    shutil.copy2(nts/"manifest.json",target/"manifest.json"); shutil.copy2(nts/"project.mk",target/"project.mk")
    if (target/"src").exists(): shutil.rmtree(target/"src")
    shutil.copytree(nts/"src",target/"src")
    log=[f"UNIT: {p['display']}",f"MANIFEST: {json.dumps(h,sort_keys=True)}",f"PRECHECK ISSUES: {', '.join(issues) if issues else 'none'}"]
    if issues:
        rows.append((uid,p["kind"],"FAIL","manifest/project precheck: "+",".join(issues))); fail=True; (OUT/(uid+".txt")).write_text("\n".join(log)+"\n"); continue
    env=None
    c=subprocess.run(["make","-C",str(target),"GCC_BIN_PATH=/usr/bin"],cwd=ROOT,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=180,env=env)
    log.append(c.stdout)
    if c.returncode:
        rows.append((uid,p["kind"],"FAIL","ARM compile")); fail=True; (OUT/(uid+".txt")).write_text("\n".join(log)+"\n"); continue
    c2=subprocess.run(["make","-C",str(target),"GCC_BIN_PATH=/usr/bin","install"],cwd=ROOT,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=120)
    log.append(c2.stdout)
    unit=target/(project+".ntkdigunit")
    elf=target/"build"/(project+".elf")
    if c2.returncode or not unit.exists() or not elf.exists():
        rows.append((uid,p["kind"],"FAIL","package/install output missing")); fail=True; (OUT/(uid+".txt")).write_text("\n".join(log)+"\n"); continue
    try:
        with zipfile.ZipFile(unit) as z:
            bad=z.testzip(); names=z.namelist()
            if bad or not names: raise RuntimeError("bad/empty ntkdigunit")
    except Exception as e:
        rows.append((uid,p["kind"],"FAIL","invalid ntkdigunit")); fail=True; log.append(repr(e)); (OUT/(uid+".txt")).write_text("\n".join(log)+"\n"); continue
    size=subprocess.run(["arm-none-eabi-size","-A",str(elf)],text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
    log.append(size.stdout)
    uout=OUT/uid; uout.mkdir(exist_ok=True); shutil.copy2(unit,uout/unit.name); (uout/"MEMORY.txt").write_text(size.stdout)
    rows.append((uid,p["kind"],"PASS",f"{h['name']} {h['version']}")); (OUT/(uid+".txt")).write_text("\n".join(log)+"\n")

summary=["# ARM build/package/manifest retest","", "| Unit | Type | Result | Manifest |","|---|---|---|---|"]
for r in rows: summary.append("| %s | %s | **%s** | %s |"%r)
summary += ["", "This is Layer A build/package/ABI evidence only; it is not physical NTS-1 validation."]
(OUT/"SUMMARY.md").write_text("\n".join(summary)+"\n"); print("\n".join(summary)); sys.exit(1 if fail else 0)
