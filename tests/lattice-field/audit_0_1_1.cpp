// Diagnostic observations of unmodified FIELD 0.1-1. Not a CI pass/fail gate.
// Some component probes directly seed capture state to isolate playback behavior.
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <algorithm>
#define LATTICE_TEST
#include "../../effects/lattice-field/nts1/src/lattice_field.cpp"
const float pi = 3.14159265358979323846f;
void init(float clk=.56f,float mode=.1f,float mix=.5f) {
  DELFX_INIT(0,0); test_bpm=120;
  DELFX_PARAM(k_user_delfx_param_time,static_cast<int32_t>(clk*2147483000.f));
  DELFX_PARAM(k_user_delfx_param_depth,static_cast<int32_t>(mode*2147483000.f));
  DELFX_PARAM(k_user_delfx_param_shift_depth,static_cast<int32_t>(mix*2147483000.f));
  DELFX_RESUME();
}
void sample(float x,float y) { float b[2]={x,y}; DELFX_PROCESS(b,1); }
void input(float x) { sample(x,x); }
float sinus(unsigned n,float hz=220.f) {return std::sin(2*pi*hz*n/SR);}
float rmsCapture(unsigned s) {
  double e=0;for(unsigned n=0;n<seeds[s].length;++n)e+=capture[s][0][n]*capture[s][0][n];
  return std::sqrt(e/seeds[s].length);
}
uint64_t stateHash() {
  uint64_t h=1469598103934665603ull;
  for(unsigned c=0;c<4;++c)for(unsigned n=0;n<FDN;++n){uint32_t bits;std::memcpy(&bits,&field[c][n],4);h=(h^bits)*1099511628211ull;}
  return h;
}
int main(){
  std::puts("A: incoming C-D-E-like equal-amplitude pitch changes, each 500 ms");
  init();
  for(unsigned n=0;n<72000;++n){const float hz=n<24000?261.6256f:(n<48000?293.6648f:329.6276f);input(.4f*sinus(n,hz));if(n%24000==23999)std::printf("  segment %u admitted=%u spawned=%u\n",n/24000+1,admitted,spawned);}
  std::puts("B: clearly separated 125 ms notes, 500 ms onset spacing");
  init();for(unsigned n=0;n<72000;++n){input(n%24000<6000?.4f*sinus(n):0);if(n%24000==23999)std::printf("  segment %u admitted=%u spawned=%u\n",n/24000+1,admitted,spawned);}
  std::puts("C: 100 ms notes every 250 ms for 6 s at slow 1/2 CLOCK (1 s tick)");
  init(.99f);for(unsigned n=0;n<6*SR;++n)input(n%12000<4800?.4f*sinus(n):0);
  std::printf("  while playing: admitted=%u spawned=%u\n",admitted,spawned);
  for(unsigned n=0;n<2*SR;++n)input(0);
  std::printf("  after stopping +2s: admitted=%u spawned=%u\n",admitted,spawned);
  std::puts("D: held sine input detection vs amplitude (500 ms)");
  const float amplitudes[]={.005f,.01f,.02f,.05f,.4f};
  for(float a:amplitudes){init();for(unsigned n=0;n<24000;++n)input(a*sinus(n));std::printf("  peak=%.3f admitted=%u\n",a,admitted);}
  std::puts("E: 500 ms soft attack to .4 FS, inspect first capture before replacement");
  init();for(unsigned n=0;n<24000;++n)input(.4f*(n/24000.f)*sinus(n));
  std::printf("  first-capture RMS=%.6f vs full-level sine RMS=%.6f (%.2f dB); length=%u\n",rmsCapture(0),.4f/std::sqrt(2.f),20*std::log10(rmsCapture(0)/(.4f/std::sqrt(2.f))),seeds[0].length);
  std::puts("F: isolated Ping-Pong event, LEFT-ONLY stereo capture, left/right clear read gains");
  for(unsigned event=0;event<2;++event){init(.56f,.6f);Seed &s=seeds[0];s.state=Playing;s.id=1;s.mode=2;s.limit=8;s.length=4096;s.event=event;
    for(unsigned n=0;n<4096;++n){capture[0][0][n]=.4f*sinus(n);capture[0][1][n]=0;}
    spawn(0);Voice &v=voices[0];double l=0,r=0;
    for(unsigned n=96;n<3998;++n){float a=interpolated(0,0,n,s.length)*v.panL;float b=interpolated(0,1,n,s.length)*v.panR;l+=a*a;r+=b*b;}
    std::printf("  event=%u clear L_RMS=%.6f R_RMS=%.6f\n",event+1,std::sqrt(l/3902),std::sqrt(r/3902));}
  std::puts("G: isolated +12 ghost from an 18 kHz captured tone; inspect folded 12 kHz component");
  init();Seed &s=seeds[0];s.state=Playing;s.id=1;s.mode=0;s.limit=8;s.length=4096;s.event=5;
  for(unsigned n=0;n<4096;++n)capture[0][0][n]=capture[0][1][n]=.4f*sinus(n,18000);
  spawn(0);Voice &v=voices[0];double cs=0,sn=0;unsigned count=0;
  for(unsigned n=0;n<v.length;++n){float a=interpolated(0,0,v.position,s.length);v.lpL+=(a-v.lpL)*.45f;v.position+=v.ratio;
    if(n>128&&n<1900){cs+=v.lpL*std::cos(2*pi*12000*n/SR);sn+=v.lpL*std::sin(2*pi*12000*n/SR);++count;}}
  const float alias=2*std::sqrt(cs*cs+sn*sn)/count;
  std::printf("  12kHz amplitude before ghost envelope/level=%.6f (%.2f dB relative to .4 input)\n",alias,20*std::log10(alias/.4f));
  std::puts("H: block-size invariance across 1/16/32/64 frames");
  for(unsigned frames: {1u,16u,32u,64u}) {init();double energy=0;float b[128];for(unsigned n=0;n<96000;n+=frames){for(unsigned j=0;j<frames;++j)b[2*j]=b[2*j+1]=((n+j)%24000<6000)?.4f*sinus(n+j):0;DELFX_PROCESS(b,frames);for(unsigned j=0;j<2*frames;++j)energy+=b[j]*b[j];}std::printf("  block=%u output_energy=%.12f state_hash=%llu admitted=%u spawned=%u\n",frames,energy,(unsigned long long)stateHash(),admitted,spawned);}
  std::printf("I: host-shim MIX ID=%u; compare to real Korg header separately\n",unsigned(k_user_delfx_param_shift_depth));
}
