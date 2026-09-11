#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include "tests/pre-handoff/common/measure.hpp"
#include "effects/attractor/nts1/src/attractor.cpp"

static void require(bool ok,const char* msg){if(!ok){std::fprintf(stderr,"ATTRACTOR FAIL: %s\n",msg);std::exit(2);}}
static int32_t q31p(float x){x=std::max(0.0f,std::min(0.999999f,x));return static_cast<int32_t>(x*2147483647.0f);}
static void set_controls(float rate,float depth){MODFX_PARAM(k_user_modfx_param_time,q31p(rate));MODFX_PARAM(k_user_modfx_param_depth,q31p(depth));}
static void settle(float rate,float depth,int blocks=220){set_controls(rate,depth);std::vector<float> z(128),y(128),s(128),sy(128);for(int b=0;b<blocks;++b)MODFX_PROCESS(z.data(),y.data(),s.data(),sy.data(),64);}
static std::vector<float> process_constant(std::size_t frames,float l,float r){std::vector<float> in(frames*2),out(frames*2),s(frames*2),sy(frames*2);for(std::size_t i=0;i<frames;++i){in[2*i]=l;in[2*i+1]=r;}std::size_t p=0;while(p<frames){uint32_t n=static_cast<uint32_t>(std::min<std::size_t>(64,frames-p));MODFX_PROCESS(in.data()+2*p,out.data()+2*p,s.data()+2*p,sy.data()+2*p,n);p+=n;}return out;}
static std::vector<float> process_signal(const std::vector<float>& mono){std::vector<float> in(mono.size()*2),out(mono.size()*2),s(mono.size()*2),sy(mono.size()*2);for(std::size_t i=0;i<mono.size();++i)in[2*i]=in[2*i+1]=mono[i];std::size_t p=0;while(p<mono.size()){uint32_t n=static_cast<uint32_t>(std::min<std::size_t>(64,mono.size()-p));MODFX_PROCESS(in.data()+2*p,out.data()+2*p,s.data()+2*p,sy.data()+2*p,n);p+=n;}return out;}
static double motion_amount(const std::vector<float>& interleaved){auto l=hs_measure::channel(interleaved,0),r=hs_measure::channel(interleaved,1);double e=0.0;for(std::size_t i=1;i<l.size();++i){double d0=(l[i]-r[i])-(l[i-1]-r[i-1]);e+=std::fabs(d0);}return e/static_cast<double>(std::max<std::size_t>(1,l.size()-1));}

int main(){
  // DEPTH=0 is a dry spine.
  MODFX_INIT(0,0); settle(0.9f,0.0f); std::vector<float> src(24000); for(std::size_t i=0;i<src.size();++i)src[i]=0.45f*std::sin(2.0*hs_measure::kPi*733.0*i/48000.0); auto dry=hs_measure::channel(process_signal(src),0); require(hs_measure::max_abs_diff(src,dry)<2e-6,"DEPTH=0 is not dry");

  // DEPTH controls audible orbit width on mono material.
  MODFX_INIT(0,0); settle(0.8f,0.2f); auto mild=process_constant(160000,0.45f,0.45f); auto ml=hs_measure::channel(mild,0),mr=hs_measure::channel(mild,1); double mild_sep=0.0; for(std::size_t i=0;i<ml.size();++i){double d=ml[i]-mr[i];mild_sep+=d*d;} mild_sep=std::sqrt(mild_sep/ml.size());
  MODFX_INIT(0,0); settle(0.8f,1.0f); auto wide=process_constant(160000,0.45f,0.45f); auto wl=hs_measure::channel(wide,0),wr=hs_measure::channel(wide,1); double wide_sep=0.0; for(std::size_t i=0;i<wl.size();++i){double d=wl[i]-wr[i];wide_sep+=d*d;} wide_sep=std::sqrt(wide_sep/wl.size());
  require(wide_sep>mild_sep*2.5+1e-5,"DEPTH does not materially widen chaotic orbit");

  // RATE changes orbit update/movement rate while preserving the chaotic character.
  MODFX_INIT(0,0); settle(0.10f,1.0f); auto slow=process_constant(240000,0.4f,0.4f); double slow_motion=motion_amount(slow);
  MODFX_INIT(0,0); settle(0.95f,1.0f); auto fast=process_constant(240000,0.4f,0.4f); double fast_motion=motion_amount(fast);
  require(fast_motion>slow_motion*1.8+1e-7,"RATE does not measurably increase orbit motion");

  // Reset must reproduce the deterministic attractor trajectory.
  MODFX_INIT(0,0); settle(0.73f,0.88f); auto a=process_constant(64000,0.36f,0.36f);
  MODFX_INIT(0,0); settle(0.73f,0.88f); auto b=process_constant(64000,0.36f,0.36f);
  require(hs_measure::max_abs_diff(a,b)<1e-6,"reset does not reproduce deterministic chaotic trajectory");

  // The output must remain bounded and both channels remain alive at maximum orbit.
  auto ls=hs_measure::stats(wl),rs=hs_measure::stats(wr); require(ls.rms>0.05&&rs.rms>0.05,"maximum orbit dropped a channel"); require(ls.peak<=1.001&&rs.peak<=1.001,"maximum orbit exceeded bounds");

  // Lifecycle reset should return the attractor to its canonical initial state.
  MODFX_INIT(0,0); settle(0.65f,1.0f); (void)process_constant(32000,0.4f,0.4f); MODFX_SUSPEND(); MODFX_RESUME(); auto after=process_constant(8192,0.4f,0.4f); MODFX_SUSPEND(); MODFX_RESUME(); auto again=process_constant(8192,0.4f,0.4f); require(hs_measure::max_abs_diff(after,again)<1e-6,"suspend/resume does not reset attractor state deterministically");

  // Long control-abuse soak.
  MODFX_INIT(0,0); std::vector<float> in(128),out(128),sub(128),sy(128);
  for(int block=0;block<9000;++block){set_controls(((block*31)%101)/100.0f,((block*67)%101)/100.0f);for(int i=0;i<64;++i){float v=0.72f*std::sin(2.0*hs_measure::kPi*(89.0+(block%17)*97.0)*(block*64+i)/48000.0);in[2*i]=v;in[2*i+1]=0.6f*v;}MODFX_PROCESS(in.data(),out.data(),sub.data(),sy.data(),64);for(float v:out){require(std::isfinite(v),"non-finite output during soak");require(std::fabs(v)<=1.001f,"output bound exceeded during soak");}}

  std::puts("ATTRACTOR A-class project-specific harness PASS"); return 0;
}
