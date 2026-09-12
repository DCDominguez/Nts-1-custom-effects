#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include "tests/pre-handoff/common/measure.hpp"
#include "effects/lattice-core/nts1/src/lattice_core.cpp"

static void require(bool ok,const char* msg){if(!ok){std::fprintf(stderr,"LATTICE CORE FAIL: %s\n",msg);std::exit(2);}}
static int32_t q31p(float x){x=std::max(0.0f,std::min(0.999999f,x));return static_cast<int32_t>(x*2147483647.0f);}
static void set_controls(float time,float pattern){MODFX_PARAM(k_user_modfx_param_time,q31p(time));MODFX_PARAM(k_user_modfx_param_depth,q31p(pattern));}
static void run_frames(std::size_t frames,float amp,double hz,std::vector<float>* capture=nullptr){std::vector<float> in(128),out(128);if(capture)capture->clear();std::size_t done=0;while(done<frames){uint32_t n=static_cast<uint32_t>(std::min<std::size_t>(64,frames-done));in.resize(n*2);out.assign(n*2,0.0f);for(uint32_t i=0;i<n;++i){float v=amp*std::sin(2.0*hs_measure::kPi*hz*(done+i)/48000.0);in[2*i]=v;in[2*i+1]=v;}std::vector<float> sub(n*2),sy(n*2);MODFX_PROCESS(in.data(),out.data(),sub.data(),sy.data(),n);if(capture)capture->insert(capture->end(),out.begin(),out.end());done+=n;}}
static std::vector<float> render_patch(float time,float pattern){MODFX_INIT(0,0);set_controls(0.0f,pattern);run_frames(96000,0.40f,233.0);set_controls(time,pattern);run_frames(96000,0.40f,233.0);std::vector<float> out;run_frames(96000,0.40f,233.0,&out);return out;}

static double capture_filter_gain(double hz){
  reset_state();
  double in_e=0.0,out_e=0.0;
  const int warm=12000,total=60000;
  for(int i=0;i<total;++i){
    const float x=0.5f*std::sin(2.0*hs_measure::kPi*hz*i/48000.0);
    const float y=filter_capture(x);
    if(i>=warm){in_e+=(double)x*x;out_e+=(double)y*y;}
  }
  return std::sqrt(out_e/in_e);
}

static void run_bright_register(std::size_t frames,float time,float pattern){
  MODFX_INIT(0,0);set_controls(0.0f,pattern);
  std::vector<float> in(128),out(128),sub(128),sy(128);
  std::size_t done=0;
  while(done<96000){uint32_t n=(uint32_t)std::min<std::size_t>(64,96000-done);for(uint32_t i=0;i<n;++i){const double t=(double)(done+i)/48000.0;float v=0.0f;for(int h=1;h<=8;++h)v+=(0.22f/(float)h)*std::sin(2.0*hs_measure::kPi*(2200.0*h)*t);in[2*i]=in[2*i+1]=v;}MODFX_PROCESS(in.data(),out.data(),sub.data(),sy.data(),n);done+=n;}
  set_controls(time,pattern);done=0;
  while(done<frames){uint32_t n=(uint32_t)std::min<std::size_t>(64,frames-done);for(uint32_t i=0;i<n;++i){const double t=(double)(96000+done+i)/48000.0;float v=0.0f;for(int h=1;h<=8;++h)v+=(0.22f/(float)h)*std::sin(2.0*hs_measure::kPi*(2200.0*h)*t);in[2*i]=in[2*i+1]=v;}MODFX_PROCESS(in.data(),out.data(),sub.data(),sy.data(),n);for(uint32_t i=0;i<n*2;++i){require(std::isfinite(out[i]),"bright-register path produced non-finite output");require(std::fabs(out[i])<=0.996f,"bright-register path exceeded output bound");}done+=n;}
}

