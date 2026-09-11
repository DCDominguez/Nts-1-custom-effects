#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "tests/pre-handoff/common/measure.hpp"
#include "effects/longmemory/nts1/src/longmemory.cpp"
static void req(bool x,const char*m){if(!x){std::fprintf(stderr,"LONGMEMORY FAIL: %s\n",m);std::exit(2);}}static int32_t q(float x){x=std::max(0.f,std::min(.999999f,x));return(int32_t)(x*2147483647.f);}static void set(float len,float mem,float mix){DELFX_PARAM(k_user_delfx_param_time,q(len));DELFX_PARAM(k_user_delfx_param_depth,q(mem));DELFX_PARAM(k_user_delfx_param_shift_depth,q(mix));}static void settle(float a,float b,float m,int n=300){set(a,b,m);std::vector<float>z(128);for(int i=0;i<n;i++)DELFX_PROCESS(z.data(),64);}static void proc(std::vector<float>&x){std::size_t p=0,n=x.size()/2;while(p<n){uint32_t k=(uint32_t)std::min<std::size_t>(64,n-p);DELFX_PROCESS(x.data()+2*p,k);p+=k;}}static std::vector<float>imp(std::size_t n){std::vector<float>x(n*2);x[0]=x[1]=.7f;return x;}static std::size_t first(const std::vector<float>&x,double th=.02){auto l=hs_measure::channel(x,0);for(std::size_t i=1;i<l.size();++i)if(std::fabs(l[i])>th)return i;return l.size();}
int main(){
 // LENGTH/TIME spans from short memory to multi-second delay.
 DELFX_INIT(0,0);settle(.05f,.1f,1);auto s=imp(15000);proc(s);auto fs=first(s);DELFX_INIT(0,0);settle(.95f,.1f,1);auto l=imp(125000);proc(l);auto fl=first(l);req(fs<s.size()/2&&fl<l.size()/2&&fl>fs*20,"LENGTH does not span expected long-memory range");
 // MEMORY increases feedback persistence and stereo offset.
 DELFX_INIT(0,0);settle(.35f,.05f,1);auto low=imp(220000);proc(low);DELFX_INIT(0,0);settle(.35f,.95f,1);auto high=imp(220000);proc(high);auto ll=hs_measure::channel(low,0),hl=hs_measure::channel(high,0),hr=hs_measure::channel(high,1);double loLate=hs_measure::stats(ll,120000,80000).rms,hiLate=hs_measure::stats(hl,120000,80000).rms;req(hiLate>loLate+1e-6,"MEMORY does not increase late persistence");req(hs_measure::max_abs_diff(hl,hr)>1e-5,"MEMORY stereo offset absent");
 // Near-maximum MEMORY is allowed to be very long but must remain finite/bounded through multiple wraps.
 DELFX_INIT(0,0);settle(.7f,1.f,1);auto soak=imp(500000);proc(soak);for(float v:soak){req(std::isfinite(v),"non-finite near-loop output");req(std::fabs(v)<=1.001,"near-loop output exceeded bounds");}
 // MIX=0 dry contract.
 DELFX_INIT(0,0);settle(.9f,1.f,0);std::vector<float>x(24000*2);for(std::size_t i=0;i<24000;i++){float v=.25f*std::sin(2*hs_measure::kPi*733.0*i/48000);x[2*i]=x[2*i+1]=v;}auto ref=x;proc(x);req(hs_measure::max_abs_diff(x,ref)<2e-6,"MIX=0 is not dry");
 DELFX_SUSPEND();DELFX_RESUME();std::vector<float>z(140000*2);proc(z);req(hs_measure::stats(hs_measure::channel(z,0)).peak<1e-7,"reset emitted stale long-memory history");
 std::puts("LONGMEMORY A-class project-specific harness PASS");return 0;}
