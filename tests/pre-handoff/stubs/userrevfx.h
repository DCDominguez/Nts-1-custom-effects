#pragma once
#include <stdint.h>
#include <math.h>
#define __sdram
inline float q31_to_f32(int32_t x) { return (float)x / 2147483648.0f; }
inline float fx_sinf(float x) { return sinf(6.2831853071795864769f * x); }
inline float fx_cosf(float x) { return cosf(6.2831853071795864769f * x); }
inline float fx_sqrtf(float x) { return sqrtf(x); }
inline float fx_tanhf(float x) { return tanhf(x); }
inline float fx_absf(float x) { return fabsf(x); }
static float test_bpm = 120.0f;
inline float fx_get_bpmf() { return test_bpm; }
inline uint32_t fx_get_bpm() { return (uint32_t)(test_bpm * 10.0f + 0.5f); }
enum { k_user_revfx_param_time = 0, k_user_revfx_param_depth = 1, k_user_revfx_param_reserved0 = 2, k_user_revfx_param_shift_depth = 3 };
