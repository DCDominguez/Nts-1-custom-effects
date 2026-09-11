#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "tests/pre-handoff/common/measure.hpp"
#include "effects/helix/nts1/src/helix.cpp"
static void req(bool ok,const char*m){if(!ok){std::fprintf(stderr,"HELIX FAIL: %s\n",m);std::exit(2);}}
static int32_t q(float x){x=std::max(0.f,std::min(.999999f,x));return(int32_t)(x*2147483647.f);}static void set(float a,float b){MODFX_PARAM(k_user_modfx_param_time,q(a));MODFX_PARAM(k_user_modfx_param_depth,q(b));}
static void settle(float a,float b,int blocks=180){set(a,b);std::vector<float>z(128),o(128),s(128),sy(128);for(int i=0;i<blocks;i++)MODFX_PROCESS(z.data(),o.data(),s.data(),sy.data(),64);}static std::vector<float>run(std::size_t n,double hz,float amp=.45f){std::vector<float>in(n*2),out(n*2),s(n*2),sy(n*2);for(std::size_t i=0;i<n;i++){float v=amp*std::sin(2.0*hs_measure::kPi*hz*i/48000.0);in[2*i]=in[2*i+1]=v;}std::size_t p=0;while(p<n){uint32_t k=(uint32_t)std::min<std::size_t>(64,n-p);MODFX_PROCESS(in.data()+2*p,out.data()+2*p,s.data()+2*p,sy.data()+2*p,k);p+=k;}return out;}
static double motion(const std::vector<float>&x){auto l=hs_measure::channel(x,0);double e=0;for(std::size_t i=2;i<l.size();i++)e+=std::fabs((double)l[i]-2.0*l[i-1]+l[i-2]);return e/std::max<std::size_t>(1,l.size()-2);}
int main(){
 // Buffered flanger must deliver audio after its short history fills.
 MODFX_INIT(0,0);settle(.2f,.1f);auto mild=run(96000,811);req(hs_measure::stats(hs_measure::channel(mild,0),4096).rms>.04,"processed audio not delivered after buffer fill");
 // HELIX depth must materially expand moving read-head geometry/feedback.
 MODFX_INIT(0,0);settle(.35f,.05f);auto shallow=run(120000,500);MODFX_INIT(0,0);settle(.35f,1.f);auto deep=run(120000,500);req(hs_measure::max_abs_diff(shallow,deep)>.015,"HELIX depth does not materially change output");
 // ROTATION rate must increase movement speed.
 MODFX_INIT(0,0);settle(.05f,.9f);auto slow=run(240000,430);double sm=motion(slow);MODFX_INIT(0,0);settle(.95f,.9f);auto fast=run(240000,430);double fm=motion(fast);req(fm>sm*1.02+1e-8,"ROTATION does not measurably increase moving-head motion");
 // Stereo offset and bounded feedback identity.
 auto l=hs_measure::channel(deep,0),r=hs_measure::channel(deep,1);req(hs_measure::max_abs_diff(l,r)>1e-4,"stereo delay-head offset absent");req(hs_measure::stats(l).peak<=1.001&&hs_measure::stats(r).peak<=1.001,"feedback output exceeded bounds");
 // Repeated wraps at max depth must remain continuous enough to avoid hard full-scale jumps.
 double maxjump=0;for(std::size_t i=1;i<l.size();i++)maxjump=std::max(maxjump,std::fabs((double)l[i]-l[i-1]));req(maxjump<1.3,"moving-head/buffer wrap produced full-scale discontinuity");
 // Lifecycle reset clears delay RAM.
 MODFX_SUSPEND();MODFX_RESUME();std::vector<float>z(12000*2),o(12000*2),s(12000*2),sy(12000*2);MODFX_PROCESS(z.data(),o.data(),s.data(),sy.data(),12000);req(hs_measure::stats(hs_measure::channel(o,0)).peak<1e-7,"reset emitted stale delay history");
 // Long wrap + parameter soak.
 MODFX_INIT(0,0);std::vector<float>in(128),out(128),sub(128),suby(128);for(int b=0;b<10000;b++){set(((b*19)%101)/100.f,((b*47)%101)/100.f);for(int i=0;i<64;i++){float v=.75f*std::sin(2.0*hs_measure::kPi*(97+(b%23)*67.0)*(b*64+i)/48000.0);in[2*i]=v;in[2*i+1]=.9f*v;}MODFX_PROCESS(in.data(),out.data(),sub.data(),suby.data(),64);for(float v:out){req(std::isfinite(v),"non-finite soak output");req(std::fabs(v)<=1.001,"soak output exceeded bounds");}}
 std::puts("HELIX A-class project-specific harness PASS");return 0;}
