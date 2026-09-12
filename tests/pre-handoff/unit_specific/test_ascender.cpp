#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "tests/pre-handoff/common/measure.hpp"
#include "effects/ascender/nts1/src/ascender.cpp"

static void req(bool ok,const char*m){if(!ok){std::fprintf(stderr,"ASCENDER FAIL: %s\n",m);std::exit(2);}}
static int32_t q(float x){x=std::max(0.f,std::min(.999999f,x));return(int32_t)(x*2147483647.f);}
static void set(float a,float b){MODFX_PARAM(k_user_modfx_param_time,q(a));MODFX_PARAM(k_user_modfx_param_depth,q(b));}
static void settle(float a,float b,int blocks=180){set(a,b);std::vector<float>z(128),o(128),s(128),sy(128);for(int i=0;i<blocks;i++)MODFX_PROCESS(z.data(),o.data(),s.data(),sy.data(),64);}
static std::vector<float>run(std::size_t n,double hz,float amp=.45f){std::vector<float>in(n*2),out(n*2),s(n*2),sy(n*2);for(std::size_t i=0;i<n;i++){float v=amp*std::sin(2.0*hs_measure::kPi*hz*i/48000.0);in[2*i]=in[2*i+1]=v;}std::size_t p=0;while(p<n){uint32_t k=(uint32_t)std::min<std::size_t>(64,n-p);MODFX_PROCESS(in.data()+2*p,out.data()+2*p,s.data()+2*p,sy.data()+2*p,k);p+=k;}return out;}
static float phase_advance(float rate){MODFX_INIT(0,0);settle(rate,.9f,300);float start=ph;std::vector<float>z(12000*2),o(12000*2),s(12000*2),sy(12000*2);MODFX_PROCESS(z.data(),o.data(),s.data(),sy.data(),12000);float dph=ph-start;if(dph<0)dph+=1.f;return dph;}
static double rms_envelope_span(const std::vector<float>& stereo,std::size_t block=1024){auto l=hs_measure::channel(stereo,0);double lo=1e9,hi=0;for(std::size_t p=0;p+block<=l.size();p+=block){double e=0;for(std::size_t i=0;i<block;++i)e+=(double)l[p+i]*l[p+i];double r=std::sqrt(e/(double)block);lo=std::min(lo,r);hi=std::max(hi,r);}return hi-lo;}

int main(){
 MODFX_INIT(0,0);settle(.2f,.1f);auto mild=run(96000,777);req(hs_measure::stats(hs_measure::channel(mild,0)).rms>.05,"mild patch lost audio");

 MODFX_INIT(0,0);settle(.4f,.05f);auto low=run(120000,650);
 MODFX_INIT(0,0);settle(.4f,1.f);auto high=run(120000,650);
 req(hs_measure::max_abs_diff(low,high)>.01,"HEIGHT/DEPTH does not materially change barber-pole output");
 // Perceptual contract added after physical MkI testing: at high DEPTH a sustained
 // tone must develop a clearly moving notch/envelope field, not merely a numerically
 // different all-pass output. The previous 100%-wet voice failed this listening test.
 const double lowSpan=rms_envelope_span(low),highSpan=rms_envelope_span(high);
 req(highSpan>0.035,"high DEPTH barber-pole movement is still too subtle");
 req(highSpan>lowSpan*4.0,"DEPTH does not materially increase audible climb motion");

 const float slowAdv=phase_advance(.05f),fastAdv=phase_advance(.95f);req(fastAdv>slowAdv*5.f,"CLIMB/RATE does not materially increase barber-pole phase advance");
 MODFX_INIT(0,0);settle(.95f,.9f);auto fast=run(240000,500);auto l=hs_measure::channel(fast,0);double rms=hs_measure::stats(l).rms,maxjump=0;for(std::size_t i=1;i<l.size();i++)maxjump=std::max(maxjump,std::fabs((double)l[i]-l[i-1]));req(maxjump<1.2,"wrap produced full-scale discontinuity");req(rms>.03,"barber-pole output collapsed");
 auto rr=hs_measure::channel(fast,1);req(hs_measure::max_abs_diff(l,rr)>1e-4,"stereo offset absent");req(hs_measure::stats(l).peak<=1.001&&hs_measure::stats(rr).peak<=1.001,"output exceeded bounds");
 MODFX_SUSPEND();MODFX_RESUME();std::vector<float>z(16384*2),o(16384*2),s(16384*2),sy(16384*2);MODFX_PROCESS(z.data(),o.data(),s.data(),sy.data(),16384);req(hs_measure::stats(hs_measure::channel(o,0)).peak<1e-7,"reset left residual state");
 MODFX_INIT(0,0);std::vector<float>in(128),out(128),sub(128),suby(128);for(int b=0;b<8000;b++){set(((b*23)%101)/100.f,((b*59)%101)/100.f);for(int i=0;i<64;i++){float v=.78f*std::sin(2.0*hs_measure::kPi*(71+(b%19)*91.0)*(b*64+i)/48000.0);in[2*i]=v;in[2*i+1]=.8f*v;}MODFX_PROCESS(in.data(),out.data(),sub.data(),suby.data(),64);for(float v:out){req(std::isfinite(v),"non-finite soak output");req(std::fabs(v)<=1.001,"soak output exceeded bounds");}}
 std::puts("ASCENDER 0.2-0 A-class project-specific harness PASS (physical MkI retest required)");return 0;
}
