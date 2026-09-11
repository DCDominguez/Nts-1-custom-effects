#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <vector>
#include "tests/pre-handoff/common/measure.hpp"
#include "usermodfx.h"

namespace asc_fx {
#include "effects/ascender/nts1/src/ascender.cpp"
}
namespace attr_fx {
#include "effects/attractor/nts1/src/attractor.cpp"
}
namespace cap_fx {
#include "effects/capstan/nts1/src/capstan.cpp"
}
namespace iron_fx {
#include "effects/ironrot/nts1/src/ironrot.cpp"
}

using Init=void(*)(uint32_t,uint32_t);using Process=void(*)(const float*,float*,const float*,float*,uint32_t);using Param=void(*)(uint8_t,int32_t);struct Unit{const char*name;Init init;Process process;Param param;};
static int32_t q(float x){x=std::max(0.0f,std::min(.999999f,x));return static_cast<int32_t>(x*2147483647.0f);} 
static std::vector<float> source(std::size_t frames){std::vector<float>x(frames*2u);for(std::size_t i=0;i<frames;++i){float a=.125f*std::sin(2.0*hs_measure::kPi*193.0*i/48000.0);float b=.075f*std::sin(2.0*hs_measure::kPi*613.0*i/48000.0);float c=.040f*std::sin(2.0*hs_measure::kPi*1471.0*i/48000.0);x[2u*i]=a+b+c;x[2u*i+1u]=.88f*a-.62f*b+.93f*c;}return x;}
static void process(Unit &u,const std::vector<float>&in,std::vector<float>&out){std::vector<float>sub(in.size()),subout(in.size());std::size_t p=0,frames=in.size()/2u;while(p<frames){uint32_t n=static_cast<uint32_t>(std::min<std::size_t>(64u,frames-p));u.process(in.data()+2u*p,out.data()+2u*p,sub.data()+2u*p,subout.data()+2u*p,n);p+=n;}}
static double rms_stereo(const std::vector<float>&x,std::size_t skip){std::size_t f=x.size()/2u;auto l=hs_measure::channel(x,0,2,skip,f-skip);auto r=hs_measure::channel(x,1,2,skip,f-skip);return .5*(hs_measure::stats(l).rms+hs_measure::stats(r).rms);} 
static double peak_stereo(const std::vector<float>&x,std::size_t skip){std::size_t f=x.size()/2u;auto l=hs_measure::channel(x,0,2,skip,f-skip);auto r=hs_measure::channel(x,1,2,skip,f-skip);return std::max(hs_measure::stats(l).peak,hs_measure::stats(r).peak);} 
static double diff_rms(const std::vector<float>&a,const std::vector<float>&b,std::size_t skip){double e=0;std::size_t n=0;for(std::size_t i=skip*2u;i<a.size();++i){double d=static_cast<double>(a[i])-b[i];e+=d*d;++n;}return n?std::sqrt(e/n):0;}
static void probe(Unit u,float depth){constexpr std::size_t frames=8u*48000u,skip=2u*48000u;auto in=source(frames);std::vector<float>out(in.size());u.init(0,0);u.param(k_user_modfx_param_time,q(.55f));u.param(k_user_modfx_param_depth,q(depth));process(u,in,out);double ir=rms_stereo(in,skip),orr=rms_stereo(out,skip);std::printf("%s depth=%.2f out/input=%.3f delta/input=%.3f peak=%.3f\n",u.name,depth,orr/ir,diff_rms(out,in,skip)/ir,peak_stereo(out,skip));}
int main(){Unit units[]={{"ASCENDER",asc_fx::MODFX_INIT,asc_fx::MODFX_PROCESS,asc_fx::MODFX_PARAM},{"ATTRACTOR",attr_fx::MODFX_INIT,attr_fx::MODFX_PROCESS,attr_fx::MODFX_PARAM},{"CAPSTAN",cap_fx::MODFX_INIT,cap_fx::MODFX_PROCESS,cap_fx::MODFX_PARAM},{"IRONROT",iron_fx::MODFX_INIT,iron_fx::MODFX_PROCESS,iron_fx::MODFX_PARAM}};for(auto &u:units){probe(u,.50f);probe(u,.90f);}return 0;}
