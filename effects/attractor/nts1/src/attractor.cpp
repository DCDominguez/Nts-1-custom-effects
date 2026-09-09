#include "usermodfx.h"
#include <stdint.h>

namespace {
static constexpr float kSampleRate=48000.f;
static constexpr float kSlew=0.0015f;
static float s_rate_target=.25f,s_depth_target=0.f,s_rate=.25f,s_depth=0.f;
static float s_x=.231f,s_y=.677f,s_pan=0.f,s_pan_target=0.f;
static uint32_t s_counter=0u;
static inline float clamp01(float x){return x<0.f?0.f:(x>1.f?1.f:x);} 
static inline float clamp11(float x){return x<-1.f?-1.f:(x>1.f?1.f:x);} 
static inline float clamp_audio(float x){return x<-1.f?-1.f:(x>1.f?1.f:x);} 
static inline void step_chaos(){
 float nx=3.87f*s_x*(1.f-s_x)+0.025f*(s_y-.5f);
 float ny=3.91f*s_y*(1.f-s_y)+0.021f*(s_x-.5f);
 s_x=nx<.001f?.001f:(nx>.999f?.999f:nx);
 s_y=ny<.001f?.001f:(ny>.999f?.999f:ny);
 s_pan_target=clamp11((s_x-s_y)*1.85f);
}
static inline void reset(){s_x=.231f;s_y=.677f;s_pan=s_pan_target=0.f;s_counter=0u;}
}

void MODFX_INIT(uint32_t platform,uint32_t api){(void)platform;(void)api;reset();}
void MODFX_PROCESS(const float *main_xn,float *main_yn,const float *sub_xn,float *sub_yn,uint32_t frames){
 (void)sub_xn;(void)sub_yn;
 for(uint32_t i=0;i<frames;++i){
  s_rate+=(s_rate_target-s_rate)*kSlew; s_depth+=(s_depth_target-s_depth)*kSlew;
  const float updates=1.5f+70.f*s_rate*s_rate;
  uint32_t period=(uint32_t)(kSampleRate/updates); if(period<32u)period=32u;
  if(++s_counter>=period){s_counter=0u;step_chaos();}
  s_pan+=(s_pan_target-s_pan)*0.0025f;
  const float l=main_xn[2u*i],r=main_xn[2u*i+1];
  const float mid=.5f*(l+r),side=.5f*(l-r);
  const float p=s_pan*s_depth;
  const float moved_l=0.72f*mid*(1.f-p)+side*(1.f-.35f*s_depth);
  const float moved_r=0.72f*mid*(1.f+p)-side*(1.f-.35f*s_depth);
  main_yn[2u*i]=clamp_audio(l+(moved_l-l)*s_depth);
  main_yn[2u*i+1]=clamp_audio(r+(moved_r-r)*s_depth);
 }
}
void MODFX_SUSPEND(void){reset();}
void MODFX_RESUME(void){reset();}
void MODFX_PARAM(uint8_t index,int32_t value){float v=clamp01(q31_to_f32(value));if(index==k_user_modfx_param_time)s_rate_target=v;else if(index==k_user_modfx_param_depth)s_depth_target=v;}
