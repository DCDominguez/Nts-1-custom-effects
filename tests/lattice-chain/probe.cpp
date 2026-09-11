// Diagnostic only: actual production source, desktop SDK shims, no ARM timing claim.
#include <cstdint>
#include <cmath>
#include <cstdio>
#include <algorithm>
#include "usermodfx.h"
namespace core {
#include "../../effects/lattice-core/nts1/src/lattice_core.cpp"
}
#define LATTICE_TEST
namespace field {
#include "../../effects/lattice-field/nts1/src/lattice_field.cpp"
}
int32_t param(float x){return static_cast<int32_t>(x*2147483000.f);}
int main(){
 std::puts("input_peak CORE_TIME core_peak output_peak core_guard_min field_guard_min field_guard_attacks clipped nonfinite core_events_in_full_freeze");
 for(float amp:{.2f,.4f,.85f})for(float time:{0.f,.5f,.9f,1.f}){
  core::MODFX_INIT(0,0);field::DELFX_INIT(0,0);
  core::MODFX_PARAM(0,param(time==1.f?.9f:time));core::MODFX_PARAM(1,param(.5f));
  field::DELFX_PARAM(0,param(.56f));field::DELFX_PARAM(1,param(.1f));field::DELFX_PARAM(3,param(.5f));
  float cp=0,op=0,cg=1,fg=1;unsigned clips=0,bad=0,freezeEvents=0;
  for(unsigned n=0;n<8*48000;++n){
   if(time==1.f&&n==2*48000)core::MODFX_PARAM(0,param(1.f));
   const float hz=n%72000<24000?261.6256f:n%72000<48000?293.6648f:329.6276f;
   float x[2]={amp*std::sin(6.28318530718f*hz*n/48000.f),0};x[1]=x[0];float y[2];
   unsigned before=core::s_event_counter;
   const bool frozen=core::s_freeze_active&&core::s_freeze_mix>.9999f;
   core::MODFX_PROCESS(x,y,nullptr,nullptr,1);
   if(frozen)freezeEvents+=core::s_event_counter-before;
   cp=std::max(cp,std::max(std::fabs(y[0]),std::fabs(y[1])));cg=std::min(cg,core::s_guard_gain);
   field::DELFX_PROCESS(y,1);fg=std::min(fg,field::wetGuard);
   for(float v:y){op=std::max(op,std::fabs(v));clips+=std::fabs(v)>=.99999f;bad+=!std::isfinite(v);}
  }
  std::printf("%.2f %.2f %.6f %.6f %.6f %.6f %u %u %u %u\n",amp,time,cp,op,cg,fg,field::guardHits,clips,bad,freezeEvents);
  if(clips||bad)return 1;
 }
}
