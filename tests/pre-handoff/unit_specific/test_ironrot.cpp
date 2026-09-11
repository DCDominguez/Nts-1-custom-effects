#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "tests/pre-handoff/common/measure.hpp"
#include "effects/ironrot/nts1/src/ironrot.cpp"

static void req(bool ok,const char*m){if(!ok){std::fprintf(stderr,"IRONROT FAIL: %s\n",m);std::exit(2);}}
static int32_t q(float x){x=std::max(0.f,std::min(.999999f,x));return(int32_t)(x*2147483647.f);}
static void set(float ch,float cor){MODFX_PARAM(k_user_modfx_param_time,q(ch));MODFX_PARAM(k_user_modfx_param_depth,q(cor));}
static void settle(float ch,float cor,int blocks=240){set(ch,cor);std::vector<float>z(128),o(128),s(128),sy(128);for(int i=0;i<blocks;i++)MODFX_PROCESS(z.data(),o.data(),s.data(),sy.data(),64);}
static std::vector<float> run(std::size_t n,const std::vector<float>&mono){std::vector<float>in(n*2),out(n*2),s(n*2),sy(n*2);for(std::size_t i=0;i<n;i++)in[2*i]=in[2*i+1]=mono[i];std::size_t p=0;while(p<n){uint32_t k=(uint32_t)std::min<std::size_t>(64,n-p);MODFX_PROCESS(in.data()+2*p,out.data()+2*p,s.data()+2*p,sy.data()+2*p,k);p+=k;}return out;}
static std::vector<float> sine(std::size_t n,double hz,double amp=.35){std::vector<float>x(n);for(std::size_t i=0;i<n;i++)x[i]=(float)(amp*std::sin(2.0*hs_measure::kPi*hz*i/48000.0));return x;}
static std::vector<float> two_tone(std::size_t n){std::vector<float>x(n);for(std::size_t i=0;i<n;i++)x[i]=.22f*std::sin(2.0*hs_measure::kPi*250.0*i/48000.0)+.22f*std::sin(2.0*hs_measure::kPi*5000.0*i/48000.0);return x;}

int main(){
 // CORROSION=0 is a true dry contract even while CHARACTER moves internally.
 MODFX_INIT(0,0);settle(.95f,0.f);auto src=sine(50000,997),dry=run(src.size(),src);auto dl=hs_measure::channel(dry,0);req(hs_measure::max_abs_diff(dl,src)<2e-6,"CORROSION=0 is not dry");

 // CHARACTER must move the spectral balance from dark/rounded toward bright/eroded at meaningful corrosion.
 auto mt=two_tone(140000);
 MODFX_INIT(0,0);settle(.02f,.82f);auto dark=hs_measure::channel(run(mt.size(),mt),0);double darkRatio=hs_measure::tone_power(dark,5000,48000,24000,90000)/(hs_measure::tone_power(dark,250,48000,24000,90000)+1e-12);
 MODFX_INIT(0,0);settle(.98f,.82f);auto bright=hs_measure::channel(run(mt.size(),mt),0);double brightRatio=hs_measure::tone_power(bright,5000,48000,24000,90000)/(hs_measure::tone_power(bright,250,48000,24000,90000)+1e-12);req(brightRatio>darkRatio*1.15,"CHARACTER does not move spectral balance toward bright/eroded");

 // CORROSION must add nonlinear harmonic character, not merely level.
 auto one=sine(140000,1000,.28);
 MODFX_INIT(0,0);settle(.8f,.05f);auto low=hs_measure::channel(run(one.size(),one),0);double lowH=hs_measure::tone_power(low,3000,48000,20000,100000)/(hs_measure::tone_power(low,1000,48000,20000,100000)+1e-12);
 MODFX_INIT(0,0);settle(.8f,1.f);auto high=hs_measure::channel(run(one.size(),one),0);double highH=hs_measure::tone_power(high,3000,48000,20000,100000)/(hs_measure::tone_power(high,1000,48000,20000,100000)+1e-12);req(highH>lowH*2.0+1e-6,"CORROSION does not materially increase harmonic fracture");req(hs_measure::stats(high).peak<=1.001,"hot corrosion exceeded output bound");

 // Suspend/resume clears tone/envelope memory; silence must remain silence.
 MODFX_SUSPEND();MODFX_RESUME();std::vector<float>z(8192*2),o(8192*2),s(8192*2),sy(8192*2);MODFX_PROCESS(z.data(),o.data(),s.data(),sy.data(),8192);req(hs_measure::stats(hs_measure::channel(o,0)).peak<1e-7,"lifecycle reset left residual tone state");

 // Hot nonlinear/control soak checks boundedness, finiteness and pathological DC accumulation.
 MODFX_INIT(0,0);std::vector<float>in(128),out(128),sub(128),suby(128);double dc=0;std::size_t count=0;for(int b=0;b<10000;b++){set(((b*31)%101)/100.f,((b*67)%101)/100.f);for(int i=0;i<64;i++){float v=.88f*std::sin(2.0*hs_measure::kPi*(113+(b%13)*173.0)*(b*64+i)/48000.0);in[2*i]=v;in[2*i+1]=-.91f*v;}MODFX_PROCESS(in.data(),out.data(),sub.data(),suby.data(),64);for(float v:out){req(std::isfinite(v),"non-finite soak output");req(std::fabs(v)<=1.001,"soak output exceeded bounds");dc+=v;++count;}}req(std::fabs(dc/std::max<std::size_t>(1,count))<.08,"nonlinear soak accumulated excessive DC");
 std::puts("IRONROT A-class project-specific harness PASS");return 0;
}
