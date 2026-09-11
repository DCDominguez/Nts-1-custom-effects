#!/usr/bin/env python3
import os, pathlib, subprocess, sys, tempfile, textwrap

ROOT = pathlib.Path(__file__).resolve().parents[2]
STUBS = ROOT / "tests" / "pre-handoff" / "stubs"
OUT = ROOT / "tests" / "pre-handoff" / "results.md"

UNITS = [
    ("SPECTRA", "osc", "oscillators/spectra/nts1/src/spectra.cpp"),
    ("PARALLAX", "delfx", "effects/parallax/nts1/src/parallax.cpp"),
    ("CHORDGHOST", "delfx", "effects/chordghost/nts1/src/chordghost.cpp"),
    ("DUST", "modfx", "effects/dust/nts1/src/dust.cpp"),
    ("CARRIER", "modfx", "effects/carrier/nts1/src/carrier.cpp"),
    ("VECTORFILTER", "modfx", "effects/vectorfilter/nts1/src/vectorfilter.cpp"),
    ("IRONROT", "modfx", "effects/ironrot/nts1/src/ironrot.cpp"),
    ("ATTRACTOR", "modfx", "effects/attractor/nts1/src/attractor.cpp"),
    ("ZEROCROSS", "modfx", "effects/zerocross/nts1/src/zerocross.cpp"),
    ("PHASEWELL", "modfx", "effects/phasewell/nts1/src/phasewell.cpp"),
    ("ASCENDER", "modfx", "effects/ascender/nts1/src/ascender.cpp"),
    ("HELIX", "modfx", "effects/helix/nts1/src/helix.cpp"),
    ("CAPSTAN", "modfx", "effects/capstan/nts1/src/capstan.cpp"),
    ("SIDEBAND", "modfx", "effects/sideband/nts1/src/sideband.cpp"),
    ("FAULTLINE", "modfx", "effects/faultline/nts1/src/faultline.cpp"),
    ("BALLISTIC", "delfx", "effects/ballistic/nts1/src/ballistic.cpp"),
    ("RAINFALL", "delfx", "effects/rainfall/nts1/src/rainfall.cpp"),
    ("SWARMDELAY", "delfx", "effects/swarmdelay/nts1/src/swarmdelay.cpp"),
    ("GLITCHREPEAT", "delfx", "effects/glitchrepeat/nts1/src/glitchrepeat.cpp"),
    ("BUCKETLINE", "delfx", "effects/bucketline/nts1/src/bucketline.cpp"),
    ("LONGMEMORY", "delfx", "effects/longmemory/nts1/src/longmemory.cpp"),
    ("SHARD", "delfx", "effects/shard/nts1/src/shard.cpp"),
    ("ABYSS", "revfx", "effects/abyss/nts1/src/abyss.cpp"),
    ("AUREOLE", "revfx", "effects/aureole/nts1/src/aureole.cpp"),
    ("NEBULA", "revfx", "effects/nebula/nts1/src/nebula.cpp"),
    ("LATTICE CORE", "modfx", "effects/lattice-core/nts1/src/lattice_core.cpp"),
    ("LATTICE ECHO", "delfx", "effects/lattice-echo/nts1/src/lattice_echo.cpp"),
    ("LATTICE SPACE", "revfx", "effects/lattice-cloud/nts1/src/lattice_cloud.cpp"),
    ("LATTICE FIELD", "delfx", "effects/lattice-field/nts1/src/lattice_field.cpp"),
]

COMMON = r'''
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>
static int32_t q31p(float x) { if (x < 0.f) x=0.f; if (x > .999999f) x=.999999f; return (int32_t)(x*2147483647.f); }
static bool finite_vec(const std::vector<float>& v) { for(float x:v) if(!std::isfinite(x)) return false; return true; }
static float energy(const std::vector<float>& v, size_t start=0) { double e=0; for(size_t i=start;i<v.size();++i) e += (double)v[i]*v[i]; return (float)e; }
static float maxabs(const std::vector<float>& v) { float m=0; for(float x:v) m=std::max(m,std::fabs(x)); return m; }
static void fill_sine(std::vector<float>& v, float hz, float amp=.35f, float phase=0.f) { for(size_t i=0;i<v.size()/2;i++){ float s=amp*std::sin(6.283185307179586f*(hz*(float)i/48000.f+phase)); v[2*i]=s; v[2*i+1]=s*.83f; } }
static void fail(const char* s){ std::fprintf(stderr,"FAIL: %s\n",s); std::exit(2); }
'''

