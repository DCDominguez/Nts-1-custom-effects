#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "tests/pre-handoff/common/measure.hpp"
#include "effects/abyss/nts1/src/abyss.cpp"
static void req(bool x,const char*m){if(!x){std::fprintf(stderr,"ABYSS FAIL: %s\n",m);std::exit(2);}}static int32_t q(float x){x=std::max(0.f,std::min(.999999f,x));return(int32_t)(x*2147483647.f);}static void set(float dec,float space,float mix){REVFX_PARAM(k_user_revfx_param_time,q(dec));REVFX_PARAM(k_user_revfx_param_depth,q(space));REVFX_PARAM(k_user_revfx_param_shift_depth,q(mix));}static void settle(float a,float b,float m,int n=240){set(a,b,m);std::vector<float>z(128);for(int i=0;i<n;i++)REVFX_PROCESS(z.data(),64);}static void proc(std::vector<float>&x){std::size_t p=0,n=x.size()/2;while(p<n){uint32_t k=(uint32_t)std::min<std::size_t>(64,n-p);REVFX_PROCESS(x.data()+2*p,k);p+=k;}}static std::vector<float>imp(std::size_t n){std::vector<float>x(n*2);x[0]=x[1]=.7f;return x;}
int main(){
 // MIX=0 dry.
 REVFX_INIT(0,0);settle(.6f,.5f,0);std::vector<float>x(20000*2);for(std::size_t i=0;i<20000;i++){float v=.3f*std::sin(2*hs_measure::kPi*701.0*i/48000);x[2*i]=x[2*i+1]=v;}auto ref=x;proc(x);req(hs_measure::max_abs_diff(x,ref)<2e-6,"MIX=0 is not dry");
 // An impulse must produce a delivered stereo tail, not merely internal state.
 REVFX_INIT(0,0);settle(.55f,.5f,1);auto tail=imp(180000);proc(tail);auto l=hs_measure::channel(tail,0),r=hs_measure::channel(tail,1);req(hs_measure::stats(l,1000).rms>1e-4,"reverb tail did not reach output");req(hs_measure::max_abs_diff(l,r)>1e-4,"SPACE network lacks stereo tail");
 // DECAY/TIME materially increases late-tail energy.
 REVFX_INIT(0,0);settle(.05f,.5f,1);auto shortT=imp(180000);proc(shortT);REVFX_INIT(0,0);settle(.98f,.5f,1);auto longT=imp(180000);proc(longT);double sLate=hs_measure::stats(hs_measure::channel(shortT,0),100000,60000).rms,lLate=hs_measure::stats(hs_measure::channel(longT,0),100000,60000).rms;req(lLate>sLate*1.5+1e-7,"DECAY does not increase late tail");
 // SPACE/DEPTH changes FDN scale/modulation and therefore audible tail geometry.
 REVFX_INIT(0,0);settle(.65f,.05f,1);auto small=imp(120000);proc(small);REVFX_INIT(0,0);settle(.65f,1.f,1);auto huge=imp(120000);proc(huge);req(hs_measure::max_abs_diff(small,huge)>.01,"SPACE does not materially change tail geometry");
 // Max settings remain bounded through repeated FDN wraps.
 REVFX_INIT(0,0);settle(1,1,1);auto soak=imp(500000);proc(soak);for(float v:soak){req(std::isfinite(v),"non-finite max-space tail");req(std::fabs(v)<=1.001,"max-space tail exceeded bounds");}
 REVFX_SUSPEND();REVFX_RESUME();std::vector<float>z(30000*2);proc(z);req(hs_measure::stats(hs_measure::channel(z,0)).peak<1e-7,"reset emitted stale reverb tail");
 std::puts("ABYSS A-class project-specific harness PASS");return 0;}
