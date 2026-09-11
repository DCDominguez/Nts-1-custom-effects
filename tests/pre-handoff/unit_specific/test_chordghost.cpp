#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include "tests/pre-handoff/common/measure.hpp"
#include "effects/chordghost/nts1/src/chordghost.cpp"

static void require(bool ok,const char* msg){if(!ok){std::fprintf(stderr,"CHORDGHOST FAIL: %s\n",msg);std::exit(2);}}
static int32_t q31p(float x){x=std::max(0.0f,std::min(0.999999f,x));return static_cast<int32_t>(x*2147483647.0f);}
static void set_mix(float x){DELFX_PARAM(k_user_delfx_param_shift_depth,q31p(x));}
static void set_div(float x){DELFX_PARAM(k_user_delfx_param_time,q31p(x));}
static void set_chord_code(uint8_t code){DELFX_PARAM(k_user_delfx_param_depth,q31p(static_cast<float>(code)/127.0f));}
static void process(std::vector<float>& x){std::size_t p=0;while(p<x.size()/2){uint32_t n=static_cast<uint32_t>(std::min<std::size_t>(64,x.size()/2-p));DELFX_PROCESS(x.data()+2*p,n);p+=n;}}
static std::vector<float> impulse(std::size_t frames,float amp=0.7f){std::vector<float>x(frames*2,0.0f);x[0]=x[1]=amp;return x;}
static std::vector<float> stereo_sine(std::size_t frames,double hz,float amp){std::vector<float>x(frames*2);for(std::size_t i=0;i<frames;++i){float v=amp*std::sin(2.0*hs_measure::kPi*hz*i/48000.0);x[2*i]=v;x[2*i+1]=v;}return x;}
static std::size_t first_wet_sample(const std::vector<float>& x,double threshold=0.02){auto l=hs_measure::channel(x,0);for(std::size_t i=1;i<l.size();++i)if(std::fabs(l[i])>threshold)return i;return l.size();}
static void settle_delay(int blocks=1200){std::vector<float> z(128,0.0f);for(int i=0;i<blocks;++i)DELFX_PROCESS(z.data(),64);}

int main(){
  // MIX endpoints: zero is dry, full mix removes the direct impulse.
  DELFX_INIT(0,0); test_bpm=120.0f; set_div(0.50f); set_mix(0.0f); auto dry=stereo_sine(12000,701.0,0.35f); auto src=dry; process(dry); require(hs_measure::max_abs_diff(src,dry)<2e-6,"MIX=0 is not dry");
  DELFX_INIT(0,0); test_bpm=120.0f; set_div(0.50f); set_mix(1.0f); auto wet0=impulse(2048); process(wet0); require(hs_measure::stats(hs_measure::channel(wet0,0)).peak<1e-5,"MIX=1 leaked direct impulse before delay");

  // TIME division contract at 120 BPM. Settle the slewed head before the impulse, then verify audible first echo timing.
  const float controls[3]={0.01f,0.51f,0.99f};
  const double multipliers[3]={0.25,1.0,2.0};
  for(int k=0;k<3;++k){
    DELFX_INIT(0,0); test_bpm=120.0f; set_div(controls[k]); set_mix(1.0f); settle_delay();
    const std::size_t expected=static_cast<std::size_t>(24000.0*multipliers[k]);
    auto x=impulse(expected+6000); process(x); const std::size_t first=first_wet_sample(x,0.03);
    require(first<x.size()/2,"TIME division produced no audible echo");
    const double err=std::fabs(static_cast<double>(first)-expected)/std::max<double>(1.0,expected);
    require(err<0.035,"TIME division first echo timing outside tolerance");
  }

  // BPM contract: the same quarter-note division must halve its sample delay when tempo doubles.
  DELFX_INIT(0,0); set_div(0.51f); set_mix(1.0f); test_bpm=120.0f; settle_delay(); auto q120=impulse(30000); process(q120); auto d120=first_wet_sample(q120);
  DELFX_INIT(0,0); set_div(0.51f); set_mix(1.0f); test_bpm=240.0f; settle_delay(); auto q240=impulse(18000); process(q240); auto d240=first_wet_sample(q240);
  require(d120>0&&d240>0&&std::fabs(static_cast<double>(d120)/(2.0*d240)-1.0)<0.05,"tempo change does not scale delay timing correctly");

  // M2 harmonic-state contract: valid codes update state/generation; HOLD/reserved codes preserve state.
  DELFX_INIT(0,0); uint32_t g0=s_chord_generation; set_chord_code(0); require(s_chord_state.root==0&&s_chord_state.quality==0,"major C code did not decode"); set_chord_code(19); require(s_chord_generation>g0&&s_chord_state.root==7&&s_chord_state.quality==1,"valid chord code failed to update state"); auto held=s_chord_state; uint32_t gh=s_chord_generation; set_chord_code(121); require(s_chord_generation==gh&&s_chord_state.code==held.code,"HOLD code changed harmonic state"); set_chord_code(127); require(s_chord_generation==gh&&s_chord_state.code==held.code,"reserved code changed harmonic state"); set_chord_code(120); require(s_chord_state.harmonic_bypass,"bypass code did not set harmonic bypass");

  // Current M2 audio path deliberately remains harmonically neutral: chord-code changes must not alter the delay waveform.
  DELFX_INIT(0,0); test_bpm=120.0f; set_div(0.51f); set_mix(0.7f); set_chord_code(0); auto a=stereo_sine(70000,330.0,0.25f); process(a);
  DELFX_INIT(0,0); test_bpm=120.0f; set_div(0.51f); set_mix(0.7f); set_chord_code(67); auto b=stereo_sine(70000,330.0,0.25f); process(b);
  require(hs_measure::max_abs_diff(a,b)<1e-6,"M2 chord-state channel unexpectedly alters neutral audio path");

  // Feedback/tail must decay and suspend/resume must clear delay RAM while preserving valid harmonic state.
  DELFX_INIT(0,0); test_bpm=180.0f; set_div(0.51f); set_mix(1.0f); set_chord_code(31); settle_delay(); auto tail=impulse(160000,0.6f); process(tail); auto l=hs_measure::channel(tail,0); double early=hs_measure::stats(l,12000,50000).rms; double late=hs_measure::stats(l,110000,45000).rms; require(early>late*2.0,"feedback tail does not decay materially");
  const uint8_t saved=s_chord_state.code; DELFX_SUSPEND(); DELFX_RESUME(); require(s_chord_state.code==saved,"suspend/resume lost valid chord state"); std::vector<float> silence(50000*2,0.0f); process(silence); require(hs_measure::stats(hs_measure::channel(silence,0)).peak<1e-7,"suspend/resume left stale delay audio");

  // Long wrap/control soak.
  DELFX_INIT(0,0); test_bpm=137.0f; std::vector<float> block(128);
  for(int bidx=0;bidx<7000;++bidx){set_div(((bidx*17)%101)/100.0f);set_mix(((bidx*43)%101)/100.0f);if((bidx%97)==0)set_chord_code(static_cast<uint8_t>((bidx/97)%121));for(int i=0;i<64;++i){float v=0.5f*std::sin(2.0*hs_measure::kPi*(103.0+(bidx%13)*47.0)*(bidx*64+i)/48000.0);block[2*i]=v;block[2*i+1]=0.8f*v;}DELFX_PROCESS(block.data(),64);for(float v:block){require(std::isfinite(v),"non-finite output during soak");require(std::fabs(v)<=1.001f,"output bound exceeded during soak");}}

  std::puts("CHORDGHOST A-class project-specific harness PASS"); return 0;
}
