#!/usr/bin/env python3
import os
import pathlib
import subprocess
import sys
import tempfile

ROOT=pathlib.Path(__file__).resolve().parents[2]
STUBS=ROOT/'tests'/'pre-handoff'/'stubs'
PROBES=ROOT/'tests'/'pre-handoff'/'level_calibration'
OUT=ROOT/'tests'/'pre-handoff'/'level-calibration-results.md'
CASES=[('Delay family',PROBES/'test_delay_levels.cpp'),('Reverb family',PROBES/'test_reverb_levels.cpp'),('Flagged ModFX',PROBES/'test_mod_levels.cpp')]

def run(src):
    with tempfile.TemporaryDirectory(prefix='level_cal_') as td:
        exe=pathlib.Path(td)/'probe'
        cmd=['g++','-std=c++17','-O2',f'-I{ROOT}',f'-I{STUBS}',str(src),'-lm','-o',str(exe)]
        cp=subprocess.run(cmd,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
        if cp.returncode:
            return False,'compile: '+cp.stdout
        rp=subprocess.run([str(exe)],text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,env=os.environ.copy())
        return rp.returncode==0,rp.stdout

def main():
    rows=[];fail=0
    for name,src in CASES:
        ok,text=run(src);fail+=0 if ok else 1
        rows.append((name,ok,text.strip()))
        print(f'== {name} ==\n{text}')
    with OUT.open('w') as f:
        f.write('# Level calibration measurements\n\n')
        f.write('Deterministic host-side loudness/presence proxies using production DSP. These measurements are calibration evidence, not a substitute for physical NTS-1 MkI listening.\n\n')
        for name,ok,text in rows:
            f.write(f'## {name} — {"PASS" if ok else "FAIL"}\n\n```text\n{text}\n```\n\n')
    return 1 if fail else 0

if __name__=='__main__':
    sys.exit(main())