int main(){
  // TIME bottom region is a true disengaged/dry region for normal-level source.
  MODFX_INIT(0,0); set_controls(0.0f,0.0f); std::vector<float> dry; run_frames(48000,0.35f,997.0,&dry); auto dl=hs_measure::channel(dry,0); std::vector<float> ref(dl.size()); for(std::size_t i=0;i<ref.size();++i)ref[i]=0.35f*std::sin(2.0*hs_measure::kPi*997.0*i/48000.0); require(hs_measure::max_abs_diff(dl,ref)<3e-5,"TIME=0 is not effectively dry"); require(voice_count(0.0f)==0,"TIME=0 does not map to zero voices");

  // Voice-population contract: 0.3-2 remains bounded 0..10 and rises with TIME.
  require(kMaxVoices==10u,"CORE 0.3-2 ordinary voice ceiling is not ten"); require(voice_count(0.04f)>=1,"TIME just above disengage did not enable first voice"); require(voice_count(0.50f)>voice_count(0.20f),"TIME does not increase voice population"); require(voice_count(0.90f)==10u,"high TIME did not reach ten-voice ceiling"); require(voice_count(1.0f)==10u,"maximum TIME exceeded or missed ten-voice ceiling");

  // High-register regression discovered in physical MkI testing. Stochastic
  // octave extension must never drive ordinary microloops faster than +12 st.
  reset_state();
  for(uint32_t p=0;p<8u;++p){for(uint32_t e=1;e<=1024u;++e){const int semi=modulated_interval(p,e,1.0f);require(semi<=12,"stochastic interval exceeded +12-semitone playback ceiling");require(semi>=-24,"stochastic interval exceeded -24-semitone lower bound");}}

  // The wet-history anti-alias conditioner must leave low/mid material almost
  // intact while materially reducing near-Nyquist energy before rate-up reads.
  const double g3=capture_filter_gain(3000.0),g18=capture_filter_gain(18000.0);
  require(g3>0.95,"capture conditioner removes too much 3 kHz material");
  require(g18<0.15,"capture conditioner does not sufficiently attenuate 18 kHz material");
  require(g18<g3*0.16,"capture conditioner lacks useful high-frequency discrimination");

  // Audible identity: moderate TIME must produce a measurable processed field and stereo movement.
  auto mid=render_patch(0.58f,0.0f); auto ml=hs_measure::channel(mid,0),mr=hs_measure::channel(mid,1); std::vector<float> midref(ml.size()); for(std::size_t i=0;i<midref.size();++i)midref[i]=0.40f*std::sin(2.0*hs_measure::kPi*233.0*i/48000.0); require(hs_measure::max_abs_diff(ml,midref)>0.01,"moderate TIME produced no material loop field"); require(hs_measure::max_abs_diff(ml,mr)>0.005,"loop field lacks stereo placement");

  // PATTERN must change the audible result under otherwise identical deterministic initialization.
  auto plain=render_patch(0.58f,0.0f); auto human=render_patch(0.58f,1.0f); require(hs_measure::max_abs_diff(plain,human)>0.01,"PATTERN does not materially change rendered field");

  // High TIME reaches the runtime ceiling and remains finite/bounded in host execution. This does NOT certify MkI callback/CPU margin.
  MODFX_INIT(0,0); set_controls(0.0f,0.75f); run_frames(90000,0.55f,147.0); set_controls(0.90f,0.75f); std::vector<float> high; run_frames(240000,0.55f,147.0,&high); for(float v:high){require(std::isfinite(v),"non-finite output at high TIME");require(std::fabs(v)<=0.996f,"high TIME exceeded limiter bound");}

  // Full-path high-register/harmonic-rich stress added after the physical report.
  run_bright_register(240000,0.90f,1.0f);

  // Maximum TIME must engage recent-history freeze and stop history writes.
  MODFX_INIT(0,0); set_controls(0.0f,0.5f); run_frames(90000,0.45f,211.0); const uint32_t before=s_write; set_controls(1.0f,0.5f); run_frames(24000,0.45f,211.0); require(s_freeze_active,"maximum TIME did not engage freeze"); const uint32_t frozen_write=s_write; require(frozen_write>=before||before-frozen_write<kBufferSize,"invalid write index after freeze");

  // 0.3-2 preserves the runtime contract: once freeze is active, ordinary microloop state must stop advancing/scheduling.
  const uint32_t frozen_events=s_event_counter;
  uint32_t repeats[kMaxVoices]; uint32_t waits[kMaxVoices]; float phases[kMaxVoices];
  for(uint32_t i=0;i<kMaxVoices;++i){repeats[i]=s_voice[i].repeats_left;waits[i]=s_voice[i].wait_samples;phases[i]=s_voice[i].phase;}
  std::vector<float> frozen; run_frames(96000,0.0f,211.0,&frozen);
  require(s_write==frozen_write,"history continued writing while freeze active"); require(s_event_counter==frozen_events,"ordinary microloop events were scheduled during freeze");
  for(uint32_t i=0;i<kMaxVoices;++i){require(s_voice[i].repeats_left==repeats[i],"ordinary repeat state advanced during freeze");require(s_voice[i].wait_samples==waits[i],"ordinary wait state advanced during freeze");require(s_voice[i].phase==phases[i],"ordinary phase advanced during freeze");}
  auto fl=hs_measure::stats(hs_measure::channel(frozen,0),12000); require(fl.rms>0.01,"freeze failed to replay captured history into silence");

  // Freeze release must resume history acquisition and ordinary engine work.
  set_controls(0.80f,0.5f); uint32_t w0=s_write; run_frames(4096,0.3f,311.0); require(!s_freeze_active,"freeze hysteresis did not release below threshold"); require(s_write!=w0,"history acquisition did not resume after freeze release");

  // Suspend/resume clears history and cannot emit stale audio into silence.
  MODFX_SUSPEND(); MODFX_RESUME(); set_controls(0.8f,0.2f); std::vector<float> silence; run_frames(30000,0.0f,100.0,&silence); require(hs_measure::stats(hs_measure::channel(silence,0)).peak<1e-7,"lifecycle reset emitted stale history");

  // Long parameter abuse/soak including repeated freeze crossings.
  MODFX_INIT(0,0); std::vector<float> in(128),out(128),sub(128),sy(128); for(int block=0;block<9000;++block){float t=((block*17)%101)/100.0f; if((block%503)>470)t=1.0f; set_controls(t,((block*43)%101)/100.0f);for(int i=0;i<64;++i){float v=0.60f*std::sin(2.0*hs_measure::kPi*(83.0+(block%19)*59.0)*(block*64+i)/48000.0);in[2*i]=v;in[2*i+1]=0.9f*v;}MODFX_PROCESS(in.data(),out.data(),sub.data(),sy.data(),64);for(float v:out){require(std::isfinite(v),"non-finite output during soak");require(std::fabs(v)<=0.996f,"output bound exceeded during soak");}}

  std::puts("LATTICE CORE 0.3-2 A-class project-specific harness PASS (high-register physical MkI retest required)"); return 0;
}
