#include "stubs/usermodfx.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include <chrono>

void MODFX_INIT(uint32_t,uint32_t);
void MODFX_PROCESS(const float*,float*,const float*,float*,uint32_t);
void MODFX_SUSPEND(void);
void MODFX_RESUME(void);
void MODFX_PARAM(uint8_t,int32_t);

float host_test_bpm = 120.0f;
static uint32_t rng_state = 0x31415926u;

static void die(const char* m){ std::fprintf(stderr,"FAIL: %s\n",m); std::exit(1); }
static void check(bool ok,const char* m){ if(!ok) die(m); }
static int32_t q(float x){ x=std::max(0.0f,std::min(1.0f,x)); return static_cast<int32_t>(x*2147483000.0f); }
static float tone(uint64_t n,float hz=220.0f,float a=.4f){ return a*std::sin(6.2831853071795864769*hz*(double)n/48000.0); }
static float noise(float a=.4f){ rng_state=rng_state*1664525u+1013904223u; return a*((int32_t)(rng_state>>8)/8388608.0f-1.0f); }

struct Stats{ double in_abs=0,out_abs=0,diff_abs=0; float peak=0; uint64_t frames=0; };
static void process_block(const float* in,float* out,uint32_t frames,Stats& s){
  MODFX_PROCESS(in,out,nullptr,nullptr,frames);
  for(uint32_t i=0;i<frames*2u;++i){
    check(std::isfinite(out[i]),"non-finite output");
    s.peak=std::max(s.peak,std::fabs(out[i]));
    check(std::fabs(out[i])<8.0f,"unbounded output");
    s.in_abs += std::fabs(in[i]); s.out_abs += std::fabs(out[i]); s.diff_abs += std::fabs(out[i]-in[i]);
  }
  s.frames += frames;
}
static void setp(float a,float b){ MODFX_PARAM(k_user_modfx_param_time,q(a)); MODFX_PARAM(k_user_modfx_param_depth,q(b)); }

int main(int argc,char**argv){
  const char* name=argc>1?argv[1]:"modfx";
  constexpr uint32_t B=64;
  float in[B*2],out[B*2];
  Stats all;

  MODFX_INIT(0,0); setp(0,0); MODFX_RESUME();
  for(uint32_t block=0;block<375;++block){
    std::fill(in,in+B*2,0.0f); std::fill(out,out+B*2,0.0f); Stats s; process_block(in,out,B,s);
    if(s.peak>1.0e-5f) die("silence generates unintended output");
  }
  std::puts("PASS silence/rest");

  double best_diff=0;
  const float vals[3]={0.0f,0.5f,1.0f};
  for(float a:vals) for(float b:vals){
    MODFX_INIT(0,0); setp(a,b); MODFX_RESUME(); Stats s; uint64_t n=0;
    for(uint32_t block=0;block<375;++block){
      for(uint32_t i=0;i<B;++i,++n){ float l=tone(n,220,.42f),r=.73f*tone(n,329.63f,.42f); in[2*i]=l; in[2*i+1]=r; }
      process_block(in,out,B,s);
    }
    best_diff=std::max(best_diff,s.diff_abs/std::max<uint64_t>(1,s.frames*2)); all.peak=std::max(all.peak,s.peak);
  }
  check(best_diff>1.0e-5,"no measurable effect behavior across parameter grid");
  std::printf("PASS parameter grid; max mean |wet-dry| %.8f\n",best_diff);

  MODFX_INIT(0,0); setp(1,1); MODFX_RESUME(); Stats hot; uint64_t n=0;
  for(uint32_t block=0;block<750;++block){
    for(uint32_t i=0;i<B;++i,++n){ float x=.72f*tone(n,110,.95f)+.28f*noise(.95f); x=std::max(-.98f,std::min(.98f,x)); in[2*i]=x; in[2*i+1]=-.61f*x+.15f*noise(.5f); }
    process_block(in,out,B,hot);
  }
  std::printf("PASS hot/broadband bounded; peak %.6f\n",hot.peak);

  MODFX_INIT(0,0); MODFX_RESUME(); Stats sweep; n=0;
  const auto t0=std::chrono::steady_clock::now();
  for(uint32_t block=0;block<1500;++block){
    float p=(block%200)/199.0f; if((block/200)&1u) p=1.0f-p; setp(p,1.0f-p*.73f);
    for(uint32_t i=0;i<B;++i,++n){ float x=(block%7==0)?noise(.5f):tone(n,55.0f+330.0f*p,.5f); in[2*i]=x; in[2*i+1]=(block&1)?-.7f*x:.7f*x; }
    process_block(in,out,B,sweep);
  }
  const auto us=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-t0).count();
  std::printf("PASS sweep/soak; peak %.6f; host %.3f us/block\n",sweep.peak,double(us)/1500.0);

  MODFX_SUSPEND(); MODFX_RESUME(); Stats reset;
  for(uint32_t block=0;block<150;++block){ std::fill(in,in+B*2,0.0f); process_block(in,out,B,reset); }
  check(reset.peak<1.0e-4f,"resume does not return to rest");
  std::puts("PASS suspend/resume rest");
  std::printf("ALL COMMON MODFX HOST TESTS PASSED: %s (hardware CPU/tone not certified)\n",name);
}
