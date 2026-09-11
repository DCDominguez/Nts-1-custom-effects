#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include "tests/pre-handoff/common/measure.hpp"
#include "effects/vectorfilter/nts1/src/vectorfilter.cpp"

static void require(bool ok,const char* msg){if(!ok){std::fprintf(stderr,"VECTORFILTER FAIL: %s\n",msg);std::exit(2);}}
static int32_t q31p(float x){x=std::max(0.0f,std::min(0.999999f,x));return static_cast<int32_t>(x*2147483647.0f);}
static void set_controls(float cut,float vec){MODFX_PARAM(k_user_modfx_param_time,q31p(cut));MODFX_PARAM(k_user_modfx_param_depth,q31p(vec));}
static void settle(float cut,float vec,int blocks=180){set_controls(cut,vec);std::vector<float> z(128),y(128),s(128),sy(128);for(int b=0;b<blocks;++b)MODFX_PROCESS(z.data(),y.data(),s.data(),sy.data(),64);}
static std::vector<float> process_mono(const std::vector<float>& mono){std::vector<float> in(mono.size()*2),out(mono.size()*2),s(mono.size()*2),sy(mono.size()*2);for(std::size_t i=0;i<mono.size();++i)in[2*i]=in[2*i+1]=mono[i];std::size_t p=0;while(p<mono.size()){uint32_t n=static_cast<uint32_t>(std::min<std::size_t>(64,mono.size()-p));MODFX_PROCESS(in.data()+2*p,out.data()+2*p,s.data()+2*p,sy.data()+2*p,n);p+=n;}return out;}
static std::vector<float> sine(std::size_t n,double hz,float amp){std::vector<float>x(n);for(std::size_t i=0;i<n;++i)x[i]=amp*std::sin(2.0*hs_measure::kPi*hz*i/48000.0);return x;}
static std::vector<float> two_tone(std::size_t n,double a,double b){std::vector<float>x(n);for(std::size_t i=0;i<n;++i)x[i]=0.30f*std::sin(2.0*hs_measure::kPi*a*i/48000.0)+0.30f*std::sin(2.0*hs_measure::kPi*b*i/48000.0);return x;}

int main(){
  // TIME/CUTOFF contract: LP endpoint must open strongly as cutoff rises.
  MODFX_INIT(0,0); settle(0.08f,0.0f); auto hi=sine(96000,5000.0,0.35f); auto lp_closed=hs_measure::channel(process_mono(hi),0); double p_closed=hs_measure::tone_power(lp_closed,5000.0,48000.0,4096);
  MODFX_INIT(0,0); settle(0.92f,0.0f); auto lp_open=hs_measure::channel(process_mono(hi),0); double p_open=hs_measure::tone_power(lp_open,5000.0,48000.0,4096);
  require(p_open>p_closed*8.0+1e-7,"CUTOFF does not materially open LP endpoint");

  // VECTOR endpoint contract: LP must favor lows, HP must favor highs on same source/cutoff.
  auto mix=two_tone(120000,220.0,6000.0);
  MODFX_INIT(0,0); settle(0.38f,0.0f); auto lp=hs_measure::channel(process_mono(mix),0); double lp_lo=hs_measure::tone_power(lp,220.0,48000.0,4096), lp_hi=hs_measure::tone_power(lp,6000.0,48000.0,4096);
  MODFX_INIT(0,0); settle(0.38f,1.0f); auto hp=hs_measure::channel(process_mono(mix),0); double hp_lo=hs_measure::tone_power(hp,220.0,48000.0,4096), hp_hi=hs_measure::tone_power(hp,6000.0,48000.0,4096);
  require(lp_lo>lp_hi*3.0,"LP VECTOR endpoint does not favor low-frequency energy");
  require(hp_hi>hp_lo*3.0,"HP VECTOR endpoint does not favor high-frequency energy");

  // Mid VECTOR must expose the resonant band-pass identity near its computed cutoff.
  const float c=0.36f; const double fc=28.0+10972.0*c*c;
  auto tone=sine(96000,fc,0.25f);
  MODFX_INIT(0,0); settle(c,0.0f); auto end_lp=hs_measure::channel(process_mono(tone),0); double e_lp=hs_measure::stats(end_lp,4096).rms;
  MODFX_INIT(0,0); settle(c,0.5f); auto mid=hs_measure::channel(process_mono(tone),0); double e_mid=hs_measure::stats(mid,4096).rms;
  require(e_mid>e_lp*1.08,"VECTOR midpoint lacks intended resonant BP emphasis");

  // Stereo identity: identical stereo input remains identical; state does not spontaneously diverge.
  MODFX_INIT(0,0); settle(0.55f,0.5f); auto st=process_mono(sine(48000,997.0,0.5f)); auto l=hs_measure::channel(st,0),r=hs_measure::channel(st,1);
  require(hs_measure::max_abs_diff(l,r)<2e-6,"identical stereo input diverged unexpectedly");

  // Lifecycle: suspend/resume clears filter memory; silence after reset stays silent.
  MODFX_INIT(0,0); settle(0.5f,0.5f); (void)process_mono(sine(8192,330.0,0.8f)); MODFX_SUSPEND(); MODFX_RESUME(); std::vector<float> silence(8192,0.0f); auto silent=hs_measure::channel(process_mono(silence),0);
  require(hs_measure::stats(silent).peak<1e-7,"suspend/resume left residual filter state");

  // Hot/control-sweep soak: all vector/cutoff combinations remain finite and bounded.
  MODFX_INIT(0,0); std::vector<float> in(128),out(128),sub(128),sy(128);
  for(int block=0;block<7000;++block){set_controls(((block*29)%101)/100.0f,((block*47)%101)/100.0f);for(int i=0;i<64;++i){float v=0.92f*std::sin(2.0*hs_measure::kPi*(73.0+(block%23)*151.0)*(block*64+i)/48000.0);in[2*i]=v;in[2*i+1]=-0.85f*v;}MODFX_PROCESS(in.data(),out.data(),sub.data(),sy.data(),64);for(float v:out){require(std::isfinite(v),"non-finite output during soak");require(std::fabs(v)<=1.001f,"output bound exceeded during soak");}}

  std::puts("VECTORFILTER A-class project-specific harness PASS"); return 0;
}
