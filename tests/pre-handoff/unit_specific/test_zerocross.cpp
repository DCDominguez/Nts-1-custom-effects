#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "tests/pre-handoff/common/measure.hpp"
#include "effects/zerocross/nts1/src/zerocross.cpp"

static void req(bool ok,const char*m){if(!ok){std::fprintf(stderr,"ZEROCROSS FAIL: %s\n",m);std::exit(2);}}
static int32_t q(float x){x=std::max(0.f,std::min(.999999f,x));return(int32_t)(x*2147483647.f);}
static void set(float rate,float depth){MODFX_PARAM(k_user_modfx_param_time,q(rate));MODFX_PARAM(k_user_modfx_param_depth,q(depth));}
static void settle(float rate,float depth,int blocks=200){set(rate,depth);std::vector<float>z(128),o(128),s(128),sy(128);for(int i=0;i<blocks;i++)MODFX_PROCESS(z.data(),o.data(),s.data(),sy.data(),64);}
static std::vector<float> run(std::size_t n,double hz=0,double amp=.4){std::vector<float>in(n*2),out(n*2),s(n*2),sy(n*2);for(std::size_t i=0;i<n;i++){float v=hz?(float)(amp*std::sin(2.0*hs_measure::kPi*hz*i/48000.0)):0.f;in[2*i]=in[2*i+1]=v;}std::size_t p=0;while(p<n){uint32_t k=(uint32_t)std::min<std::size_t>(64,n-p);MODFX_PROCESS(in.data()+2*p,out.data()+2*p,s.data()+2*p,sy.data()+2*p,k);p+=k;}return out;}

int main(){
 // Fixed delayed reference is part of the identity: DEPTH=0 should deliver the impulse near 170 samples, not immediately.
 MODFX_INIT(0,0);settle(.2f,0.f);std::vector<float>in(1200*2),out(1200*2),s(1200*2),sy(1200*2);in[0]=in[1]=.5f;MODFX_PROCESS(in.data(),out.data(),s.data(),sy.data(),1200);auto ol=hs_measure::channel(out,0);std::size_t first=ol.size();for(std::size_t i=0;i<ol.size();++i)if(std::fabs(ol[i])>1e-4){first=i;break;}req(first>=165&&first<=175,"fixed delayed reference latency is wrong");

 // DEPTH must increase sweep width/feedback and create the documented L/R phase divergence on mono input.
 MODFX_INIT(0,0);settle(.45f,.03f);auto shallow=run(170000,733);auto sl=hs_measure::channel(shallow,0,2,20000,120000),sr=hs_measure::channel(shallow,1,2,20000,120000);double sd=hs_measure::max_abs_diff(sl,sr);
 MODFX_INIT(0,0);settle(.45f,1.f);auto deep=run(170000,733);auto dl=hs_measure::channel(deep,0,2,20000,120000),dr=hs_measure::channel(deep,1,2,20000,120000);double dd=hs_measure::max_abs_diff(dl,dr);req(dd>sd*2.0+1e-4,"DEPTH does not materially increase stereo sweep divergence");req(hs_measure::max_abs_diff(shallow,deep)>.01,"DEPTH change does not reach output");

 // TIME/RATE must increase modulation movement at high DEPTH.
 MODFX_INIT(0,0);settle(.03f,.92f);auto slow=hs_measure::channel(run(260000,431),0);double sv=hs_measure::window_frequency_variance(slow,6000);
 MODFX_INIT(0,0);settle(.98f,.92f);auto fast=hs_measure::channel(run(260000,431),0);double fv=hs_measure::window_frequency_variance(fast,6000);req(fv>sv*1.15+1e-4,"TIME/RATE does not measurably increase modulation movement");

 // Reset clears circular-buffer/feedback state.
 MODFX_SUSPEND();MODFX_RESUME();auto silence=run(4096);req(hs_measure::stats(hs_measure::channel(silence,0)).peak<1e-7,"reset emitted stale delay/feedback history");

 // Long ring-wrap/control soak.
 MODFX_INIT(0,0);std::vector<float>bi(128),bo(128),bs(128),bsy(128);for(int b=0;b<10000;b++){set(((b*37)%101)/100.f,((b*59)%101)/100.f);for(int i=0;i<64;i++){float v=.8f*std::sin(2.0*hs_measure::kPi*(83+(b%23)*97.0)*(b*64+i)/48000.0);bi[2*i]=v;bi[2*i+1]=-.8f*v;}MODFX_PROCESS(bi.data(),bo.data(),bs.data(),bsy.data(),64);for(float v:bo){req(std::isfinite(v),"non-finite soak output");req(std::fabs(v)<=1.001,"soak output exceeded bounds");}}
 std::puts("ZEROCROSS A-class project-specific harness PASS");return 0;
}
