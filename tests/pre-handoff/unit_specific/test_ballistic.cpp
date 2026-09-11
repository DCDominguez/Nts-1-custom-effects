#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "tests/pre-handoff/common/measure.hpp"
#include "effects/ballistic/nts1/src/ballistic.cpp"
static void req(bool x,const char*m){if(!x){std::fprintf(stderr,"BALLISTIC FAIL: %s\n",m);std::exit(2);}}static int32_t q(float x){x=std::max(0.f,std::min(.999999f,x));return(int32_t)(x*2147483647.f);}static void set(float a,float b,float mix){DELFX_PARAM(k_user_delfx_param_time,q(a));DELFX_PARAM(k_user_delfx_param_depth,q(b));DELFX_PARAM(k_user_delfx_param_shift_depth,q(mix));}
static void settle(float a,float b,float mix,int blocks=220){set(a,b,mix);std::vector<float>z(128);for(int i=0;i<blocks;i++)DELFX_PROCESS(z.data(),64);}static void process(std::vector<float>&x){std::size_t p=0,n=x.size()/2;while(p<n){uint32_t k=(uint32_t)std::min<std::size_t>(64,n-p);DELFX_PROCESS(x.data()+2*p,k);p+=k;}}static std::vector<float>imp(std::size_t n,float a=.7f){std::vector<float>x(n*2);x[0]=x[1]=a;return x;}static std::size_t first(const std::vector<float>&x,double th=.02){auto l=hs_measure::channel(x,0);for(std::size_t i=1;i<l.size();++i)if(std::fabs(l[i])>th)return i;return l.size();}
int main(){
 DELFX_INIT(0,0);settle(.4f,.5f,0);std::vector<float>x(20000*2);for(std::size_t i=0;i<20000;i++)x[2*i]=x[2*i+1]=.3f*std::sin(2*hs_measure::kPi*701.0*i/48000);auto ref=x;process(x);req(hs_measure::max_abs_diff(x,ref)<2e-6,"MIX=0 is not dry");
 // TRAJECTORY contract is the production curve itself: accelerating -> even -> decelerating.
 req(curve(.25f,0.f)<.25f&&curve(.75f,0.f)<.75f,"accelerating trajectory curve invalid");req(std::fabs(curve(.25f,.5f)-.25f)<1e-6&&std::fabs(curve(.75f,.5f)-.75f)<1e-6,"even trajectory curve invalid");req(curve(.25f,1.f)>.25f&&curve(.75f,1.f)>.75f,"decelerating trajectory curve invalid");
 // RANGE reaches the audible output: first tap moves later as TIME increases.
 DELFX_INIT(0,0);settle(.15f,.5f,1);auto a=imp(60000);process(a);auto fa=first(a);DELFX_INIT(0,0);settle(.75f,.5f,1);auto b=imp(60000);process(b);auto fb=first(b);req(fa<a.size()/2&&fb<b.size()/2&&fb>fa*2,"RANGE does not move first audible tap later");
 // TRAJECTORY changes the rendered tap field under identical RANGE.
 DELFX_INIT(0,0);settle(.55f,0.f,1);auto acc=imp(65000);process(acc);DELFX_INIT(0,0);settle(.55f,1.f,1);auto dec=imp(65000);process(dec);req(hs_measure::max_abs_diff(acc,dec)>.02,"TRAJECTORY does not change audible tap geometry");
 // Tail decays and remains bounded.
 DELFX_INIT(0,0);settle(.6f,0.f,1);auto tail=imp(180000,.6f);process(tail);auto l=hs_measure::channel(tail,0);req(hs_measure::stats(l).peak<=1.001,"output exceeded bounds");req(hs_measure::stats(l,120000,50000).rms<hs_measure::stats(l,1000,100000).rms*.6,"feedback tail failed to decay");
 DELFX_SUSPEND();DELFX_RESUME();std::vector<float>z(20000*2);process(z);req(hs_measure::stats(hs_measure::channel(z,0)).peak<1e-7,"reset emitted stale delay");
 std::puts("BALLISTIC A-class project-specific harness PASS");return 0;}
