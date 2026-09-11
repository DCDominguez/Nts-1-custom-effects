#include "stubs/userrevfx.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include <chrono>

void REVFX_INIT(uint32_t,uint32_t);
void REVFX_PROCESS(float*,uint32_t);
void REVFX_SUSPEND(void);
void REVFX_RESUME(void);
void REVFX_PARAM(uint8_t,int32_t);

float host_test_bpm=120.0f; static uint32_t rng_state=0xabcdef01u;
static void die(const char*m){std::fprintf(stderr,"FAIL: %s\n",m);std::exit(1);} static void check(bool x,const char*m){if(!x)die(m);} 
static int32_t q(float x){x=std::max(0.0f,std::min(1.0f,x));return static_cast<int32_t>(x*2147483000.0f);} 
static float noise(float a=.4f){rng_state=rng_state*1664525u+1013904223u;return a*((int32_t)(rng_state>>8)/8388608.0f-1.0f);} 
static float tone(uint64_t n,float hz=220,float a=.4f){return a*std::sin(6.2831853071795864769*hz*(double)n/48000.0);} 
static void setp(float t,float d,float m){REVFX_PARAM(k_user_revfx_param_time,q(t));REVFX_PARAM(k_user_revfx_param_depth,q(d));REVFX_PARAM(k_user_revfx_param_shift_depth,q(m));}
struct Stats{float peak=0;uint64_t clip_hits=0,frames=0;double abs=0;};
static void process(float*b,uint32_t n,Stats&s){REVFX_PROCESS(b,n);for(uint32_t i=0;i<2*n;++i){check(std::isfinite(b[i]),"non-finite output");float a=std::fabs(b[i]);s.peak=std::max(s.peak,a);s.abs+=a;if(a>=.9995f)++s.clip_hits;check(a<8.0f,"unbounded output");}s.frames+=n;}
static void settle(float t,float d,float m,uint32_t frames=144000){setp(t,d,m);float b[128]={0};Stats s;for(uint32_t n=0;n<frames;n+=64)process(b,64,s);}

int main(int argc,char**argv){const char*name=argc>1?argv[1]:"revfx";constexpr uint32_t B=64;float b[B*2];
 REVFX_INIT(0,0);REVFX_RESUME();settle(.5f,.5f,0);double dryerr=0;uint64_t n=0;Stats dry;
 for(uint32_t block=0;block<375;++block){for(uint32_t i=0;i<B;++i,++n){float l=tone(n,220,.4f),r=.77f*tone(n,311.13f,.4f);b[2*i]=l;b[2*i+1]=r;}float orig[B*2];std::copy(b,b+B*2,orig);process(b,B,dry);for(uint32_t i=0;i<B*2;++i)dryerr+=std::fabs(b[i]-orig[i]);}
 dryerr/=double(375*B*2);check(dryerr<2.0e-3,"MIX=0 does not preserve dry path after settling");std::printf("PASS dry path mean error %.8f\n",dryerr);

 REVFX_INIT(0,0);REVFX_RESUME();settle(.85f,.75f,1);Stats imp;std::fill(b,b+B*2,0.0f);b[0]=b[1]=.6f;process(b,B,imp);double e1=0,e2=0,e3=0;const uint32_t total=30*48000/B;
 for(uint32_t block=0;block<total;++block){std::fill(b,b+B*2,0.0f);process(b,B,imp);double e=0;for(float x:b)e+=std::fabs(x);if(block>=48000/B&&block<6*48000/B)e1+=e;if(block>=12*48000/B&&block<17*48000/B)e2+=e;if(block>=25*48000/B)e3+=e;}
 std::printf("INFO impulse/tail metrics: E1 %.9f E2 %.9f E3 %.9f ratios %.9f %.9f peak %.6f\n",e1,e2,e3,e1>0?e2/e1:0.0,e2>0?e3/e2:0.0,imp.peak);
 check(e1>1.0e-4,"impulse fails to excite reverb tail");
 check(e3<e1*.45+1.0e-4,"reverb tail does not decay enough by 25-30 s");
 check(e3<e2*.9+1.0e-4,"late reverb energy is not trending toward rest");
 std::puts("PASS impulse/tail decay");

 REVFX_INIT(0,0);REVFX_RESUME();Stats normal;n=0;setp(.9f,.9f,.8f);for(uint32_t block=0;block<3000;++block){for(uint32_t i=0;i<B;++i,++n){float x=.55f*tone(n,110,.8f)+.2f*noise(.5f);x=std::max(-.8f,std::min(.8f,x));b[2*i]=x;b[2*i+1]=-.55f*x+.1f*noise(.4f);}process(b,B,normal);}std::printf("INFO normal headroom: peak %.6f clip hits %llu\n",normal.peak,(unsigned long long)normal.clip_hits);check(normal.clip_hits==0,"normal material reaches hard full-scale clamp");std::puts("PASS normal headroom");

 REVFX_INIT(0,0);REVFX_RESUME();Stats sweep;n=0;const auto t0=std::chrono::steady_clock::now();for(uint32_t block=0;block<18000;++block){float p=(block%300)/299.0f;if((block/300)&1u)p=1-p;setp(p,1-p*.72f,(block%997)/996.0f);for(uint32_t i=0;i<B;++i,++n){float x=(block%13==0)?noise(.5f):tone(n,60+600*p,.45f);b[2*i]=x;b[2*i+1]=(block&1)?-.7f*x:.7f*x;}process(b,B,sweep);}const auto us=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-t0).count();std::printf("PASS sweep/wrap soak; peak %.6f host %.3f us/block\n",sweep.peak,double(us)/18000.0);
 REVFX_SUSPEND();REVFX_RESUME();setp(.8f,.8f,1);Stats rest;for(uint32_t block=0;block<750;++block){std::fill(b,b+B*2,0.0f);process(b,B,rest);}check(rest.peak<1.0e-4f,"reset silence creates output");std::puts("PASS reset/rest");std::printf("ALL COMMON REVFX HOST TESTS PASSED: %s (hardware timing/tone not certified)\n",name);
}
