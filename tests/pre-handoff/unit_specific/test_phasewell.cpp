#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "tests/pre-handoff/common/measure.hpp"
#include "effects/phasewell/nts1/src/phasewell.cpp"
static void req(bool ok,const char*m){if(!ok){std::fprintf(stderr,"PHASEWELL FAIL: %s\n",m);std::exit(2);}}
static int32_t q(float x){x=std::max(0.f,std::min(.999999f,x));return(int32_t)(x*2147483647.f);}static void set(float rate,float depth){MODFX_PARAM(k_user_modfx_param_time,q(rate));MODFX_PARAM(k_user_modfx_param_depth,q(depth));}
static void settle(float rate,float depth,int b=180){set(rate,depth);std::vector<float>z(128),o(128),s(128),sy(128);for(int i=0;i<b;i++)MODFX_PROCESS(z.data(),o.data(),s.data(),sy.data(),64);}static std::vector<float> run(std::size_t n,double hz,float amp=.45f){std::vector<float>in(n*2),out(n*2),s(n*2),sy(n*2);for(std::size_t i=0;i<n;i++){float v=amp*std::sin(2.0*hs_measure::kPi*hz*i/48000.0);in[2*i]=in[2*i+1]=v;}std::size_t p=0;while(p<n){uint32_t k=(uint32_t)std::min<std::size_t>(64,n-p);MODFX_PROCESS(in.data()+2*p,out.data()+2*p,s.data()+2*p,sy.data()+2*p,k);p+=k;}return out;}
static double movement(const std::vector<float>&x){auto l=hs_measure::channel(x,0),r=hs_measure::channel(x,1);double e=0;for(std::size_t i=1;i<l.size();++i){double a=(l[i]-r[i])-(l[i-1]-r[i-1]);e+=std::fabs(a);}return e/std::max<std::size_t>(1,l.size()-1);}
int main(){
 MODFX_INIT(0,0);settle(.2f,.1f);auto mild=run(96000,997);auto ml=hs_measure::channel(mild,0),mr=hs_measure::channel(mild,1);req(hs_measure::stats(ml).rms>.05,"mild settings lost audio");req(hs_measure::max_abs_diff(ml,mr)>1e-4,"asymmetric stereo phaser path is absent");
 MODFX_INIT(0,0);settle(.4f,.05f);auto lo=run(120000,700);std::vector<float>ref(120000);for(std::size_t i=0;i<ref.size();i++)ref[i]=.45f*std::sin(2.0*hs_measure::kPi*700.0*i/48000.0);double errlo=hs_measure::max_abs_diff(hs_measure::channel(lo,0),ref);
 MODFX_INIT(0,0);settle(.4f,1.f);auto hi=run(120000,700);double errhi=hs_measure::max_abs_diff(hs_measure::channel(hi,0),ref);req(errhi>errlo*1.15,"DEPTH does not materially strengthen phasewell identity");
 MODFX_INIT(0,0);settle(.05f,.9f);auto slow=run(240000,431);double sm=movement(slow);MODFX_INIT(0,0);settle(.95f,.9f);auto fast=run(240000,431);double fm=movement(fast);req(fm>sm*1.25+1e-8,"RATE does not increase modulation movement");
 auto fl=hs_measure::channel(fast,0),fr=hs_measure::channel(fast,1);req(hs_measure::stats(fl).peak<=1.001&&hs_measure::stats(fr).peak<=1.001,"output exceeded bounds");
 MODFX_SUSPEND();MODFX_RESUME();std::vector<float>z(8192*2),o(8192*2),s(8192*2),sy(8192*2);MODFX_PROCESS(z.data(),o.data(),s.data(),sy.data(),8192);req(hs_measure::stats(hs_measure::channel(o,0)).peak<1e-7,"lifecycle reset left residual state");
 MODFX_INIT(0,0);std::vector<float>in(128),out(128),sub(128),suby(128);for(int b=0;b<7000;b++){set(((b*31)%101)/100.f,((b*53)%101)/100.f);for(int i=0;i<64;i++){float v=.8f*std::sin(2.0*hs_measure::kPi*(89+(b%17)*83.0)*(b*64+i)/48000.0);in[2*i]=v;in[2*i+1]=-.7f*v;}MODFX_PROCESS(in.data(),out.data(),sub.data(),suby.data(),64);for(float v:out){req(std::isfinite(v),"non-finite soak output");req(std::fabs(v)<=1.001,"soak output exceeded bound");}}
 std::puts("PHASEWELL A-class project-specific harness PASS");return 0;}
