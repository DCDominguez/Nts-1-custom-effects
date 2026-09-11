#!/usr/bin/env python3
import json, pathlib, subprocess, sys, os, shlex

ROOT = pathlib.Path(__file__).resolve().parents[2]
TESTDIR = ROOT / "tests" / "pre-handoff"
OUT = ROOT / "artifacts" / "pre-handoff-host"
OUT.mkdir(parents=True, exist_ok=True)
profiles = json.loads((TESTDIR / "profiles.json").read_text())

runners = {
    "osc": TESTDIR / "runner_osc.cpp",
    "modfx": TESTDIR / "runner_modfx.cpp",
    "delfx": TESTDIR / "runner_delfx.cpp",
    "revfx": TESTDIR / "runner_revfx.cpp",
}

rows=[]
fail=False
for p in profiles:
    uid=p["id"]
    exe=pathlib.Path("/tmp") / ("pre-handoff-"+uid)
    log=OUT/(uid+".txt")
    cmd=["g++","-std=c++11","-O1","-g","-Wall","-Wextra",
         "-fsanitize=address,undefined","-fno-omit-frame-pointer",
         "-I",str(TESTDIR/"stubs"),str(runners[p["kind"]]),str(ROOT/p["source"]),"-o",str(exe)]
    text=["UNIT: %s"%p["display"],"SOURCE: %s"%p["source"],"COMPILE: "+" ".join(shlex.quote(x) for x in cmd)]
    try:
        c=subprocess.run(cmd,cwd=ROOT,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=120)
    except Exception as e:
        text.append("COMPILE EXCEPTION: %r"%e); log.write_text("\n".join(text)); rows.append((uid,p["kind"],p["status"],"FAIL","compile exception")); fail=True; continue
    text.append(c.stdout)
    if c.returncode:
        rows.append((uid,p["kind"],p["status"],"FAIL","host compile")); fail=True; log.write_text("\n".join(text)); continue
    args=[str(exe),p["display"]]
    if p["kind"]=="delfx": args.append(p.get("tail","bounded"))
    env=dict(os.environ); env.setdefault("ASAN_OPTIONS","detect_leaks=0:halt_on_error=1"); env.setdefault("UBSAN_OPTIONS","halt_on_error=1:print_stacktrace=1")
    try:
        r=subprocess.run(args,cwd=ROOT,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=180,env=env)
    except Exception as e:
        text.append("RUN EXCEPTION: %r"%e); log.write_text("\n".join(text)); rows.append((uid,p["kind"],p["status"],"FAIL","host run exception")); fail=True; continue
    text.append(r.stdout)
    status="PASS" if r.returncode==0 else "FAIL"
    note="common production-DSP host gate"
    if r.returncode: fail=True
    rows.append((uid,p["kind"],p["status"],status,note))
    log.write_text("\n".join(text))

# FIELD keeps its stronger bespoke test in addition to the common suite.
field_log=OUT/"lattice-field-bespoke.txt"
cmd=["g++","-std=c++11","-O1","-g","-Wall","-Wextra","-Werror","-fsanitize=address,undefined","-fno-omit-frame-pointer","-I",str(ROOT/"tests/lattice-field/stubs"),str(ROOT/"tests/lattice-field/test.cpp"),"-o","/tmp/field-bespoke"]
c=subprocess.run(cmd,cwd=ROOT,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=120)
body=["FIELD BESPOKE COMPILE",c.stdout]
if c.returncode==0:
    env=dict(os.environ); env.setdefault("ASAN_OPTIONS","detect_leaks=0:halt_on_error=1"); env.setdefault("UBSAN_OPTIONS","halt_on_error=1:print_stacktrace=1")
    r=subprocess.run(["/tmp/field-bespoke"],cwd=ROOT,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=240,env=env)
    body.append(r.stdout)
    if r.returncode: fail=True; rows.append(("lattice-field-bespoke","delfx","active","FAIL","existing FIELD A-level test"))
    else: rows.append(("lattice-field-bespoke","delfx","active","PASS","existing FIELD A-level test"))
else:
    fail=True; rows.append(("lattice-field-bespoke","delfx","active","FAIL","existing FIELD test compile"))
field_log.write_text("\n".join(body))

summary=["# Pre-handoff production-DSP host test summary","", "| Unit | Type | Repo status | Result | Gate |","|---|---|---|---|---|"]
for row in rows: summary.append("| %s | %s | %s | **%s** | %s |"%row)
summary += ["", "These are host-side engineering tests of production DSP. They do not certify original NTS-1 MkI loading, callback deadline margin, physical control integration, coexistence, or musical quality."]
(OUT/"SUMMARY.md").write_text("\n".join(summary)+"\n")
print("\n".join(summary))
sys.exit(1 if fail else 0)
