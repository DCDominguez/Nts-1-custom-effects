#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "tests/pre-handoff/common/measure.hpp"
#include "effects/glitchrepeat/nts1/src/glitchrepeat.cpp"
static void req(bool x,const char*m){if(!x){std::fprintf(stderr,"GLITCHREPEAT FAIL: %s\n",m);std::exit(2);}}static int32_t q(float x){x=std::max(0.f,std::min(.999999f,x));return(int32_t)(x*2147483647.f);}static void set(float sl,float ch,float mix){DELFX_PARAM(k_user_delfx_param_time,q(sl));DELFX_PARAM(k_user_delfx_param_depth,q(ch));DELFX_PARAM(k_user_delfx_param_shift_depth,q(mix));}static void settle(float a,float b,float m,int n=260){set(a,b,m);std::vector<float>z(128);for(int i=0;i<n;i++)DELFX_PROCESS(z.data(),64);}static void proc(std::vector<float>&x){std::size_t p=0,n=x.size()/2;while(p<n){uint32_t k=(uint32_t)std::min<std::size_t>(64,n-p);DELFX_PROCESS(x.data()+2*p,k);p+=k;}}static std::vector<float>signal(std::size_t n){std::vector<float>x(n*2);for(std::size_t i=0;i<n;i++){float v=.25f*std::sin(2*hs_measure::kPi*(220.0+((i/1000)%5)*73.0)*i/48000.0)+.000002f*(float)(i%10000);x[2*i]=v;x[2*i+1]=.9f*v;}return x;}
int main(){
 // CHANCE=0 cannot arm repeats; full MIX therefore remains source-through.
 DELFX_INIT(0,0);test_bpm=120;settle(.2f,0,1);auto dry=signal(60000),ref=dry;proc(dry);req(repeats==0,"CHANCE=0 armed repeat state");req(hs_measure::max_abs_diff(dry,ref)<2e-6,"CHANCE=0 full-mix path altered source");
 // CHANCE=1 must arm on the beat, deliver buffered material, and then expire after finite repeats.
 DELFX_INIT(0,0);test_bpm=120;settle(.2f,1,1);auto wet=signal(60000),wetRef=wet;proc(wet);req(hs_measure::max_abs_diff(wet,wetRef)>.01,"repeat state did not reach audible output");req(repeats==0,"repeat state failed to expire by end of test");req(loopStart==0,"expired repeat retained loop ownership");
 // SLICE controls loop length materially.
 DELFX_INIT(0,0);test_bpm=120;settle(.1f,1,1);auto s=signal(25000);proc(s);uint32_t shortLen=loopLen;DELFX_INIT(0,0);test_bpm=120;settle(.9f,1,1);auto l=signal(25000);proc(l);uint32_t longLen=loopLen;req(longLen>shortLen*8,"SLICE does not materially increase repeat window");
 // BPM controls arming boundary: at 240 BPM a repeat is active by 12k samples; at 120 it is not yet.
 DELFX_INIT(0,0);test_bpm=120;settle(.2f,1,1);auto pre=signal(14000);proc(pre);bool active120=repeats>0;DELFX_INIT(0,0);test_bpm=240;settle(.2f,1,1);auto fast=signal(14000);proc(fast);bool active240=repeats>0;req(!active120&&active240,"tempo does not move repeat-arm boundary");
 // Low BPM fallback is 120.
 DELFX_INIT(0,0);test_bpm=0;settle(.2f,1,1);auto fb=signal(14000);proc(fb);req(repeats==0,"BPM fallback did not behave like 120 BPM before first beat");
 for(float v:wet){req(std::isfinite(v),"non-finite repeat output");req(std::fabs(v)<=1.001,"repeat output exceeded bounds");}
 DELFX_SUSPEND();DELFX_RESUME();req(repeats==0&&loopStart==0&&loopPos==0,"lifecycle reset did not clear repeat ownership");std::vector<float>z(12000*2);proc(z);req(hs_measure::stats(hs_measure::channel(z,0)).peak<1e-7,"reset emitted stale buffer audio");
 std::puts("GLITCHREPEAT A-class project-specific harness PASS");return 0;}