def harness_modfx(src):
    return COMMON + f'\n#include "{src.as_posix()}"\n' + r'''
int main(){
  const uint32_t B=64; std::vector<float> in(B*2), out(B*2), sub(B*2), sout(B*2);
  MODFX_INIT(0,0); MODFX_PARAM(k_user_modfx_param_time,q31p(.5f)); MODFX_PARAM(k_user_modfx_param_depth,q31p(.5f));
  MODFX_PROCESS(in.data(),out.data(),sub.data(),sout.data(),B); if(!finite_vec(out)||maxabs(out)>8.f) fail("silence safety");
  fill_sine(in,220.f); MODFX_PROCESS(in.data(),out.data(),sub.data(),sout.data(),B); if(!finite_vec(out)||maxabs(out)>8.f||energy(out)<1e-8f) fail("normal signal delivery");
  for(int k=0;k<1200;k++){ float a=(k%200)/199.f,b=((k*37)%200)/199.f; MODFX_PARAM(k_user_modfx_param_time,q31p(a)); MODFX_PARAM(k_user_modfx_param_depth,q31p(b)); fill_sine(in,55.f+(k%12)*73.f,.55f); MODFX_PROCESS(in.data(),out.data(),sub.data(),sout.data(),B); if(!finite_vec(out)||maxabs(out)>8.f) fail("sweep/soak bound"); }
  MODFX_SUSPEND(); MODFX_RESUME(); std::fill(in.begin(),in.end(),0.f); for(int k=0;k<64;k++){ MODFX_PROCESS(in.data(),out.data(),sub.data(),sout.data(),B); if(!finite_vec(out)||maxabs(out)>8.f) fail("reset/rest bound"); }
  return 0;
}
'''

def harness_delfx(src):
    return COMMON + f'\n#include "{src.as_posix()}"\n' + r'''
int main(){
  const uint32_t B=64; std::vector<float> x(B*2);
  DELFX_INIT(0,0); DELFX_PARAM(k_user_delfx_param_time,q31p(.5f)); DELFX_PARAM(k_user_delfx_param_depth,q31p(.5f)); DELFX_PARAM(k_user_delfx_param_shift_depth,q31p(.85f));
  x[0]=.5f; x[1]=.4f; double post=0, early=0, late=0; const int blocks=12000;
  for(int k=0;k<blocks;k++){ DELFX_PROCESS(x.data(),B); if(!finite_vec(x)||maxabs(x)>8.f) fail("impulse/tail bound"); double e=energy(x); if(k<20) early+=e; if(k>20) post+=e; if(k>blocks-500) late+=e; std::fill(x.begin(),x.end(),0.f); }
  if(post<1e-9) fail("no delayed delivery");
  for(int k=0;k<1800;k++){ float a=(k%240)/239.f,b=((k*41)%240)/239.f,m=((k*17)%240)/239.f; DELFX_PARAM(k_user_delfx_param_time,q31p(a)); DELFX_PARAM(k_user_delfx_param_depth,q31p(b)); DELFX_PARAM(k_user_delfx_param_shift_depth,q31p(m)); fill_sine(x,80.f+(k%16)*61.f,.5f); DELFX_PROCESS(x.data(),B); if(!finite_vec(x)||maxabs(x)>8.f) fail("sweep/soak bound"); }
  DELFX_SUSPEND(); DELFX_RESUME(); std::fill(x.begin(),x.end(),0.f); for(int k=0;k<64;k++){ DELFX_PROCESS(x.data(),B); if(!finite_vec(x)||maxabs(x)>8.f) fail("reset/rest bound"); }
  return 0;
}
'''

def harness_revfx(src):
    return COMMON + f'\n#include "{src.as_posix()}"\n' + r'''
int main(){
  const uint32_t B=64; std::vector<float> x(B*2);
  REVFX_INIT(0,0); REVFX_PARAM(k_user_revfx_param_time,q31p(.6f)); REVFX_PARAM(k_user_revfx_param_depth,q31p(.6f)); REVFX_PARAM(k_user_revfx_param_shift_depth,q31p(.8f));
  x[0]=.5f; x[1]=.4f; double post=0; const int blocks=12000;
  for(int k=0;k<blocks;k++){ REVFX_PROCESS(x.data(),B); if(!finite_vec(x)||maxabs(x)>8.f) fail("impulse/tail bound"); if(k>5) post+=energy(x); std::fill(x.begin(),x.end(),0.f); }
  if(post<1e-9) fail("no reverb delivery");
  for(int k=0;k<1800;k++){ float a=(k%240)/239.f,b=((k*29)%240)/239.f,m=((k*11)%240)/239.f; REVFX_PARAM(k_user_revfx_param_time,q31p(a)); REVFX_PARAM(k_user_revfx_param_depth,q31p(b)); REVFX_PARAM(k_user_revfx_param_shift_depth,q31p(m)); fill_sine(x,70.f+(k%17)*67.f,.5f); REVFX_PROCESS(x.data(),B); if(!finite_vec(x)||maxabs(x)>8.f) fail("sweep/soak bound"); }
  REVFX_SUSPEND(); REVFX_RESUME(); std::fill(x.begin(),x.end(),0.f); for(int k=0;k<128;k++){ REVFX_PROCESS(x.data(),B); if(!finite_vec(x)||maxabs(x)>8.f) fail("reset/rest bound"); }
  return 0;
}
'''

