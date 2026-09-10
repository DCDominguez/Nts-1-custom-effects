#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <algorithm>
#define LATTICE_TEST
#include "../../effects/lattice-field/nts1/src/lattice_field.cpp"

void check(bool ok, const char *name) { if (!ok) { std::fprintf(stderr,"FAIL: %s\n",name); std::exit(1); } }
void params(float clock, float mode, float mix) {
  DELFX_PARAM(k_user_delfx_param_time, static_cast<int32_t>(clock * 2147483000.0f));
  DELFX_PARAM(k_user_delfx_param_depth, static_cast<int32_t>(mode * 2147483000.0f));
  DELFX_PARAM(k_user_delfx_param_shift_depth, static_cast<int32_t>(mix * 2147483000.0f));
}
float frame(float l, float r) {
  float b[2] = {l,r}; DELFX_PROCESS(b,1);
  check(std::isfinite(b[0]) && std::isfinite(b[1]),"finite output");
  check(std::fabs(b[0]) <= 1.0f && std::fabs(b[1]) <= 1.0f,"bounded output");
  check(liveCount() <= 8u,"hard voice ceiling");
  return std::max(std::fabs(b[0]),std::fabs(b[1]));
}
float tone(uint32_t n, float hz = 220.0f) { return 0.4f*std::sin(6.28318530718f*hz*n/48000.0f); }
void silence(uint32_t n) { for (uint32_t i=0;i<n;++i) frame(0,0); }
int main() {
  // Dry identity and stereo preservation, including antiphase source.
  DELFX_INIT(0,0); params(.56f,.1f,0); DELFX_RESUME();
  for (uint32_t n=0;n<4800;++n) {
    float l=tone(n), r=-l*.7f, b[2]={l,r}; DELFX_PROCESS(b,1);
    check(b[0]==l && b[1]==r,"exact dry stereo spine");
  }
  std::puts("PASS dry identity/stereo");
  // Every division/mode terminates with no spontaneous recapture or stuck voice.
  for (uint32_t d=0;d<8;++d) for (uint32_t m=0;m<4;++m) {
    DELFX_INIT(0,0); params((d+.5f)/8,(m+.5f)/4,1); DELFX_RESUME();
    for (uint32_t n=0;n<6000;++n) frame(tone(n),tone(n));
    const uint32_t count=admitted;
    float wetPeak=0;
    const uint32_t duration=12*tickFor(d)+12*SR;
    for (uint32_t n=0;n<duration;++n) {
      const float p=frame(0,0); wetPeak=std::max(wetPeak,p);
      if (n>duration-4800) check(p<1e-6f,"bloom returns to silence");
    }
    check(count==1 && admitted==count,"one isolated note one seed");
    check(spawned==6,"six finite events");
    check(wetPeak>.08f,"audible wet response");
    check(seeds[0].state==Empty && seeds[1].state==Empty && !liveCount(),"empty lifecycle");
    if (d<2) check(maxLive<=4,"fast subdivision voice budget");
  }
  std::puts("PASS all 32 mode/division lifecycles, wet audibility and rest");
  // Held tone never gets a time-based recapture.
  DELFX_INIT(0,0); params(.56f,.1f,.5f); DELFX_RESUME();
  for (uint32_t n=0;n<12*SR;++n) frame(tone(n),tone(n));
  check(admitted==1,"held tone does not replenish seeds");
  silence(SR/2);
  for (uint32_t n=0;n<6000;++n) frame(tone(n),tone(n));
  check(admitted==2,"release and rearm accepts new note");
  std::puts("PASS sustained source and rearm");
  // Deadline remains fixed through knob/tempo changes and uint32 wrap.
  DELFX_INIT(0,0); now=0xfffff000u; params(.05f,.1f,.5f); DELFX_RESUME(); now=0xfffff000u;
  for (uint32_t n=0;n<4000;++n) frame(tone(n),tone(n));
  const uint32_t expiry=seeds[0].deadline, identity=seeds[0].id;
  params(.99f,.99f,1); test_bpm=30;
  silence(4000);
  check(seeds[0].id==identity && seeds[0].deadline==expiry,"fixed admission deadline");
  silence(14*SR); check(!liveCount(),"clock wrap expiry"); test_bpm=120;
  std::puts("PASS fixed deadline, knob latching and clock wrap");
  // Rapid inputs force retirement while old audio is sounding.
  DELFX_INIT(0,0); params(.8f,.1f,.7f); DELFX_RESUME();
  for (uint32_t n=0;n<8*SR;++n) {
    if (n%17000==0) params((n%5)*.23f,(n%7)*.15f,.65f);
    const float x=(n%9600<3600)?tone(n):0;
    frame(x,-.7f*x);
    for (uint32_t v=0;v<VOICES;++v) if (voices[v].active)
      check(seeds[voices[v].seed].id==voices[v].id,"capture ownership");
  }
  check(admitted>10,"dense input still admits new notes");
  silence(50*SR); check(!liveCount(),"dense sequence terminates");
  std::puts("PASS dense replacement and control sweeps");
  // Bad host tempo/input and corrupt individual voice cannot poison dry/output.
  DELFX_INIT(0,0); test_bpm=std::numeric_limits<float>::quiet_NaN();
  for(uint32_t n=0;n<6000;++n) frame(tone(n),tone(n));
  check(seeds[0].tick==12000,"NaN tempo fallback");
  frame(std::numeric_limits<float>::quiet_NaN(),std::numeric_limits<float>::infinity());
  silence(SR/2);
  for(uint32_t i=0;i<VOICES;++i) if(voices[i].active) voices[i].position=std::numeric_limits<float>::quiet_NaN();
  frame(.1f,.2f); test_bpm=120;
  params(.6f,.8f,.7f); DELFX_SUSPEND(); DELFX_RESUME();
  check(!liveCount() && serial==0 && mixValue==mixTarget,"lifecycle reset preserves controls");
  std::puts("PASS invalid-state containment and lifecycle reset");
  // Dense but ordinary material should not need the emergency wet guard.
  DELFX_INIT(0,0); params(.56f,.6f,.5f); DELFX_RESUME();
  float peak=0;
  for(uint32_t n=0;n<8*SR;++n) {
    const float x=(n%24000<6000)?tone(n):0;
    peak=std::max(peak,frame(x,x));
  }
  check(guardHits==0,"ordinary material avoids wet guard");
  std::printf("PASS ordinary-input headroom: output peak %.6f, guard hits %u\n",peak,guardHits);
  std::puts("ALL FIELD HOST TESTS PASSED (not MkI runtime or listening validation)");
}
