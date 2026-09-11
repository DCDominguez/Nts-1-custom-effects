#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "tests/pre-handoff/common/measure.hpp"
#include "effects/sideband/nts1/src/sideband.cpp"
static void req(bool ok,const char*m){if(!ok){std::fprintf(stderr,"SIDEBAND FAIL: %s\n",m);std::exit(2);}}
static int32_t q(float x){x=std::max(0.f,std::min(.999999f,x));return(int32_t)(x*2147483647.f);}static void set(float a,float b){MODFX_PARAM(k_user_modfx_param_time,q(a));MODFX_PARAM(k_user_modfx_param_depth,q(b));}
static void settle(float a,float b,int blocks=180){set(a,b);std::vector<float>z(128),o(128),s(128),sy(128);for(int i=0;i<blocks;i++)MODFX_PROCESS(z.data(),o.data(),s.data(),sy.data(),64);}static std::vector<float>run(std::size_t n,double hz,float amp=.45f){std::vector<float>in(n*2),out(n*2),s(n*2),sy(n*2);for(std::size_t i=0;i<n;i++){float v=amp*std::sin(2.0*hs_measure::kPi*hz*i/48000.0);in[2*i]=in[2*i+1]=v;}std::size_t p=0;while(p<n){uint32_t k=(uint32_t)std::min<std::size_t>(64,n-p);MODFX_PROCESS(in.data()+2*p,out.data()+2*p,s.data()+2*p,sy.data()+2*p,k);p+=k;}return out;}
int main(){
 // DEPTH=0 is the neutral Hilbert-aligned path: after startup it should preserve tone energy and stereo equality.
 MODFX_INIT(0,0);settle(.3f,0.f);auto neutral=run(96000,1000);auto nl=hs_measure::channel(neutral,0),nr=hs_measure::channel(neutral,1);req(hs_measure::stats(nl,4096).rms>.20,"neutral path lost source energy");req(hs_measure::max_abs_diff(nl,nr)<2e-6,"neutral mono input diverged in stereo");
 // Full DEPTH exposes upward shift on left and downward shift on right.
 const float time=.22f;const double shift=.2+4800.0*time*time;MODFX_INIT(0,0);settle(time,1.f);auto shifted=run(160000,1200);auto l=hs_measure::channel(shifted,0),r=hs_measure::channel(shifted,1);double lup=hs_measure::tone_power(l,1200.0+shift,48000,8192),ldn=hs_measure::tone_power(l,1200.0-shift,48000,8192);double rup=hs_measure::tone_power(r,1200.0+shift,48000,8192),rdn=hs_measure::tone_power(r,1200.0-shift,48000,8192);req(lup>ldn*1.15,"left channel does not favor upward sideband");req(rdn>rup*1.15,"right channel does not favor downward sideband at full divergence");
 // TIME must increase translation amount.
 MODFX_INIT(0,0);settle(.10f,1.f);auto low=hs_measure::channel(run(120000,1000),0);double lowhz=hs_measure::zero_cross_frequency(low,48000,8192,90000);MODFX_INIT(0,0);settle(.35f,1.f);auto high=hs_measure::channel(run(120000,1000),0);double highhz=hs_measure::zero_cross_frequency(high,48000,8192,90000);req(highhz>lowhz+250.0,"SHIFT/TIME does not materially increase translated frequency");
 req(hs_measure::stats(l).peak<=1.001&&hs_measure::stats(r).peak<=1.001,"shift output exceeded bounds");
 // Reset clears Hilbert history.
 MODFX_SUSPEND();MODFX_RESUME();std::vector<float>z(2048*2),o(2048*2),s(2048*2),sy(2048*2);MODFX_PROCESS(z.data(),o.data(),s.data(),sy.data(),2048);req(hs_measure::stats(hs_measure::channel(o,0)).peak<1e-7,"reset emitted stale Hilbert history");
 // Control and frequency soak.
 MODFX_INIT(0,0);std::vector<float>in(128),out(128),sub(128),suby(128);for(int b=0;b<9000;b++){set(((b*29)%101)/100.f,((b*61)%101)/100.f);for(int i=0;i<64;i++){float v=.75f*std::sin(2.0*hs_measure::kPi*(173+(b%11)*211.0)*(b*64+i)/48000.0);in[2*i]=v;in[2*i+1]=.9f*v;}MODFX_PROCESS(in.data(),out.data(),sub.data(),suby.data(),64);for(float v:out){req(std::isfinite(v),"non-finite soak output");req(std::fabs(v)<=1.001,"soak output exceeded bounds");}}
 std::puts("SIDEBAND A-class project-specific harness PASS");return 0;}
