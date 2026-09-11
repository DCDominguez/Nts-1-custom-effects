#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "tests/pre-handoff/common/measure.hpp"
#include "effects/aureole/nts1/src/aureole.cpp"
static void req(bool x,const char*m){if(!x){std::fprintf(stderr,"AUREOLE FAIL: %s\n",m);std::exit(2);}}static int32_t q(float x){x=std::max(0.f,std::min(.999999f,x));return(int32_t)(x*2147483647.f);}static void set(float dec,float halo,float mix){REVFX_PARAM(k_user_revfx_param_time,q(dec));REVFX_PARAM(k_user_revfx_param_depth,q(halo));REVFX_PARAM(k_user_revfx_param_shift_depth,q(mix));}static void settle(float a,float b,float m,int n=260){set(a,b,m);std::vector<float>z(128);for(int i=0;i<n;i++)REVFX_PROCESS(z.data(),64);}static void proc(std::vector<float>&x){std::size_t p=0,n=x.size()/2;while(p<n){uint32_t k=(uint32_t)std::min<std::size_t>(64,n-p);REVFX_PROCESS(x.data()+2*p,k);p+=k;}}static std::vector<float>imp(std::size_t n){std::vector<float>x(n*2);x[0]=x[1]=.65f;return x;}static std::vector<float>excite_tail(float halo){REVFX_INIT(0,0);settle(.75f,halo,1);std::vector<float>x((48000+120000)*2);for(std::size_t i=0;i<48000;i++){float v=.25f*std::sin(2*hs_measure::kPi*440.0*i/48000);x[2*i]=x[2*i+1]=v;}proc(x);return x;}
int main(){
 REVFX_INIT(0,0);settle(.6f,.5f,0);std::vector<float>x(20000*2);for(std::size_t i=0;i<20000;i++){float v=.3f*std::sin(2*hs_measure::kPi*733.0*i/48000);x[2*i]=x[2*i+1]=v;}auto ref=x;proc(x);req(hs_measure::max_abs_diff(x,ref)<2e-6,"MIX=0 is not dry");
 // Tail delivery + stereo network.
 REVFX_INIT(0,0);settle(.6f,.3f,1);auto t=imp(160000);proc(t);auto l=hs_measure::channel(t,0),r=hs_measure::channel(t,1);req(hs_measure::stats(l,1000).rms>1e-4,"reverb tail did not reach output");req(hs_measure::max_abs_diff(l,r)>1e-4,"halo network lacks stereo tail");
 // DECAY increases late energy.
 REVFX_INIT(0,0);settle(.05f,.2f,1);auto s=imp(170000);proc(s);REVFX_INIT(0,0);settle(.98f,.2f,1);auto lg=imp(170000);proc(lg);req(hs_measure::stats(hs_measure::channel(lg,0),95000,60000).rms>hs_measure::stats(hs_measure::channel(s,0),95000,60000).rms*1.4+1e-7,"DECAY does not increase late tail");
 // HALO must inject octave-shifted energy into the feedback tail. Compare 880/440 ratio after identical 440-Hz excitation.
 auto noHalo=hs_measure::channel(excite_tail(0.f),0),halo=hs_measure::channel(excite_tail(1.f),0);const std::size_t start=52000,count=90000;double n440=hs_measure::tone_power(noHalo,440,48000,start,count),n880=hs_measure::tone_power(noHalo,880,48000,start,count);double h440=hs_measure::tone_power(halo,440,48000,start,count),h880=hs_measure::tone_power(halo,880,48000,start,count);req(h440>1e-8&&n440>1e-8,"tail lost fundamental reference");req((h880/h440)>(n880/n440)*1.35+1e-5,"HALO does not increase octave-shimmer energy");
 // Max shimmer/decay remains bounded through long feedback operation.
 REVFX_INIT(0,0);settle(1,1,1);auto soak=imp(500000);proc(soak);for(float v:soak){req(std::isfinite(v),"non-finite shimmer tail");req(std::fabs(v)<=1.001,"shimmer tail exceeded bounds");}
 REVFX_SUSPEND();REVFX_RESUME();std::vector<float>z(30000*2);proc(z);req(hs_measure::stats(hs_measure::channel(z,0)).peak<1e-7,"reset emitted stale shimmer tail");
 std::puts("AUREOLE A-class project-specific harness PASS");return 0;}
