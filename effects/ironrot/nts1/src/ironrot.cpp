#include "usermodfx.h"
#include <stdint.h>

namespace {
static constexpr float kSlew = 0.0015f;
static float s_char_target=0.35f,s_cor_target=0.f,s_char=0.35f,s_cor=0.f;
static float s_lp_l=0.f,s_lp_r=0.f;
static inline float clamp01(float x){return x<0.f?0.f:(x>1.f?1.f:x);} 
static inline float clamp_audio(float x){return x<-1.f?-1.f:(x>1.f?1.f:x);} 
static inline float lerp(float a,float b,float t){return a+(b-a)*t;}
static inline float softsat(float x){
  const float x2=x*x;
  return x*(27.f+x2)/(27.f+9.f*x2);
}
static inline float process_one(float x,float &lp,float ch,float cor){
  lp += (x-lp)*(0.015f + 0.12f*ch);
  const float dark = lerp(lp,x,ch);
  const float edge = x-lp;
  const float pre = dark + edge*(0.15f + 1.15f*ch);
  const float drive = 1.f + 15.f*cor*cor;
  float y = softsat(pre*drive);
  const float cubic = y*y*y;
  y -= cubic*(0.28f*ch*cor);
  y = softsat(y*1.12f);
  return clamp_audio(lerp(x,y,cor));
}
static inline void reset(){s_lp_l=s_lp_r=0.f;}
}

void MODFX_INIT(uint32_t platform,uint32_t api){(void)platform;(void)api;reset();}
void MODFX_PROCESS(const float *main_xn,float *main_yn,const float *sub_xn,float *sub_yn,uint32_t frames){
 (void)sub_xn;(void)sub_yn;
 for(uint32_t i=0;i<frames;++i){
  s_char+=(s_char_target-s_char)*kSlew; s_cor+=(s_cor_target-s_cor)*kSlew;
  main_yn[2u*i]=process_one(main_xn[2u*i],s_lp_l,s_char,s_cor);
  main_yn[2u*i+1]=process_one(main_xn[2u*i+1],s_lp_r,s_char,s_cor);
 }
}
void MODFX_SUSPEND(void){reset();}
void MODFX_RESUME(void){reset();}
void MODFX_PARAM(uint8_t index,int32_t value){float v=clamp01(q31_to_f32(value));if(index==k_user_modfx_param_time)s_char_target=v;else if(index==k_user_modfx_param_depth)s_cor_target=v;}
