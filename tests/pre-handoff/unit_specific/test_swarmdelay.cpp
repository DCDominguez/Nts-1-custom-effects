#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "tests/pre-handoff/common/measure.hpp"
#include "effects/swarmdelay/nts1/src/swarmdelay.cpp"
static void req(bool x,const char*m){if(!x){std::fprintf(stderr,"SWARMDELAY FAIL: %s\n",m);std::exit(2);}}static int32_t q(float x){x=std::max(0.f,std::min(.999999f,x));return(int32_t)(x*2147483647.f);}static void set(float a,float b,float mix){DELFX_PARAM(k_user_delfx_param_time,q(a));DELFX_PARAM(k_user_delfx_param_depth,q(b));DELFX_PARAM(k_user_delfx_param_shift_depth,q(mix));}static void settle(float a,float b,float mix,int n=260){set(a,b,mix);std::vector<float>z(128);for(int i=0;i<n;i++)DELFX_PROCESS(z.data(),64);}static void proc(std::vector<float>&x){std::size_t p=0,n=x.size()/2;while(p<n){uint32_t k=(uint32_t)std::min<std::size_t>(64,n-p);DELFX_PROCESS(x.data()+2*p,k);p+=k;}}static std::vector<float>imp(std::size_t n){std::vector<float>x(n*2);x[0]=x[1]=.7f;return x;}static std::size_t first(const std::vector<float>&x,double th=.02){auto l=hs_measure::channel(x,0);for(std::size_t i=1;i<l.size();++i)if(std::fabs(l[i])>th)return i;return l.size();}
int main(){
 // Division 0 at 120 BPM = 1/8 beat multiplier .125 -> 3000 samples.
 DELFX_INIT(0,0);test_bpm=120;settle(.0f,0,1);auto a=imp(9000);proc(a);auto f0=first(a);req(std::fabs((double)f0-3000)<160,"short division first echo timing incorrect");
 // Division near index 6 = one beat -> 24000 samples.
 DELFX_INIT(0,0);test_bpm=120;settle(.80f,0,1);auto b=imp(32000);proc(b);auto f1=first(b);req(std::fabs((double)f1-24000)<500,"one-beat division first echo timing incorrect");
 // Tempo doubles => sample delay halves for same division.
 DELFX_INIT(0,0);test_bpm=240;settle(.80f,0,1);auto c=imp(18000);proc(c);auto f2=first(c);req(std::fabs((double)f2-12000)<350,"BPM scaling incorrect");
 // Invalid/very-low BPM uses 120 fallback.
 DELFX_INIT(0,0);test_bpm=0;settle(.80f,0,1);auto d0=imp(32000);proc(d0);req(std::fabs((double)first(d0)-24000)<500,"BPM fallback is not 120");
 // DIVERGENCE spreads the four heads and creates stereo divergence while remaining bounded.
 DELFX_INIT(0,0);test_bpm=120;settle(.5f,0,1);auto tight=imp(50000);proc(tight);DELFX_INIT(0,0);test_bpm=120;settle(.5f,1,1);auto wide=imp(50000);proc(wide);auto wl=hs_measure::channel(wide,0),wr=hs_measure::channel(wide,1);req(hs_measure::max_abs_diff(tight,wide)>.01,"DIVERGENCE does not alter swarm timing");req(hs_measure::max_abs_diff(wl,wr)>1e-3,"DIVERGENCE lacks stereo swarm");req(hs_measure::stats(wl).peak<=1.001&&hs_measure::stats(wr).peak<=1.001,"output exceeded bounds");
 DELFX_SUSPEND();DELFX_RESUME();std::vector<float>z(30000*2);proc(z);req(hs_measure::stats(hs_measure::channel(z,0)).peak<1e-7,"reset emitted stale history");
 std::puts("SWARMDELAY A-class project-specific harness PASS");return 0;}