def harness_osc(src):
    return r'''
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>
static void fail(const char* s){ std::fprintf(stderr,"FAIL: %s\n",s); std::exit(2); }
''' + f'\n#include "{src.as_posix()}"\n' + r'''
static double render(uint8_t note){ user_osc_param_t p{}; p.pitch=(uint16_t)note<<8; p.shape_lfo=0; const uint32_t B=64; std::vector<int32_t> y(B); double e=0,zc=0; int32_t prev=0; for(int k=0;k<750;k++){ OSC_CYCLE(&p,y.data(),B); for(auto q:y){ float f=(float)q/2147483648.f; if(!std::isfinite(f)||std::fabs(f)>1.01f) fail("osc finite/bound"); e+=(double)f*f; if((prev<0&&q>=0)||(prev>=0&&q<0)) zc++; prev=q; } } if(e<1e-8) fail("osc silent"); return zc; }
int main(){ OSC_INIT(0,0); user_osc_param_t p{}; p.pitch=69u<<8; OSC_NOTEON(&p); double z1=render(57),z2=render(69); if(z2<=z1*1.25) fail("pitch response sanity"); const uint16_t idxs[]={k_user_osc_param_shape,k_user_osc_param_shiftshape,k_user_osc_param_id1,k_user_osc_param_id2,k_user_osc_param_id3,k_user_osc_param_id4,k_user_osc_param_id5,k_user_osc_param_id6}; const uint16_t vals[]={0,512,1023}; for(auto idx:idxs) for(auto v:vals){ uint16_t vv=v; if(idx==k_user_osc_param_id1) vv=(v==0?0:(v==512?2:3)); else if(idx==k_user_osc_param_id4) vv=(v==0?0:(v==512?4:7)); else if(idx>=k_user_osc_param_id2 && idx<=k_user_osc_param_id6) vv=(v==0?0:(v==512?50:100)); OSC_PARAM(idx,vv); render(69); } OSC_NOTEOFF(&p); return 0; }
'''

def build_one(name, typ, rel):
    src = ROOT / rel
    if not src.exists(): return False, "source missing"
    if typ == "modfx": code=harness_modfx(src)
    elif typ == "delfx": code=harness_delfx(src)
    elif typ == "revfx": code=harness_revfx(src)
    else: code=harness_osc(src)
    with tempfile.TemporaryDirectory(prefix="prehand_") as td:
        cpp=pathlib.Path(td)/"test.cpp"; exe=pathlib.Path(td)/"test"
        cpp.write_text(code)
        cmd=["g++","-std=c++17","-O1","-g","-fsanitize=address,undefined","-fno-omit-frame-pointer",f"-I{STUBS}",str(cpp),"-lm","-o",str(exe)]
        cp=subprocess.run(cmd,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
        if cp.returncode: return False, "compile: "+cp.stdout[-1200:].replace("\n"," | ")
        rp=subprocess.run([str(exe)],text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,env={**os.environ,"ASAN_OPTIONS":"detect_leaks=0:halt_on_error=1"})
        if rp.returncode: return False, "runtime: "+rp.stdout[-1200:].replace("\n"," | ")
        return True, "common production-DSP host gate PASS"

def main():
    rows=[]; failed=0
    for name,typ,rel in UNITS:
        ok,note=build_one(name,typ,rel); rows.append((name,typ,"PASS" if ok else "FAIL",note)); failed += 0 if ok else 1
        print(f"{name}: {'PASS' if ok else 'FAIL'} - {note}")
    OUT.parent.mkdir(parents=True,exist_ok=True)
    with OUT.open("w") as f:
        f.write("# Suite pre-handoff common-gate results\n\n")
        f.write("This is the deterministic production-DSP common gate from `PRE_HANDOFF_PROTOCOL.md`. It does not certify physical MkI loading, real-time CPU margin, or subjective tone/musicality. Unit-specific musical identity remains subject to each QA/spec and DC listening.\n\n")
        f.write("| Unit | Type | Common gate | Notes |\n|---|---|---:|---|\n")
        for a,b,c,d in rows: f.write(f"| {a} | `{b}` | **{c}** | {d.replace('|','/')} |\n")
        f.write(f"\n**Result: {len(rows)-failed}/{len(rows)} common-gate PASS; {failed} FAIL.**\n")
    return 1 if failed else 0

if __name__ == "__main__": sys.exit(main())
