#pragma once
#include <stdint.h>
#include <cmath>
#include <algorithm>

#ifndef __sdram
#define __sdram
#endif
#ifndef __fast_inline
#define __fast_inline inline
#endif
#ifndef fast_inline
#define fast_inline inline
#endif

typedef int32_t q31_t;

static constexpr float k_samplerate = 48000.0f;
static constexpr float k_samplerate_recipf = 1.0f / k_samplerate;

static inline float q31_to_f32(int32_t v) {
  return (v == INT32_MIN) ? -1.0f : static_cast<float>(v) / 2147483647.0f;
}
static inline int32_t f32_to_q31(float x) {
  if (!std::isfinite(x)) return 0;
  x = std::max(-1.0f, std::min(1.0f, x));
  if (x <= -1.0f) return INT32_MIN;
  return static_cast<int32_t>(std::lrint(x * 2147483647.0f));
}
static inline float param_val_to_f32(uint16_t v) {
  return std::min<uint16_t>(v, 1023u) / 1023.0f;
}

extern float host_test_bpm;
static inline float fx_get_bpmf(void) { return host_test_bpm; }
static inline uint32_t fx_get_bpm(void) {
  if (!std::isfinite(host_test_bpm) || host_test_bpm <= 0.0f) return 1200u;
  return static_cast<uint32_t>(host_test_bpm * 10.0f + 0.5f);
}

static inline float fx_sinf(float x) { return std::sin(6.2831853071795864769f * x); }
static inline float fx_cosf(float x) { return std::cos(6.2831853071795864769f * x); }
static inline float fx_tanf(float x) { return std::tan(x); }
static inline float fx_tanpif(float x) { return std::tan(3.14159265358979323846f * x); }
static inline float fx_sqrtf(float x) { return std::sqrt(std::max(0.0f, x)); }
static inline float fx_powf(float a, float b) { return std::pow(a, b); }
static inline float fx_logf(float x) { return std::log(std::max(x, 1.0e-30f)); }
static inline float fx_expf(float x) { return std::exp(x); }
static inline float fx_fabsf(float x) { return std::fabs(x); }
static inline float fx_floorf(float x) { return std::floor(x); }
static inline float fx_ceilf(float x) { return std::ceil(x); }
static inline float fx_roundf(float x) { return std::round(x); }
static inline float fx_softclipf(float c, float x) {
  const float k = 1.0f + std::max(0.0f, c) * 8.0f;
  return std::tanh(k * x) / std::tanh(k);
}
static inline float fx_sat(float x) { return std::max(-1.0f, std::min(1.0f, x)); }
