#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "tests/pre-handoff/common/measure.hpp"
#include "effects/lattice-cloud/nts1/src/lattice_cloud.cpp"

static void req(bool ok,const char*m){if(!ok){std::fprintf(stderr,"LATTICE SPACE FAIL: %s\n",m);std::exit(2);}}
static int32_t q(float x){x=std::max(0.f,std::min(.999999f,x));return(int32_t)(x*2147483647.f);}
static void set(float space,float drift,float mix){REVFX_PARAM(k_user_revfx_param_time,q(space));REVFX_PARAM(k_user_revfx_param_depth,q(drift));REVFX_PARAM(k_user_revfx_param_shift_depth,q(mix));}
static void settle(float a,float b,float m,int blocks=260){set(a,b,m);std::vector<float>z(128);for(int i=0;i<blocks;i++)REVFX_PROCESS(z.data(),64);}
static void proc(std::vector<float>&x){std::size_t p=0,n=x.size()/2;while(p<n){uint32_t k=(uint32_t)std::min<std::size_t>(64,n-p);REVFX_PROCESS(x.data()+2*p,k);p+=k;}}
static std::vector<float> tone(std::size_t n,double hz=0,double amp=.35){std::vector<float>x(n*2);for(std::size_t i=0;i<n;i++){float v=hz?(float)(amp*std::sin(2.0*hs_measure::kPi*hz*i/48000.0)):0.f;x[2*i]=x[2*i+1]=v;}return x;}
static std::vector<float> impulse_tail(float space,float drift,std::size_t frames){REVFX_INIT(0,0);settle(space,drift,1.f);std::vector<float>x(frames*2);x[0]=x[1]=.7f;proc(x);return x;}

int main(){
 // MIX=0 preserves source clarity exactly while the room network runs internally.
 REVFX_INIT(0,0);settle(.6f,.4f,0.f);auto dry=tone(50000,701),ref=dry;proc(dry);req(hs_measure::max_abs_diff(dry,ref)<2e-6,"MIX=0 is not dry");

 // SPACE must increase room/feedback persistence rather than merely level.
 auto small=impulse_tail(.05f,.25f,6*48000);auto large=impulse_tail(.98f,.25f,6*48000);auto sl=hs_measure::channel(small,0),ll=hs_measure::channel(large,0);double smallLate=hs_measure::stats(sl,2*48000,2*48000).energy;double largeLate=hs_measure::stats(ll,2*48000,2*48000).energy;req(largeLate>smallLate*1.5+1e-8,"SPACE does not materially increase late-room persistence");

 // DRIFT must change the rendered spatial motion/decay geometry and preserve stereo width.
 REVFX_INIT(0,0);settle(.72f,.01f,1.f);auto still=tone(180000,347);proc(still);REVFX_INIT(0,0);settle(.72f,1.f,1.f);auto moving=tone(180000,347);proc(moving);req(hs_measure::max_abs_diff(still,moving)>.005,"DRIFT does not reach rendered output");auto ml=hs_measure::channel(moving,0,2,30000,120000),mr=hs_measure::channel(moving,1,2,30000,120000);req(hs_measure::max_abs_diff(ml,mr)>.005,"SPACE output lacks stereo field");

 // Maximum SPACE must stay bounded and ultimately return toward rest.
 auto tail=impulse_tail(1.f,1.f,14*48000);auto tl=hs_measure::channel(tail,0);req(hs_measure::stats(tl).peak<=1.001,"maximum SPACE exceeded output bounds");double end=hs_measure::stats(tl,13*48000,48000).rms;req(end<.0015,"maximum SPACE tail failed to decay toward rest");req(s_guard_gain>0.f&&s_guard_gain<=1.001f,"output guard entered invalid state");

 // Suspend/resume clears every predelay/diffusion/FDN stage.
 REVFX_SUSPEND();REVFX_RESUME();std::vector<float>z(16000*2);proc(z);req(hs_measure::stats(hs_measure::channel(z,0)).peak<1e-7,"reset emitted stale room history");

 // Long control/wrap soak covers all circular buffers, diffusion stages and guard recovery.
 REVFX_INIT(0,0);std::vector<float>b(128);for(int bb=0;bb<11000;bb++){set(((bb*17)%101)/100.f,((bb*43)%101)/100.f,((bb*79)%101)/100.f);for(int i=0;i<64;i++){uint32_t n=bb*64+i;float v=.78f*std::sin(2.0*hs_measure::kPi*(97+(bb%17)*107.0)*n/48000.0);b[2*i]=v;b[2*i+1]=-.72f*v;}REVFX_PROCESS(b.data(),64);for(float v:b){req(std::isfinite(v),"non-finite soak output");req(std::fabs(v)<=1.001,"soak output exceeded bounds");}}
 std::puts("LATTICE SPACE A-class project-specific harness PASS");return 0;
}
