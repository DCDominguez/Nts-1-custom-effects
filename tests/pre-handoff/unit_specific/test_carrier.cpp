#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include "tests/pre-handoff/common/measure.hpp"
#include "effects/carrier/nts1/src/carrier.cpp"

static void require(bool ok, const char* msg) {
  if (!ok) { std::fprintf(stderr, "CARRIER FAIL: %s\n", msg); std::exit(2); }
}

static int32_t q31p(float x) {
  x = std::max(0.0f, std::min(0.999999f, x));
  return static_cast<int32_t>(x * 2147483647.0f);
}

static void set_controls(float time, float depth) {
  MODFX_PARAM(k_user_modfx_param_time, q31p(time));
  MODFX_PARAM(k_user_modfx_param_depth, q31p(depth));
}

static void settle(float time, float depth, int blocks = 180) {
  set_controls(time, depth);
  std::vector<float> z(128,0.0f), y(128,0.0f), s(128,0.0f), sy(128,0.0f);
  for (int b=0;b<blocks;++b) MODFX_PROCESS(z.data(),y.data(),s.data(),sy.data(),64);
}

static std::vector<float> process_constant(std::size_t frames, float l, float r) {
  std::vector<float> in(frames*2), out(frames*2), sub(frames*2), sy(frames*2);
  for (std::size_t i=0;i<frames;++i) { in[2*i]=l; in[2*i+1]=r; }
  std::size_t pos=0;
  while (pos<frames) {
    const uint32_t n=static_cast<uint32_t>(std::min<std::size_t>(64,frames-pos));
    MODFX_PROCESS(in.data()+2*pos,out.data()+2*pos,sub.data()+2*pos,sy.data()+2*pos,n);
    pos+=n;
  }
  return out;
}

static std::vector<float> process_signal(const std::vector<float>& mono) {
  std::vector<float> in(mono.size()*2), out(mono.size()*2), sub(mono.size()*2), sy(mono.size()*2);
  for (std::size_t i=0;i<mono.size();++i) in[2*i]=in[2*i+1]=mono[i];
  std::size_t pos=0;
  while (pos<mono.size()) {
    const uint32_t n=static_cast<uint32_t>(std::min<std::size_t>(64,mono.size()-pos));
    MODFX_PROCESS(in.data()+2*pos,out.data()+2*pos,sub.data()+2*pos,sy.data()+2*pos,n);
    pos+=n;
  }
  return out;
}

int main() {
  // DEPTH=0 must be a true dry contract.
  MODFX_INIT(0,0);
  settle(0.73f,0.0f);
  std::vector<float> src(16384);
  for (std::size_t i=0;i<src.size();++i) src[i]=0.42f*std::sin(2.0*hs_measure::kPi*997.0*i/48000.0);
  auto dry=hs_measure::channel(process_signal(src),0);
  require(hs_measure::max_abs_diff(src,dry)<2e-6,"DEPTH=0 is not dry");

  // TIME must control actual carrier frequency. At ring depth, DC input exposes the carrier directly.
  MODFX_INIT(0,0);
  const float t=0.75f;
  settle(t,1.0f,260);
  auto ring=process_constant(96000,0.5f,0.5f);
  auto ringL=hs_measure::channel(ring,0);
  const double measured=hs_measure::zero_cross_frequency(ringL,48000.0,4096,80000);
  const double expected=carrier_hz(t);
  require(measured>0.0 && std::fabs(measured-expected)/expected<0.035,"TIME carrier frequency outside tolerance");

  // DEPTH midpoint is AM: positive DC source must stay essentially non-negative.
  MODFX_INIT(0,0);
  settle(0.72f,0.5f,260);
  auto am=hs_measure::channel(process_constant(48000,0.6f,0.6f),0);
  float am_min=1.0f;
  for(float v:am) am_min=std::min(am_min,v);
  require(am_min>-0.01f,"DEPTH midpoint no longer behaves as AM");

  // Full depth is ring modulation: same positive source must cross both polarities.
  MODFX_INIT(0,0);
  settle(0.72f,1.0f,260);
  auto rm=hs_measure::channel(process_constant(48000,0.6f,0.6f),0);
  float mn=1.0f,mx=-1.0f;
  for(float v:rm){mn=std::min(mn,v);mx=std::max(mx,v);}
  require(mn<-0.20f && mx>0.20f,"full DEPTH lacks bipolar ring-modulation output");

  // Ring region must create the documented stereo phase split without dropping either channel.
  MODFX_INIT(0,0);
  settle(0.70f,1.0f,260);
  auto stereo=process_constant(96000,0.45f,0.45f);
  auto l=hs_measure::channel(stereo,0), r=hs_measure::channel(stereo,1);
  const auto ls=hs_measure::stats(l), rs=hs_measure::stats(r);
  require(ls.rms>0.05 && rs.rms>0.05,"stereo ring path dropped a channel");
  require(hs_measure::normalized_correlation(l,r)<0.90,"ring region lacks intended stereo phase separation");
  require(ls.peak<=1.001 && rs.peak<=1.001,"output exceeded bounds");

  // Suspend/resume must reset phase deterministically and must not leak stale input history.
  MODFX_INIT(0,0);
  settle(0.70f,1.0f,220);
  MODFX_SUSPEND(); MODFX_RESUME();
  auto a=process_constant(4096,0.4f,0.4f);
  MODFX_SUSPEND(); MODFX_RESUME();
  auto b=process_constant(4096,0.4f,0.4f);
  require(hs_measure::max_abs_diff(a,b)<1e-6,"phase reset is not deterministic");

  // Control-abuse soak: sweep TIME/DEPTH through oversampled and ordinary paths.
  MODFX_INIT(0,0);
  std::vector<float> in(128),out(128),sub(128),sy(128);
  for(int block=0;block<7000;++block){
    set_controls(((block*37)%101)/100.0f,((block*61)%101)/100.0f);
    for(int i=0;i<64;++i){
      float v=0.7f*std::sin(2.0*hs_measure::kPi*(97.0+(block%19)*113.0)*(block*64+i)/48000.0);
      in[2*i]=v; in[2*i+1]=v*0.83f;
    }
    MODFX_PROCESS(in.data(),out.data(),sub.data(),sy.data(),64);
    for(float v:out){require(std::isfinite(v),"non-finite output during soak");require(std::fabs(v)<=1.001f,"out-of-range output during soak");}
  }

  std::puts("CARRIER A-class project-specific harness PASS");
  return 0;
}
