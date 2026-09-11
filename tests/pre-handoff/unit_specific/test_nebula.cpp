#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "tests/pre-handoff/common/measure.hpp"
#include "effects/nebula/nts1/src/nebula.cpp"

static void req(bool ok,const char*m){if(!ok){std::fprintf(stderr,"NEBULA FAIL: %s\n",m);std::exit(2);}}
static int32_t q(float x){x=std::max(0.f,std::min(.999999f,x));return(int32_t)(x*2147483647.f);}
static void set(float history,float density,float mix){REVFX_PARAM(k_user_revfx_param_time,q(history));REVFX_PARAM(k_user_revfx_param_depth,q(density));REVFX_PARAM(k_user_revfx_param_shift_depth,q(mix));}
static void settle(float a,float b,float m,int blocks=260){set(a,b,m);std::vector<float>z(128);for(int i=0;i<blocks;i++)REVFX_PROCESS(z.data(),64);}
static void proc(std::vector<float>&x){std::size_t p=0,n=x.size()/2;while(p<n){uint32_t k=(uint32_t)std::min<std::size_t>(64,n-p);REVFX_PROCESS(x.data()+2*p,k);p+=k;}}
static std::vector<float> tone(std::size_t n,double hz=0,double amp=.35){std::vector<float>x(n*2);for(std::size_t i=0;i<n;i++){float v=hz?(float)(amp*std::sin(2.0*hs_measure::kPi*hz*i/48000.0)):0.f;x[2*i]=v;x[2*i+1]=.9f*v;}return x;}

int main(){
 // MIX=0 must preserve the source exactly even while the granular history runs internally.
 REVFX_INIT(0,0);settle(.5f,.5f,0.f);auto dry=tone(50000,619),ref=dry;proc(dry);req(hs_measure::max_abs_diff(dry,ref)<2e-6,"MIX=0 is not dry");

 // DEPTH must turn a sparse one-grain field into a denser, wider four-grain cloud and reach audible output.
 REVFX_INIT(0,0);settle(.55f,.01f,1.f);auto sparse=tone(180000,347);proc(sparse);auto sl=hs_measure::channel(sparse,0,2,30000,120000),sr=hs_measure::channel(sparse,1,2,30000,120000);double sparseStereo=hs_measure::max_abs_diff(sl,sr);
 REVFX_INIT(0,0);settle(.55f,1.f,1.f);auto dense=tone(180000,347);proc(dense);auto dl=hs_measure::channel(dense,0,2,30000,120000),dr=hs_measure::channel(dense,1,2,30000,120000);double denseStereo=hs_measure::max_abs_diff(dl,dr);req(denseStereo>sparseStereo+1e-3,"DEPTH does not expand granular stereo scatter");req(hs_measure::max_abs_diff(sparse,dense)>.01,"DEPTH/density change does not reach output");

 // TIME expands the deterministic history aperture substantially; rendered output must change with it.
 REVFX_INIT(0,0);settle(.05f,.7f,1.f);float lowT=t;auto shortHist=tone(180000,251);proc(shortHist);
 REVFX_INIT(0,0);settle(.98f,.7f,1.f);float highT=t;auto longHist=tone(180000,251);proc(longHist);req((2500.f+27000.f*highT*highT)>(2500.f+27000.f*lowT*lowT)*4.f,"TIME does not materially expand history aperture");req(hs_measure::max_abs_diff(shortHist,longHist)>.01,"history aperture change does not reach output");

 // Seeded grain placement and all control smoothing must reproduce after a fresh init.
 REVFX_INIT(0,0);settle(.8f,.9f,1.f);auto a=tone(160000,293);proc(a);REVFX_INIT(0,0);settle(.8f,.9f,1.f);auto b=tone(160000,293);proc(b);req(hs_measure::max_abs_diff(a,b)<1e-6,"seeded granular trajectory not reproducible after init");

 // High-density/high-history tail must remain bounded and return toward rest after source stops.
 REVFX_INIT(0,0);settle(.95f,1.f,1.f);auto excite=tone(48000,220,.5);proc(excite);std::vector<float>tail(9*48000*2);proc(tail);auto tl=hs_measure::channel(tail,0);req(hs_measure::stats(tl).peak<=1.001,"tail exceeded bounds");double endRms=hs_measure::stats(tl,8*48000,48000).rms;req(endRms<.003,"granular feedback tail failed to decay toward rest");

 REVFX_SUSPEND();REVFX_RESUME();auto z=tone(40000);proc(z);req(hs_measure::stats(hs_measure::channel(z,0)).peak<1e-7,"reset emitted stale granular history");

 // Long stochastic/control soak crosses the 32768-sample history many times.
 REVFX_INIT(0,0);std::vector<float>blk(128);for(int bb=0;bb<10000;bb++){set(((bb*19)%101)/100.f,((bb*47)%101)/100.f,((bb*73)%101)/100.f);for(int i=0;i<64;i++){float v=.82f*std::sin(2.0*hs_measure::kPi*(101+(bb%17)*109.0)*(bb*64+i)/48000.0);blk[2*i]=v;blk[2*i+1]=-.76f*v;}REVFX_PROCESS(blk.data(),64);for(float v:blk){req(std::isfinite(v),"non-finite soak output");req(std::fabs(v)<=1.001,"soak output exceeded bounds");}}
 std::puts("NEBULA A-class project-specific harness PASS");return 0;
}
