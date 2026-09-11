#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "tests/pre-handoff/common/measure.hpp"
#include "effects/bucketline/nts1/src/bucketline.cpp"
static void req(bool x,const char*m){if(!x){std::fprintf(stderr,"BUCKETLINE FAIL: %s\n",m);std::exit(2);}}static int32_t q(float x){x=std::max(0.f,std::min(.999999f,x));return(int32_t)(x*2147483647.f);}static void set(float de,float age,float mix){DELFX_PARAM(k_user_delfx_param_time,q(de));DELFX_PARAM(k_user_delfx_param_depth,q(age));DELFX_PARAM(k_user_delfx_param_shift_depth,q(mix));}static void settle(float a,float b,float m,int n=260){set(a,b,m);std::vector<float>z(128);for(int i=0;i<n;i++)DELFX_PROCESS(z.data(),64);}static void proc(std::vector<float>&x){std::size_t p=0,n=x.size()/2;while(p<n){uint32_t k=(uint32_t)std::min<std::size_t>(64,n-p);DELFX_PROCESS(x.data()+2*p,k);p+=k;}}static std::vector<float>imp(std::size_t n){std::vector<float>x(n*2);x[0]=x[1]=.7f;return x;}static std::size_t first(const std::vector<float>&x,double th=.02){auto l=hs_measure::channel(x,0);for(std::size_t i=1;i<l.size();++i)if(std::fabs(l[i])>th)return i;return l.size();}
int main(){
 // DELAY/TIME must move the first echo across a large range when AGE=0 removes modulation offset.
 DELFX_INIT(0,0);settle(.08f,0,1);auto shortD=imp(12000);proc(shortD);auto fs=first(shortD);DELFX_INIT(0,0);settle(.75f,0,1);auto longD=imp(50000);proc(longD);auto fl=first(longD);req(fs<shortD.size()/2&&fl<longD.size()/2&&fl>fs*8,"DELAY does not materially move first echo");
 // AGE changes the wet path: modulation, darker feedback loss and higher feedback.
 DELFX_INIT(0,0);settle(.45f,.05f,1);auto young=imp(150000);proc(young);DELFX_INIT(0,0);settle(.45f,1.f,1);auto old=imp(150000);proc(old);req(hs_measure::max_abs_diff(young,old)>.01,"AGE does not alter rendered delay character");auto ol=hs_measure::channel(old,0),orr=hs_measure::channel(old,1);req(hs_measure::max_abs_diff(ol,orr)>1e-4,"AGE lacks stereo delay separation");
 // Higher AGE should preserve more late-tail energy because feedback increases from .2 toward .84.
 auto yl=hs_measure::channel(young,0);double youngLate=hs_measure::stats(yl,80000,50000).rms,oldLate=hs_measure::stats(ol,80000,50000).rms;req(oldLate>youngLate+1e-6,"AGE does not increase late memory/feedback persistence");req(hs_measure::stats(ol).peak<=1.001&&hs_measure::stats(orr).peak<=1.001,"output exceeded bounds");
 // MIX=0 remains dry.
 DELFX_INIT(0,0);settle(.6f,1.f,0);std::vector<float>x(20000*2);for(std::size_t i=0;i<20000;i++){float v=.3f*std::sin(2*hs_measure::kPi*911.0*i/48000);x[2*i]=x[2*i+1]=v;}auto ref=x;proc(x);req(hs_measure::max_abs_diff(x,ref)<2e-6,"MIX=0 is not dry");
 DELFX_SUSPEND();DELFX_RESUME();std::vector<float>z(30000*2);proc(z);req(hs_measure::stats(hs_measure::channel(z,0)).peak<1e-7,"reset emitted stale bucket history");
 // Long max-AGE wrap soak stays finite.
 DELFX_INIT(0,0);settle(.95f,1.f,1);auto soak=imp(300000);proc(soak);for(float v:soak){req(std::isfinite(v),"non-finite long-delay output");req(std::fabs(v)<=1.001,"long-delay output exceeded bounds");}
 std::puts("BUCKETLINE A-class project-specific harness PASS");return 0;}
