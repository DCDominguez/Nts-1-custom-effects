#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <vector>
#include "tests/pre-handoff/common/measure.hpp"
#include "userrevfx.h"

namespace abyss_fx {
#include "effects/abyss/nts1/src/abyss.cpp"
}
namespace aureole_fx {
#include "effects/aureole/nts1/src/aureole.cpp"
}
namespace nebula_fx {
#include "effects/nebula/nts1/src/nebula.cpp"
}

using Init = void (*)(uint32_t, uint32_t);
using Process = void (*)(float *, uint32_t);
using Param = void (*)(uint8_t, int32_t);
struct Unit { const char *name; Init init; Process process; Param param; };

static int32_t q(float x){x=std::max(0.0f,std::min(0.999999f,x));return static_cast<int32_t>(x*2147483647.0f);} 
static std::vector<float> source(std::size_t frames){std::vector<float>x(frames*2u);uint32_t rng=0x85ebca6bu;for(std::size_t i=0;i<frames;++i){rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;float n=(static_cast<int32_t>(rng&0xffffu)-32768)/32768.0f;float a=.095f*std::sin(2.0*hs_measure::kPi*181.0*i/48000.0);float b=.067f*std::sin(2.0*hs_measure::kPi*463.0*i/48000.0);float c=.043f*std::sin(2.0*hs_measure::kPi*1061.0*i/48000.0);x[2u*i]=a+b+c+.018f*n;x[2u*i+1u]=.91f*a-.72f*b+.81f*c-.014f*n;}return x;}
static void process(Unit &u,std::vector<float>&x){std::size_t p=0,frames=x.size()/2u;while(p<frames){uint32_t n=static_cast<uint32_t>(std::min<std::size_t>(64u,frames-p));u.process(x.data()+2u*p,n);p+=n;}}
static double rms_stereo(const std::vector<float>&x,std::size_t skip){std::size_t f=x.size()/2u;auto l=hs_measure::channel(x,0,2,skip,f-skip);auto r=hs_measure::channel(x,1,2,skip,f-skip);return .5*(hs_measure::stats(l).rms+hs_measure::stats(r).rms);} 
static double peak_stereo(const std::vector<float>&x,std::size_t skip){std::size_t f=x.size()/2u;auto l=hs_measure::channel(x,0,2,skip,f-skip);auto r=hs_measure::channel(x,1,2,skip,f-skip);return std::max(hs_measure::stats(l).peak,hs_measure::stats(r).peak);} 
static double diff_rms(const std::vector<float>&a,const std::vector<float>&b,std::size_t skip){double e=0;std::size_t n=0;for(std::size_t i=skip*2u;i<a.size();++i){double d=static_cast<double>(a[i])-b[i];e+=d*d;++n;}return n?std::sqrt(e/n):0;}
static void probe(Unit u,float mix){constexpr std::size_t frames=8u*48000u,skip=2u*48000u;auto in=source(frames),out=in;u.init(0,0);u.param(k_user_revfx_param_time,q(.68f));u.param(k_user_revfx_param_depth,q(.55f));u.param(k_user_revfx_param_shift_depth,q(mix));process(u,out);double ir=rms_stereo(in,skip),orr=rms_stereo(out,skip);std::printf("%s mix=%.2f out/input=%.3f delta/input=%.3f peak=%.3f\n",u.name,mix,orr/ir,diff_rms(out,in,skip)/ir,peak_stereo(out,skip));}
int main(){Unit units[]={{"ABYSS",abyss_fx::REVFX_INIT,abyss_fx::REVFX_PROCESS,abyss_fx::REVFX_PARAM},{"AUREOLE",aureole_fx::REVFX_INIT,aureole_fx::REVFX_PROCESS,aureole_fx::REVFX_PARAM},{"NEBULA",nebula_fx::REVFX_INIT,nebula_fx::REVFX_PROCESS,nebula_fx::REVFX_PARAM}};for(auto &u:units){probe(u,.50f);probe(u,1.00f);}return 0;}
