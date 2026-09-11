#include "stubs/userdelfx.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include <chrono>
#include <cstring>

void DELFX_INIT(uint32_t,uint32_t);
void DELFX_PROCESS(float*,uint32_t);
void DELFX_SUSPEND(void);
void DELFX_RESUME(void);
void DELFX_PARAM(uint8_t,int32_t);

float host_test_bpm = 120.0f;
static uint32_t rng_state=0x27182818u;
static void die(const char*m){std::fprintf(stderr,"FAIL: %s\n",m);std::exit(1);} static void check(bool x,const char*m){if(!x)die(m);} 
static int32_t q(float x){x=std::max(0.0f,std::min(1.0f,x));return static_cast<int32_t>(x*2147483000.0f);} 
static float tone(uint64_t n,float hz=220,float a=.4f){return a*std::sin(6.2831853071795864769*hz*(double)n/48000.0);} 
static float noise(float a=.4f){rng_state=rng_state*1664525u+1013904223u;return a*((int32_t)(rng_state>>8)/8388608.0f-1.0f);} 
static void setp(float t,float d,float m){DELFX_PARAM(k_user_delfx_param_time,q(t));DELFX_PARAM(k_user_delfx_param_depth,q(d));DELFX_PARAM(k_user_delfx_param_shift_depth,q(m));}
struct Stats{double abs=0;float peak=0;uint64_t frames=0;};
static void process(float*b,uint32_t n,Stats&s){DELFX_PROCESS(b,n);for(uint32_t i=0;i<2*n;++i){check(std::isfinite(b[i]),"non-finite output");s.peak=std::max(s.peak,std::fabs(b[i]));check(std::fabs(b[i])<8.0f,"unbounded output");s.abs+=std::fabs(b[i]);}s.frames+=n;}
static void settle(float t,float d,float m,uint32_t frames=144000){setp(t,d,m);float b[128]={0};Stats s;for(uint32_t n=0;n<frames;n+=64)process(b,64,s);}
static double abs_block(const float*b){double e=0;for(uint32_t i=0;i<128;++i)e+=std::fabs(b[i]);return e;}

int main(int argc,char**argv){
 const char*name=argc>1?argv[1]:"delfx"; const bool finite=(argc>2 && std::strcmp(argv[2],"finite")==0); constexpr uint32_t B=64; float b[B*2];
 DELFX_INIT(0,0);DELFX_RESUME();settle(.5f,.5f,0);double dryerr=0;uint64_t n=0;Stats dry;
 for(uint32_t block=0;block<375;++block){for(uint32_t i=0;i<B;++i,++n){float l=tone(n,220,.4f),r=tone(n,329.63f,.31f);b[2*i]=l;b[2*i+1]=r;}float orig[B*2];std::copy(b,b+B*2,orig);process(b,B,dry);for(uint32_t i=0;i<B*2;++i)dryerr+=std::fabs(b[i]-orig[i]);}
 dryerr/=double(375*B*2);check(dryerr<2.0e-3,"MIX=0 does not preserve dry path after settling");std::printf("PASS dry path mean error %.8f\n",dryerr);

 // First probe a literal impulse. Capture/granular delays are allowed to require
 // a short fragment instead, but conventional delays should answer the impulse.
 bool impulseHeard=false;
 {
   DELFX_INIT(0,0);DELFX_RESUME();settle(.55f,.55f,1);Stats s;std::fill(b,b+B*2,0.0f);b[0]=b[1]=.7f;process(b,B,s);
   for(uint32_t block=0;block<8*48000/B;++block){std::fill(b,b+B*2,0.0f);process(b,B,s);if(abs_block(b)>1.0e-5)impulseHeard=true;}
 }
 std::printf("INFO literal impulse response: %s\n",impulseHeard?"present":"not detected; short-fragment probe required");

 // A 40 ms tone burst is long enough to exercise fragment-capture delays while
 // remaining a tightly bounded excitation for conventional feedback delays.
 bool heard=false; double bestEarly=0,bestLate=0; const float vals[3]={.15f,.55f,.95f};
 for(float t:vals)for(float d:vals){
   DELFX_INIT(0,0);DELFX_RESUME();settle(t,d,1);Stats s;n=0;
   for(uint32_t f=0;f<1920;f+=B){
     for(uint32_t i=0;i<B;++i,++n){float x=tone(n,220,.55f);b[2*i]=x;b[2*i+1]=.77f*x;}
     process(b,B,s);
   }
   double early=0,late=0;const uint32_t total=45*48000/B;
   for(uint32_t block=0;block<total;++block){
     std::fill(b,b+B*2,0.0f);process(b,B,s);const double e=abs_block(b);
     if(block<8*48000/B)early+=e;
     if(block>=40*48000/B)late+=e;
   }
   if(early>1.0e-3)heard=true;
   if(early>bestEarly){bestEarly=early;bestLate=late;}
 }
 std::printf("INFO strongest burst tail: early %.9f late %.9f ratio %.9f\n",bestEarly,bestLate,bestEarly>0?bestLate/bestEarly:0.0);
 check(heard,"no delayed/tail response detected from impulse or short fragment across parameter grid");
 if(finite) check(bestLate < std::max(1.0e-5,bestEarly*0.03),"finite delay does not return near rest after 40 s");
 else check(bestLate < std::max(1.0e-4,bestEarly*1.2),"tail energy grows instead of remaining bounded");
 std::puts("PASS excitation delivery/tail bound");

 DELFX_INIT(0,0);DELFX_RESUME();Stats sweep;n=0;const auto t0=std::chrono::steady_clock::now();
 for(uint32_t block=0;block<18000;++block){float p=(block%256)/255.0f;if((block/256)&1u)p=1-p;host_test_bpm=(block%7000<3500)?72.0f:155.0f;setp(p,1.0f-p*.61f,(block%1024)/1023.0f);for(uint32_t i=0;i<B;++i,++n){float x=(block%11==0)?noise(.65f):tone(n,70+500*p,.55f);b[2*i]=x;b[2*i+1]=(block&1)?-.63f*x:.63f*x;}process(b,B,sweep);}
 const auto us=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-t0).count();std::printf("PASS sweep/wrap/tempo soak; peak %.6f host %.3f us/block\n",sweep.peak,double(us)/18000.0);

 DELFX_SUSPEND();DELFX_RESUME();setp(.5f,.5f,1);Stats rest;for(uint32_t block=0;block<750;++block){std::fill(b,b+B*2,0.0f);process(b,B,rest);}check(rest.peak<1.0e-4f,"reset silence creates output");
 host_test_bpm=120.0f;std::puts("PASS reset/silence");std::printf("ALL COMMON DELFX HOST TESTS PASSED: %s (hardware timing/tone not certified)\n",name);
}
