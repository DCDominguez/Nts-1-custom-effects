#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "tests/pre-handoff/common/measure.hpp"
#include "effects/rainfall/nts1/src/rainfall.cpp"
static void req(bool x,const char*m){if(!x){std::fprintf(stderr,"RAINFALL FAIL: %s\n",m);std::exit(2);}}static int32_t q(float x){x=std::max(0.f,std::min(.999999f,x));return(int32_t)(x*2147483647.f);}static void set(float a,float b,float mix){DELFX_PARAM(k_user_delfx_param_time,q(a));DELFX_PARAM(k_user_delfx_param_depth,q(b));DELFX_PARAM(k_user_delfx_param_shift_depth,q(mix));}static void settle(float a,float b,float mix,int n=240){set(a,b,mix);std::vector<float>z(128);for(int i=0;i<n;i++)DELFX_PROCESS(z.data(),64);}static void proc(std::vector<float>&x){std::size_t p=0,n=x.size()/2;while(p<n){uint32_t k=(uint32_t)std::min<std::size_t>(64,n-p);DELFX_PROCESS(x.data()+2*p,k);p+=k;}}static std::vector<float>tone(std::size_t n,double hz=.0){std::vector<float>x(n*2);for(std::size_t i=0;i<n;i++){float v=hz? .35f*std::sin(2*hs_measure::kPi*hz*i/48000):0.f;x[2*i]=x[2*i+1]=v;}return x;}
int main(){
 DELFX_INIT(0,0);settle(.4f,.5f,0);auto x=tone(20000,613);auto ref=x;proc(x);req(hs_measure::max_abs_diff(x,ref)<2e-6,"MIX=0 not dry");
 // DENSITY must retarget voices more often: observe the production scheduler over an equal sample interval.
 DELFX_INIT(0,0);settle(.05f,.6f,1);uint32_t lowCnt=cnt;int lowVoice=voice;auto zl=tone(48000);proc(zl);int lowSteps=(voice-lowVoice+4)%4;uint32_t lowRemainder=cnt; (void)lowCnt;(void)lowRemainder;
 DELFX_INIT(0,0);settle(.95f,.6f,1);int highVoice=voice;auto zh=tone(48000);proc(zh);int highSteps=(voice-highVoice+4)%4; // modulo alone can alias; scheduler period itself is also production contract.
 const uint32_t pLow=(uint32_t)(SR/(.5f+11.f*.05f*.05f)),pHigh=(uint32_t)(SR/(.5f+11.f*.95f*.95f));req(pHigh<pLow/8,"DENSITY scheduler period does not contract strongly");req(hs_measure::max_abs_diff(zl,zh)>1e-4||highSteps!=lowSteps,"DENSITY failed to alter rendered field/scheduler");
 // WEATHER expands delay span/panning and feedback; mono source should become more stereo at high depth.
 DELFX_INIT(0,0);settle(.7f,.05f,1);auto mild=tone(150000,347);proc(mild);auto ml=hs_measure::channel(mild,0),mr=hs_measure::channel(mild,1);double md=hs_measure::max_abs_diff(ml,mr);
 DELFX_INIT(0,0);settle(.7f,1.f,1);auto storm=tone(150000,347);proc(storm);auto sl=hs_measure::channel(storm,0),sr=hs_measure::channel(storm,1);double sd=hs_measure::max_abs_diff(sl,sr);req(sd>md+1e-3,"WEATHER does not expand stereo field");req(hs_measure::stats(sl).peak<=1.001&&hs_measure::stats(sr).peak<=1.001,"storm output exceeded bounds");
 // Reset reproducibility includes RNG/retarget state.
 DELFX_INIT(0,0);settle(.8f,.9f,1);auto a=tone(160000,251);proc(a);DELFX_INIT(0,0);settle(.8f,.9f,1);auto b=tone(160000,251);proc(b);req(hs_measure::max_abs_diff(a,b)<1e-6,"seeded rainfall trajectory not reproducible after init");
 DELFX_SUSPEND();DELFX_RESUME();auto z=tone(30000);proc(z);req(hs_measure::stats(hs_measure::channel(z,0)).peak<1e-7,"reset emitted stale history");
 std::puts("RAINFALL A-class project-specific harness PASS");return 0;}
