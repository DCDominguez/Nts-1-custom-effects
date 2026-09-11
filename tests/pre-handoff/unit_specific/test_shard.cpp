#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "tests/pre-handoff/common/measure.hpp"
#include "effects/shard/nts1/src/shard.cpp"
static void req(bool x,const char*m){if(!x){std::fprintf(stderr,"SHARD FAIL: %s\n",m);std::exit(2);}}static int32_t q(float x){x=std::max(0.f,std::min(.999999f,x));return(int32_t)(x*2147483647.f);}static void set(float pitch,float grain,float mix){DELFX_PARAM(k_user_delfx_param_time,q(pitch));DELFX_PARAM(k_user_delfx_param_depth,q(grain));DELFX_PARAM(k_user_delfx_param_shift_depth,q(mix));}static void settle(float a,float b,float m,int n=300){set(a,b,m);std::vector<float>z(128);for(int i=0;i<n;i++)DELFX_PROCESS(z.data(),64);}static std::vector<float>run(std::size_t n,double hz,float amp=.3f){std::vector<float>x(n*2);for(std::size_t i=0;i<n;i++){float v=amp*std::sin(2*hs_measure::kPi*hz*i/48000);x[2*i]=x[2*i+1]=v;}std::size_t p=0;while(p<n){uint32_t k=(uint32_t)std::min<std::size_t>(64,n-p);DELFX_PROCESS(x.data()+2*p,k);p+=k;}return x;}
static double rendered_freq(float pitch){DELFX_INIT(0,0);settle(pitch,.5f,1);auto y=run(150000,440);auto l=hs_measure::channel(y,0);return hs_measure::zero_cross_frequency(l,48000,40000,90000);}
int main(){
 // PITCH ratio helper defines .5x -> 1x -> 2x endpoints.
 req(std::fabs(ratio(0.f)-.5f)<1e-6&&std::fabs(ratio(.5f)-1.f)<1e-6&&std::fabs(ratio(1.f)-2.f)<1e-6,"PITCH ratio map invalid");
 // The production grain reader must deliver a corresponding pitch trend at output.
 double down=rendered_freq(0.f),unity=rendered_freq(.5f),up=rendered_freq(1.f);req(down>100&&unity>200&&up>300,"pitch-shifted output was not measurable");req(down<unity*.8&&up>unity*1.35,"PITCH control does not produce expected down/unity/up ordering");
 // GRAIN changes window length / overlap behavior and therefore the rendered waveform.
 DELFX_INIT(0,0);settle(.75f,.05f,1);auto small=run(100000,330);DELFX_INIT(0,0);settle(.75f,1.f,1);auto large=run(100000,330);req(hs_measure::max_abs_diff(small,large)>.01,"GRAIN does not materially change shard texture");
 // Full wet output remains stereo-safe and bounded.
 auto ll=hs_measure::channel(large,0),rr=hs_measure::channel(large,1);req(hs_measure::max_abs_diff(ll,rr)<2e-5,"mono source unexpectedly diverged in SHARD stereo path");req(hs_measure::stats(ll).peak<=1.001,"grain output exceeded bounds");
 // MIX=0 dry.
 DELFX_INIT(0,0);settle(1.f,1.f,0);auto dry=run(20000,611);std::vector<float>ref(20000);for(std::size_t i=0;i<ref.size();i++)ref[i]=.3f*std::sin(2*hs_measure::kPi*611.0*i/48000);req(hs_measure::max_abs_diff(hs_measure::channel(dry,0),ref)<2e-6,"MIX=0 is not dry");
 DELFX_SUSPEND();DELFX_RESUME();std::vector<float>z(20000*2);std::size_t p=0;while(p<20000){uint32_t k=(uint32_t)std::min<std::size_t>(64,20000-p);DELFX_PROCESS(z.data()+2*p,k);p+=k;}req(hs_measure::stats(hs_measure::channel(z,0)).peak<1e-7,"reset emitted stale grains");
 // Long wrap + parameter soak.
 DELFX_INIT(0,0);std::vector<float>b(128);for(int n=0;n<10000;n++){set(((n*29)%101)/100.f,((n*47)%101)/100.f,1);for(int i=0;i<64;i++){float v=.65f*std::sin(2*hs_measure::kPi*(101+(n%13)*79.0)*(n*64+i)/48000);b[2*i]=v;b[2*i+1]=v;}DELFX_PROCESS(b.data(),64);for(float v:b){req(std::isfinite(v),"non-finite grain soak output");req(std::fabs(v)<=1.001,"grain soak output exceeded bounds");}}
 std::puts("SHARD A-class project-specific harness PASS");return 0;}
