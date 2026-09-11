#pragma once
#include <stdint.h>
#include <math.h>
typedef int32_t q31_t;
struct user_osc_param_t { uint16_t pitch; int32_t shape_lfo; };
static constexpr float k_samplerate = 48000.0f;
static constexpr float k_samplerate_recipf = 1.0f / k_samplerate;
inline float q31_to_f32(int32_t x) { return (float)x / 2147483648.0f; }
inline q31_t f32_to_q31(float x) { if (x > 0.999999f) x = 0.999999f; if (x < -1.0f) x = -1.0f; return (q31_t)(x * 2147483647.0f); }
inline float param_val_to_f32(uint16_t v) { return (float)v / 1023.0f; }
inline float osc_sinf(float p) { return sinf(6.2831853071795864769f * p); }
inline float osc_softclipf(float, float x) { return tanhf(x); }
inline float osc_bl_saw_idx(float note) { return note; }
inline float osc_bl_sqr_idx(float note) { return note; }
inline float osc_bl2_sawf(float p, float) { return 2.0f * p - 1.0f; }
inline float osc_bl2_sqrf(float p, float) { return p < 0.5f ? 1.0f : -1.0f; }
inline float osc_w0f_for_note(uint8_t note, uint8_t fine) { float n = (float)note + (float)fine / 256.0f; float hz = 440.0f * powf(2.0f, (n - 69.0f) / 12.0f); return hz / k_samplerate; }
static uint32_t osc_rng_state = 0x12345678u;
inline float osc_white() { osc_rng_state = 1664525u * osc_rng_state + 1013904223u; uint32_t v = (osc_rng_state >> 8) & 0x00ffffffu; return ((float)v / 8388607.5f) - 1.0f; }
enum {
  k_user_osc_param_shape = 0,
  k_user_osc_param_shiftshape = 1,
  k_user_osc_param_id1 = 2,
  k_user_osc_param_id2 = 3,
  k_user_osc_param_id3 = 4,
  k_user_osc_param_id4 = 5,
  k_user_osc_param_id5 = 6,
  k_user_osc_param_id6 = 7
};
