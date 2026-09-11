#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "tests/pre-handoff/common/measure.hpp"
#include "effects/capstan/nts1/src/capstan.cpp"
static void req(bool ok,const char*m){if(!ok){std::fprintf(stderr,"CAPSTAN FAIL: %s\n",m);std::exit(2);}}
static int32_t q(float x){x=std::max(0.f,std::min(.999999f,x));return(int32_t)(x*2147483647.f);}static void set(float a,float b){MODFX_PARAM(k_user_modfx_param_time,q(a));MODFX_PARAM(k_user_modfx_param_depth,q(b));}
static void settle(float a,float b,int blocks=180){set(a,b);std::vector<float>z(128),o(128),s(128),sy(128);for(int i=0;i<blocks;i++)MODFX_PROCESS(z.data(),o.data(),s.data(),sy.data(),64);}static std::vector<float>run(std::size_t n,double hz,float amp=.45f){std::vector<float>in(n*2),out(n*2),s(n*2),sy(n*2);for(std::size_t i=0;i<n;i++){float v=amp*std::sin(2.0*hs_measure::kPi*hz*i/48000.0);in[2*i]=in[2*i+1]=v;}std::size_t p=0;while(p<n){uint32_t k=(uint32_t)std::min<std::size_t>(64,n-p);MODFX_PROCESS(in.data()+2*p,out.data()+2*p,s.data()+2*p,sy.data()+2*p,k);p+=k;}return out;}
static double adjacent_freq_motion(const std::vector<float>&x){auto l=hs_measure::channel(x,0);std::vector<double> f;const std::size_t w=6000;for(std::size_t p=12000;p+w<=l.size();p+=w){double hz=hs_measure::zero_cross_frequency(l,48000,p,w);if(hz>0)f.push_back(hz);}double e=0;for(std::size_t i=1;i<f.size();i++)e+=std::fabs(f[i]-f[i-1]);return f.size()>1?e/(f.size()-1):0;}
int main(){
 // WEAR=0 is a true dry contract.
 MODFX_INIT(0,0);settle(.8f,0.f);auto dry=run(32000,997);std::vector<float>ref(32000);for(std::size_t i=0;i<ref.size();i++)ref[i]=.45f*std::sin(2.0*hs_measure::kPi*997.0*i/48000.0);req(hs_measure::max_abs_diff(hs_measure::channel(dry,0),ref)<2e-6,"WEAR=0 is not dry");
 // WEAR introduces the delayed/saturated tape path.
 MODFX_INIT(0,0);settle(.5f,1.f);auto worn=run(120000,997);req(hs_measure::max_abs_diff(hs_measure::channel(worn,0),ref)>.01,"WEAR does not produce material tape processing");auto wl=hs_measure::channel(worn,0),wr=hs_measure::channel(worn,1);req(hs_measure::max_abs_diff(wl,wr)>1e-4,"wear path lacks stereo delay offset");
 // MOTION changes wow/flutter speed at high WEAR.
 MODFX_INIT(0,0);settle(.05f,.9f);auto slow=run(300000,440);double sm=adjacent_freq_motion(slow);MODFX_INIT(0,0);settle(.95f,.9f);auto fast=run(300000,440);double fm=adjacent_freq_motion(fast);req(fm>sm*1.05+1e-4,"MOTION does not measurably change pitch-motion rate");
 // A fresh initialization should reproduce the seeded stochastic wear trajectory.
 MODFX_INIT(0,0);settle(.7f,1.f);auto a=run(120000,330);MODFX_INIT(0,0);settle(.7f,1.f);auto b=run(120000,330);req(hs_measure::max_abs_diff(a,b)<1e-6,"initialization does not reproduce stochastic wear trajectory");
 req(hs_measure::stats(wl).peak<=1.001&&hs_measure::stats(wr).peak<=1.001,"wear output exceeded bounds");
 // Suspend/resume clears delay history.
 MODFX_SUSPEND();MODFX_RESUME();std::vector<float>z(12000*2),o(12000*2),s(12000*2),sy(12000*2);MODFX_PROCESS(z.data(),o.data(),s.data(),sy.data(),12000);req(hs_measure::stats(hs_measure::channel(o,0)).peak<1e-7,"reset emitted stale delay history");
 // Long stochastic/control soak.
 MODFX_INIT(0,0);std::vector<float>in(128),out(128),sub(128),suby(128);for(int bb=0;bb<12000;bb++){set(((bb*23)%101)/100.f,((bb*71)%101)/100.f);for(int i=0;i<64;i++){float v=.8f*std::sin(2.0*hs_measure::kPi*(79+(bb%17)*71.0)*(bb*64+i)/48000.0);in[2*i]=v;in[2*i+1]=.88f*v;}MODFX_PROCESS(in.data(),out.data(),sub.data(),suby.data(),64);for(float v:out){req(std::isfinite(v),"non-finite soak output");req(std::fabs(v)<=1.001,"soak output exceeded bounds");}}
 std::puts("CAPSTAN A-class project-specific harness PASS");return 0;}
