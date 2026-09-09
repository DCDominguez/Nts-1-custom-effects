#include "usermodfx.h"
#include <stdint.h>

namespace {
static constexpr float kSampleRate = 48000.0f;
static constexpr float kSlew = 0.0015f;
static float s_cut_target = 0.45f, s_vec_target = 0.0f;
static float s_cut = 0.45f, s_vec = 0.0f;
static float s_low_l = 0.0f, s_band_l = 0.0f;
static float s_low_r = 0.0f, s_band_r = 0.0f;

static inline float clamp01(float x){ return x < 0.f ? 0.f : (x > 1.f ? 1.f : x); }
static inline float clamp4(float x){ return x < -4.f ? -4.f : (x > 4.f ? 4.f : x); }
static inline float clamp_audio(float x){ return x < -1.f ? -1.f : (x > 1.f ? 1.f : x); }
static inline float lerp(float a,float b,float t){ return a + (b-a)*t; }

static inline float vector_mix(float lp,float bp,float hp,float v){
  if(v < 0.5f) return lerp(lp,bp,v*2.f);
  return lerp(bp,hp,(v-0.5f)*2.f);
}

static inline float process_one(float x, float &low, float &band, float f, float damping, float v){
  low = clamp4(low + f * band);
  const float high = clamp4(x - low - damping * band);
  band = clamp4(band + f * high);
  return clamp_audio(vector_mix(low, band, high, v));
}

static inline void reset_state(){ s_low_l=s_band_l=s_low_r=s_band_r=0.f; }
}

void MODFX_INIT(uint32_t platform,uint32_t api){ (void)platform; (void)api; reset_state(); }

void MODFX_PROCESS(const float *main_xn,float *main_yn,const float *sub_xn,float *sub_yn,uint32_t frames){
  (void)sub_xn; (void)sub_yn;
  for(uint32_t i=0;i<frames;++i){
    s_cut += (s_cut_target-s_cut)*kSlew;
    s_vec += (s_vec_target-s_vec)*kSlew;
    const float c = clamp01(s_cut);
    const float v = clamp01(s_vec);
    const float fc = 35.f + 8965.f * c * c;
    const float f = 2.f * fx_sinf(fc / (2.f * kSampleRate));
    float center = 1.f - ((v*2.f-1.f) < 0.f ? -(v*2.f-1.f) : (v*2.f-1.f));
    const float damping = 1.18f - 0.43f * center;
    main_yn[2u*i]   = process_one(main_xn[2u*i],   s_low_l, s_band_l, f, damping, v);
    main_yn[2u*i+1] = process_one(main_xn[2u*i+1], s_low_r, s_band_r, f, damping, v);
  }
}

void MODFX_SUSPEND(void){ reset_state(); }
void MODFX_RESUME(void){ reset_state(); }
void MODFX_PARAM(uint8_t index,int32_t value){
  const float v = clamp01(q31_to_f32(value));
  if(index==k_user_modfx_param_time) s_cut_target=v;
  else if(index==k_user_modfx_param_depth) s_vec_target=v;
}
