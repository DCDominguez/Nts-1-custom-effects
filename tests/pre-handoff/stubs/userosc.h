#pragma once
#include "common.h"
#include <cstdlib>

struct user_osc_param_t {
  uint16_t pitch;
  int32_t shape_lfo;
};

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

static uint32_t host_osc_rng = 0x12345678u;
static inline float osc_white(void) {
  host_osc_rng = host_osc_rng * 1664525u + 1013904223u;
  return (static_cast<int32_t>(host_osc_rng >> 1) / 1073741824.0f) - 1.0f;
}
static inline float osc_sinf(float x) { return std::sin(6.2831853071795864769f * x); }
static inline float osc_w0f_for_note(uint8_t note, uint8_t fine) {
  const float semis = static_cast<float>(note) + static_cast<float>(fine) / 256.0f;
  const float hz = 440.0f * std::pow(2.0f, (semis - 69.0f) / 12.0f);
  return hz / k_samplerate;
}
static inline float osc_bl_saw_idx(float note) { return note; }
static inline float osc_bl_sqr_idx(float note) { return note; }
static inline float osc_bl2_sawf(float phase, float) { return 2.0f * phase - 1.0f; }
static inline float osc_bl2_sqrf(float phase, float) { return phase < 0.5f ? 1.0f : -1.0f; }
static inline float osc_softclipf(float c, float x) {
  const float k = 1.0f + std::max(0.0f, c) * 8.0f;
  return std::tanh(k * x) / std::tanh(k);
}
