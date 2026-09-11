#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "tests/pre-handoff/common/measure.hpp"
#include "effects/ascender/nts1/src/ascender.cpp"
static void req(bool ok,const char*m){if(!ok){std::fprintf(stderr,"ASCENDER FAIL: %s\n",m);std::exit(2);}}
static int32_t q(float x){x=std::max(0.f,std::min(.999999f,x));return(int32_t)(x*2147483647.f);}static void set(float a,float b){MODFX_PARAM(k_user_modfx_param_time,q(a));MODFX_PARAM(k_user_modfx_param_depth,q(b));}
static void settle(float a,float b,int blocks=180){set(a,b);std::vector<float>z(128),o(128),s(128),sy(128);for(int i=0;i<blocks;i++)MODFX_PROCESS(z.data(),o.data(),s.data(),sy.data(),64);}static std::vector<float>run(std::size_t n,double hz,float amp=.45f){std::vector<float>in(n*2),out(n*2),s(n*2),sy(n*2);for(std::size_t i=0;i<n;i++){float v=amp*std::sin(2.0*hs_measure::kPi*hz*i/48000.0);in[2*i]=in[2*i+1]=v;}std::size_t p=0;while(p<n){uint32_t k=(uint32_t)std::min<std::size_t>(64,n-p);MODFX_PROCESS(in.data()+2*p,out.data()+2*p,s.data()+2*p,sy.data()+2*p,k);p+=k;}return out;}
static double stereo_motion(const std::vector<float>&x){auto l=hs_measure::channel(x,0),r=hs_measure::channel(x,1);double e=0;for(std::size_t i=1;i<l.size();++i){double d=(l[i]-r[i])-(l[i-1]-r[i-1]);e+=std::fabs(d);}return e/std::max<std::size_t>(1,l.size()-1);}
int main(){
 MODFX_INIT(0,0);settle(.2f,.1f);auto mild=run(96000,777);req(hs_measure::stats(hs_measure::channel(mild,0)).rms>.05,"mild patch lost audio");
 MODFX_INIT(0,0);settle(.4f,.05f);auto low=run(120000,650);MODFX_INIT(0,0);settle(.4f,1.f);auto high=run(120000,650);req(hs_measure::max_abs_diff(low,high)>.01,"HEIGHT/DEPTH does not materially change barber-pole output");
 MODFX_INIT(0,0);settle(.05f,.9f);auto slow=run(240000,500);double sm=stereo_motion(slow);MODFX_INIT(0,0);settle(.95f,.9f);auto fast=run(240000,500);double fm=stereo_motion(fast);req(fm>sm*1.15+1e-8,"CLIMB/RATE does not increase motion");
 // Barber-pole wraps should not create isolated discontinuity spikes beyond the normal signal slope.
 auto l=hs_measure::channel(fast,0);double rms=hs_measure::stats(l).rms,maxjump=0;for(std::size_t i=1;i<l.size();i++)maxjump=std::max(maxjump,std::fabs((double)l[i]-l[i-1]));req(maxjump<1.2,"wrap produced full-scale discontinuity");req(rms>.03,"barber-pole output collapsed");
 auto r=hs_measure::channel(fast,1);req(hs_measure::max_abs_diff(l,r)>1e-4,"stereo offset absent");req(hs_measure::stats(l).peak<=1.001&&hs_measure::stats(r).peak<=1.001,"output exceeded bounds");
 MODFX_SUSPEND();MODFX_RESUME();std::vector<float>z(16384*2),o(16384*2),s(16384*2),sy(16384*2);MODFX_PROCESS(z.data(),o.data(),s.data(),sy.data(),16384);req(hs_measure::stats(hs_measure::channel(o,0)).peak<1e-7,"reset left residual state");
 MODFX_INIT(0,0);std::vector<float>in(128),out(128),sub(128),suby(128);for(int b=0;b<8000;b++){set(((b*23)%101)/100.f,((b*59)%101)/100.f);for(int i=0;i<64;i++){float v=.78f*std::sin(2.0*hs_measure::kPi*(71+(b%19)*91.0)*(b*64+i)/48000.0);in[2*i]=v;in[2*i+1]=.8f*v;}MODFX_PROCESS(in.data(),out.data(),sub.data(),suby.data(),64);for(float v:out){req(std::isfinite(v),"non-finite soak output");req(std::fabs(v)<=1.001,"soak output exceeded bounds");}}
 std::puts("ASCENDER A-class project-specific harness PASS");return 0;}
