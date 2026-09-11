#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "tests/pre-handoff/common/measure.hpp"
#include "effects/lattice-echo/nts1/src/lattice_echo.cpp"

static void req(bool ok,const char*m){if(!ok){std::fprintf(stderr,"LATTICE ECHO FAIL: %s\n",m);std::exit(2);}}
static int32_t q(float x){x=std::max(0.f,std::min(.999999f,x));return(int32_t)(x*2147483647.f);}
static void set(float clock,float mode,float mix){DELFX_PARAM(k_user_delfx_param_time,q(clock));DELFX_PARAM(k_user_delfx_param_depth,q(mode));DELFX_PARAM(k_user_delfx_param_shift_depth,q(mix));}
static void proc(std::vector<float>&x){std::size_t p=0,n=x.size()/2;while(p<n){uint32_t k=(uint32_t)std::min<std::size_t>(64,n-p);DELFX_PROCESS(x.data()+2*p,k);p+=k;}}
static void silence(std::size_t n){std::vector<float>z(n*2);proc(z);}
static std::vector<float> phrase(std::size_t n){std::vector<float>x(n*2);for(std::size_t i=0;i<n;i++){double f=180.0+((i/600)%7)*61.0;float v=.34f*std::sin(2.0*hs_measure::kPi*f*i/48000.0)+.12f*std::sin(2.0*hs_measure::kPi*997.0*i/48000.0);if((i%1300)<90)v+=.18f*(1.f-(i%1300)/90.f);x[2*i]=v;x[2*i+1]=.87f*v;}return x;}
static std::vector<float> render(float clock,float mode,float mix,std::size_t src=7000,std::size_t tail=70000,float bpm=120.f){DELFX_INIT(0,0);test_bpm=bpm;set(clock,mode,mix);silence(8000);auto x=phrase(src);std::vector<float>all((src+tail)*2);std::copy(x.begin(),x.end(),all.begin());proc(all);return all;}
static uint32_t active_voices(){uint32_t n=0;for(uint32_t i=0;i<kPlaybackVoices;i++)if(s_voice[i].active)++n;return n;}
static uint32_t valid_slots(){uint32_t n=0;for(uint32_t i=0;i<kFragmentSlots;i++)if(s_slot[i].valid)++n;return n;}

int main(){
 // An isolated source phrase must be admitted, captured and audibly replayed after the source ends.
 DELFX_INIT(0,0);test_bpm=120;set(.56f,.10f,1.f);silence(8000);auto src=phrase(7000);proc(src);req(valid_slots()>=1||s_capturing,"source phrase was not admitted/captured");std::vector<float>tail(70000*2);proc(tail);auto tl=hs_measure::channel(tail,0);req(hs_measure::stats(tl).peak>.02,"captured phrase did not reach audible delayed output");req(s_tick_counter>1,"clock scheduler did not advance");

 // The four playback modes must produce measurably different phrase geometry from the same deterministic input.
 auto fwd=render(.56f,.10f,1.f);auto rev=render(.56f,.36f,1.f);auto ping=render(.56f,.62f,1.f);auto stut=render(.56f,.90f,1.f);req(hs_measure::max_abs_diff(fwd,rev)>.01,"REVERSE is not distinct from FORWARD");req(hs_measure::max_abs_diff(fwd,ping)>.01,"PING-PONG is not distinct from FORWARD");req(hs_measure::max_abs_diff(fwd,stut)>.01,"STUTTER is not distinct from FORWARD");

 // CLOCK/tempo must move event timing and the fast STUTTER mode must obey its stricter voice budget.
 DELFX_INIT(0,0);test_bpm=120;set(.56f,.90f,1.f);silence(16000);uint32_t tick120=s_tick_samples;req(s_voice_limit<=10,"STUTTER exceeded documented voice budget");DELFX_INIT(0,0);test_bpm=240;set(.56f,.90f,1.f);silence(16000);uint32_t tick240=s_tick_samples;req(tick240<tick120,"tempo does not shorten CLOCK timing");

 // A single capture must be finite: all generations, voices and wet output return to rest without new source.
 DELFX_INIT(0,0);test_bpm=120;set(.99f,.10f,1.f);silence(8000);auto one=phrase(6500);proc(one);std::vector<float>longTail(21*48000*2);proc(longTail);req(valid_slots()==0,"finite generations left a valid fragment alive");req(active_voices()==0,"finite phrase left playback voices alive");auto lt=hs_measure::channel(longTail,0);req(hs_measure::stats(lt,20*48000,48000).peak<1e-5,"echo engine failed to return to silence");

 // Suspend/resume clears capture ownership and audio state.
 DELFX_SUSPEND();DELFX_RESUME();req(valid_slots()==0&&active_voices()==0&&!s_capturing,"lifecycle reset did not clear capture/voice state");std::vector<float>z(10000*2);proc(z);req(hs_measure::stats(hs_measure::channel(z,0)).peak<1e-7,"reset emitted stale fragment audio");

 // Dense deterministic soak exercises onset/fallback capture, wraps, all modes and control boundaries.
 DELFX_INIT(0,0);test_bpm=120;std::vector<float>b(128);for(int bb=0;bb<9000;bb++){set(((bb*13)%101)/100.f,((bb*37)%101)/100.f,((bb*71)%101)/100.f);if(bb%900==0)test_bpm=(bb/900)%2?90.f:180.f;for(int i=0;i<64;i++){uint32_t n=bb*64+i;float v=((n%9000)<4200)?(.76f*std::sin(2.0*hs_measure::kPi*(127+(bb%11)*83.0)*n/48000.0)):0.f;b[2*i]=v;b[2*i+1]=.82f*v;}DELFX_PROCESS(b.data(),64);for(float v:b){req(std::isfinite(v),"non-finite soak output");req(std::fabs(v)<=1.001,"soak output exceeded bounds");}req(active_voices()<=kPlaybackVoices,"voice pool overflowed");}
 std::puts("LATTICE ECHO A-class project-specific harness PASS");return 0;
}
