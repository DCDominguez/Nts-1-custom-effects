#include "stubs/userosc.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include <vector>

void OSC_INIT(uint32_t,uint32_t);
void OSC_CYCLE(const user_osc_param_t*,int32_t*,uint32_t);
void OSC_NOTEON(const user_osc_param_t*);
void OSC_NOTEOFF(const user_osc_param_t*);
void OSC_PARAM(uint16_t,uint16_t);
float host_test_bpm=120.0f;

static void die(const char*m){std::fprintf(stderr,"FAIL: %s\n",m);std::exit(1);} static void check(bool x,const char*m){if(!x)die(m);} 
static user_osc_param_t par(uint8_t note){user_osc_param_t p{};p.pitch=static_cast<uint16_t>(note)<<8;p.shape_lfo=0;return p;}
static void basic_sine_setup(){OSC_PARAM(k_user_osc_param_id1,0);OSC_PARAM(k_user_osc_param_id2,0);OSC_PARAM(k_user_osc_param_id3,0);OSC_PARAM(k_user_osc_param_id4,0);OSC_PARAM(k_user_osc_param_id5,0);OSC_PARAM(k_user_osc_param_id6,0);OSC_PARAM(k_user_osc_param_shape,0);OSC_PARAM(k_user_osc_param_shiftshape,0);}
static std::vector<float> render(uint8_t note,uint32_t frames){user_osc_param_t p=par(note);OSC_NOTEON(&p);std::vector<int32_t> q(frames);OSC_CYCLE(&p,q.data(),frames);std::vector<float> x(frames);for(uint32_t i=0;i<frames;++i){x[i]=q31_to_f32(q[i]);check(std::isfinite(x[i]),"non-finite oscillator output");check(std::fabs(x[i])<=1.001f,"oscillator output outside q31 range");}OSC_NOTEOFF(&p);return x;}
static float estimate_hz(const std::vector<float>&x){uint32_t cross=0;for(size_t i=2401;i<x.size();++i)if(x[i-1]<=0&&x[i]>0)++cross;float seconds=(x.size()-2401)/48000.0f;return seconds>0?cross/seconds:0;}
int main(int argc,char**argv){const char*name=argc>1?argv[1]:"osc";OSC_INIT(0,0);basic_sine_setup();
 for(uint8_t note: {uint8_t(48),uint8_t(60),uint8_t(69),uint8_t(81)}){auto x=render(note,48000);float got=estimate_hz(x);float want=440.0f*std::pow(2.0f,(float(note)-69.0f)/12.0f);float rel=std::fabs(got-want)/want;check(rel<.025f,"pitch tracking outside 2.5 percent");std::printf("PASS pitch note %u: %.3f Hz expected %.3f\n",note,got,want);} 
 std::puts("PASS fundamental pitch tracking");

 OSC_INIT(0,0);float globalPeak=0;for(uint16_t voices=0;voices<=3;++voices){OSC_PARAM(k_user_osc_param_id1,voices);for(uint16_t harm=0;harm<=7;++harm){OSC_PARAM(k_user_osc_param_id4,harm);for(uint16_t shape: {uint16_t(0),uint16_t(512),uint16_t(1023)}){OSC_PARAM(k_user_osc_param_shape,shape);OSC_PARAM(k_user_osc_param_shiftshape,1023);OSC_PARAM(k_user_osc_param_id2,100);OSC_PARAM(k_user_osc_param_id3,100);OSC_PARAM(k_user_osc_param_id5,100);OSC_PARAM(k_user_osc_param_id6,100);auto x=render(72,4096);for(float v:x)globalPeak=std::max(globalPeak,std::fabs(v));}}}
 check(globalPeak>.01f,"parameter grid produced no audio");std::printf("PASS voice/harmony/shape extremes; peak %.6f\n",globalPeak);

 OSC_INIT(0,0);uint64_t frames=0;for(uint32_t k=0;k<300;++k){OSC_PARAM(k_user_osc_param_id1,k%4);OSC_PARAM(k_user_osc_param_id2,k%101);OSC_PARAM(k_user_osc_param_id3,(k*7)%101);OSC_PARAM(k_user_osc_param_id4,k%8);OSC_PARAM(k_user_osc_param_id5,(k*11)%101);OSC_PARAM(k_user_osc_param_id6,(k*13)%101);OSC_PARAM(k_user_osc_param_shape,(k*37)%1024);OSC_PARAM(k_user_osc_param_shiftshape,(k*53)%1024);auto x=render(static_cast<uint8_t>(24+(k%96)),2048);for(float v:x)check(std::isfinite(v)&&std::fabs(v)<=1.001f,"soak output invalid");frames+=x.size();}
 std::printf("PASS note/parameter soak %llu frames\n",(unsigned long long)frames);std::printf("ALL COMMON OSC HOST TESTS PASSED: %s (hardware integration/tone not certified)\n",name);
}
