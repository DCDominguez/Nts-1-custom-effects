#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "tests/pre-handoff/common/measure.hpp"
#include "effects/faultline/nts1/src/faultline.cpp"
static void req(bool ok,const char*m){if(!ok){std::fprintf(stderr,"FAULTLINE FAIL: %s\n",m);std::exit(2);}}
static int32_t q(float x){x=std::max(0.f,std::min(.999999f,x));return(int32_t)(x*2147483647.f);}static void set(float a,float b){MODFX_PARAM(k_user_modfx_param_time,q(a));MODFX_PARAM(k_user_modfx_param_depth,q(b));}
static void settle(float a,float b,int blocks=220){set(a,b);std::vector<float>z(128),o(128),s(128),sy(128);for(int i=0;i<blocks;i++)MODFX_PROCESS(z.data(),o.data(),s.data(),sy.data(),64);}static std::vector<float>run(std::size_t n,double hz,float amp=.35f){std::vector<float>in(n*2),out(n*2),s(n*2),sy(n*2);for(std::size_t i=0;i<n;i++){float v=amp*std::sin(2.0*hs_measure::kPi*hz*i/48000.0);in[2*i]=in[2*i+1]=v;}std::size_t p=0;while(p<n){uint32_t k=(uint32_t)std::min<std::size_t>(64,n-p);MODFX_PROCESS(in.data()+2*p,out.data()+2*p,s.data()+2*p,sy.data()+2*p,k);p+=k;}return out;}
int main(){
 // CUTOFF must materially alter high-frequency transmission.
 MODFX_INIT(0,0);settle(.08f,0.f);auto closed=hs_measure::channel(run(120000,5000),0);double pc=hs_measure::tone_power(closed,5000,48000,8192);MODFX_INIT(0,0);settle(.92f,0.f);auto open=hs_measure::channel(run(120000,5000),0);double po=hs_measure::tone_power(open,5000,48000,8192);req(po>pc*4.0+1e-8,"CUTOFF does not materially change high-frequency transmission");
 // FAULT depth must increase nonlinear harmonic content on a simple tone.
 MODFX_INIT(0,0);settle(.65f,.05f);auto clean=hs_measure::channel(run(120000,500,.25f),0);double c1=hs_measure::tone_power(clean,500,48000,8192),c3=hs_measure::tone_power(clean,1500,48000,8192);MODFX_INIT(0,0);settle(.65f,1.f);auto fault=hs_measure::channel(run(120000,500,.25f),0);double f1=hs_measure::tone_power(fault,500,48000,8192),f3=hs_measure::tone_power(fault,1500,48000,8192);req(f1>1e-6&&c1>1e-6&&f3/f1>(c3/c1)*1.4+1e-5,"FAULT does not measurably increase nonlinear character");
 // Fresh init must reproduce the seeded instability trajectory.
 MODFX_INIT(0,0);settle(.55f,.9f);auto a=run(96000,317,.4f);MODFX_INIT(0,0);settle(.55f,.9f);auto b=run(96000,317,.4f);req(hs_measure::max_abs_diff(a,b)<1e-6,"initialization does not reproduce seeded instability trajectory");
 // Hot input and high fault remain finite/bounded and recover to silence after reset.
 MODFX_INIT(0,0);settle(1.f,1.f);auto hot=run(180000,911,.95f);auto hl=hs_measure::channel(hot,0);auto hs=hs_measure::stats(hl);req(hs.rms>.01,"high-fault path collapsed");req(hs.peak<=1.001,"high-fault path exceeded bound");
 MODFX_SUSPEND();MODFX_RESUME();std::vector<float>z(32000*2),o(32000*2),s(32000*2),sy(32000*2);MODFX_PROCESS(z.data(),o.data(),s.data(),sy.data(),32000);req(hs_measure::stats(hs_measure::channel(o,0),8000).peak<1e-5,"reset failed to recover toward silence");
 // Long high-state/control soak.
 MODFX_INIT(0,0);std::vector<float>in(128),out(128),sub(128),suby(128);for(int bb=0;bb<12000;bb++){set(((bb*37)%101)/100.f,((bb*73)%101)/100.f);for(int i=0;i<64;i++){float v=.95f*std::sin(2.0*hs_measure::kPi*(61+(bb%23)*101.0)*(bb*64+i)/48000.0);in[2*i]=v;in[2*i+1]=-.8f*v;}MODFX_PROCESS(in.data(),out.data(),sub.data(),suby.data(),64);for(float v:out){req(std::isfinite(v),"non-finite soak output");req(std::fabs(v)<=1.001,"soak output exceeded bounds");}}
 std::puts("FAULTLINE A-class project-specific harness PASS");return 0;}
