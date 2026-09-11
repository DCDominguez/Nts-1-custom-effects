#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "tests/pre-handoff/common/measure.hpp"
#include "effects/parallax/nts1/src/parallax.cpp"

static void req(bool ok,const char*m){if(!ok){std::fprintf(stderr,"PARALLAX FAIL: %s\n",m);std::exit(2);}}
static int32_t q(float x){x=std::max(0.f,std::min(.999999f,x));return(int32_t)(x*2147483647.f);}
static void set(float spread,float div,float mix){DELFX_PARAM(k_user_delfx_param_time,q(spread));DELFX_PARAM(k_user_delfx_param_depth,q(div));DELFX_PARAM(k_user_delfx_param_shift_depth,q(mix));}
static void settle(float spread,float div,float mix,int blocks=240){set(spread,div,mix);std::vector<float>z(128);for(int i=0;i<blocks;i++)DELFX_PROCESS(z.data(),64);}
static void proc(std::vector<float>&x){std::size_t p=0,n=x.size()/2;while(p<n){uint32_t k=(uint32_t)std::min<std::size_t>(64,n-p);DELFX_PROCESS(x.data()+2*p,k);p+=k;}}
static std::vector<float> tone(std::size_t n,double hz=.0,double amp=.35){std::vector<float>x(n*2);for(std::size_t i=0;i<n;i++){float v=hz?(float)(amp*std::sin(2.0*hs_measure::kPi*hz*i/48000.0)):0.f;x[2*i]=x[2*i+1]=v;}return x;}
static std::size_t first_energy(const std::vector<float>&x,double th=1e-4){for(std::size_t i=0;i<x.size()/2;i++)if(std::fabs(x[2*i])>th||std::fabs(x[2*i+1])>th)return i;return x.size()/2;}

int main(){
 // MIX=0 preserves the dry source exactly after parameter slew settles.
 DELFX_INIT(0,0);settle(.5f,.5f,0.f);auto dry=tone(30000,733),ref=dry;proc(dry);req(hs_measure::max_abs_diff(dry,ref)<2e-6,"MIX=0 does not preserve dry spine");

 // SPREAD must move the first wet arrival from the tight doubler region into an audible delay cluster.
 DELFX_INIT(0,0);settle(0.f,.1f,1.f);auto tight=tone(40000);tight[0]=tight[1]=.7f;proc(tight);const std::size_t firstT=first_energy(tight);
 DELFX_INIT(0,0);settle(1.f,.1f,1.f);auto wide=tone(40000);wide[0]=wide[1]=.7f;proc(wide);const std::size_t firstW=first_energy(wide);
 req(firstT>350&&firstT<1400,"tight SPREAD first arrival outside doubler region");req(firstW>firstT*2&&firstW>1500,"SPREAD does not move arrivals into spatial-cluster timing");

 // DIVERGENCE must widen/decorrelate a mono source and engage the true micro-pitch voice.
 DELFX_INIT(0,0);settle(.65f,.02f,1.f);auto narrow=tone(170000,440);proc(narrow);auto nl=hs_measure::channel(narrow,0,2,50000,100000),nr=hs_measure::channel(narrow,1,2,50000,100000);double nd=hs_measure::max_abs_diff(nl,nr);
 DELFX_INIT(0,0);settle(.65f,1.f,1.f);auto broad=tone(170000,440);proc(broad);auto bl=hs_measure::channel(broad,0,2,50000,100000),br=hs_measure::channel(broad,1,2,50000,100000);double bd=hs_measure::max_abs_diff(bl,br);req(bd>nd*1.25+1e-4,"DIVERGENCE does not materially widen independent voices");req(pitch_ratio_a()<.998f,"full DIVERGENCE does not engage true micro-pitch offset");req(hs_measure::max_abs_diff(narrow,broad)>.01,"DIVERGENCE/pitch change does not reach output");

 // Lifecycle reset clears the long delay aperture.
 DELFX_SUSPEND();DELFX_RESUME();std::vector<float>z(40000*2);proc(z);req(hs_measure::stats(hs_measure::channel(z,0)).peak<1e-7,"reset emitted stale spatial-delay history");

 // Long wrap/control soak across the 32768-sample ring.
 DELFX_INIT(0,0);std::vector<float>in(128);for(int b=0;b<9000;b++){set(((b*17)%101)/100.f,((b*43)%101)/100.f,((b*71)%101)/100.f);for(int i=0;i<64;i++){float v=.78f*std::sin(2.0*hs_measure::kPi*(91+(b%19)*83.0)*(b*64+i)/48000.0);in[2*i]=v;in[2*i+1]=-.73f*v;}DELFX_PROCESS(in.data(),64);for(float v:in){req(std::isfinite(v),"non-finite soak output");req(std::fabs(v)<=1.001,"soak output exceeded bounds");}}
 std::puts("PARALLAX A-class project-specific harness PASS");return 0;
}
