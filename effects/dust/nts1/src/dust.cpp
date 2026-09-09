#include "usermodfx.h"

#include <stdint.h>

namespace {

static const float kSlew = 0.0020f;
static float s_rate_target = 0.0f;
static float s_damage_target = 0.0f;
static float s_rate = 0.0f;
static float s_damage = 0.0f;

static float s_hold_l = 0.0f;
static float s_hold_r = 0.0f;
static float s_phase_l = 1.0f;
static float s_phase_r = 1.0f;

static inline float clamp01(float x) {
  if (x < 0.0f) return 0.0f;
  if (x > 1.0f) return 1.0f;
  return x;
}

static inline float clamp_audio(float x) {
  if (x < -1.0f) return -1.0f;
  if (x > 1.0f) return 1.0f;
  return x;
}

static inline float absf(float x) { return x < 0.0f ? -x : x; }
static inline float lerp(float a, float b, float t) { return a + (b - a) * t; }

static inline float capture_rate(float rate) {
  // 1.0 = capture every sample. At maximum RATE this falls to 1/128.
  return 1.0f / (1.0f + 127.0f * rate * rate);
}

static inline float right_capture_rate(float left_rate, float rate, float damage) {
  float fracture = clamp01((damage - 0.5f) * 2.0f) * rate;
  // Up to ~5.5% clock separation at the destructive extreme.
  return left_rate * (1.0f - 0.055f * fracture);
}

static inline float quantize_bits(float x, uint32_t bits) {
  x = clamp_audio(x);
  if (absf(x) < 1.0e-8f) return 0.0f;
  const uint32_t steps_i = 1u << (bits - 1u);
  const float steps = static_cast<float>(steps_i);
  const float scaled = x * steps;
  int32_t q = (scaled >= 0.0f) ? static_cast<int32_t>(scaled + 0.5f)
                               : static_cast<int32_t>(scaled - 0.5f);
  return clamp_audio(static_cast<float>(q) / steps);
}

static inline float quantize_smooth(float x, float damage) {
  // Crossfade adjacent quantizers instead of jumping abruptly at bit boundaries.
  float bits_f = 16.0f - 12.0f * damage;
  if (bits_f < 4.0f) bits_f = 4.0f;
  if (bits_f > 16.0f) bits_f = 16.0f;
  uint32_t hi = static_cast<uint32_t>(bits_f);
  if (hi < 4u) hi = 4u;
  if (hi > 16u) hi = 16u;
  uint32_t lo = (hi > 4u) ? hi - 1u : hi;
  const float frac = static_cast<float>(hi) - bits_f;
  const float q_hi = quantize_bits(x, hi);
  const float q_lo = quantize_bits(x, lo);
  return lerp(q_hi, q_lo, frac);
}

static inline void reset_holds(void) {
  s_hold_l = 0.0f;
  s_hold_r = 0.0f;
  s_phase_l = 1.0f;
  s_phase_r = 1.0f;
}

} // namespace

void MODFX_INIT(uint32_t platform, uint32_t api) {
  (void)platform;
  (void)api;
  s_rate_target = s_damage_target = 0.0f;
  s_rate = s_damage = 0.0f;
  reset_holds();
}

void MODFX_PROCESS(const float *main_xn, float *main_yn,
                   const float *sub_xn, float *sub_yn,
                   uint32_t frames) {
  (void)sub_xn;
  (void)sub_yn;

  for (uint32_t i = 0u; i < frames; ++i) {
    s_rate += (s_rate_target - s_rate) * kSlew;
    s_damage += (s_damage_target - s_damage) * kSlew;

    const float in_l = main_xn[i * 2u];
    const float in_r = main_xn[i * 2u + 1u];
    const float rate_l = capture_rate(s_rate);
    const float rate_r = right_capture_rate(rate_l, s_rate, s_damage);

    s_phase_l += rate_l;
    if (s_phase_l >= 1.0f) {
      s_phase_l -= 1.0f;
      s_hold_l = in_l;
    }

    s_phase_r += rate_r;
    if (s_phase_r >= 1.0f) {
      s_phase_r -= 1.0f;
      s_hold_r = in_r;
    }

    const float crushed_l = quantize_smooth(s_hold_l, s_damage);
    const float crushed_r = quantize_smooth(s_hold_r, s_damage);

    main_yn[i * 2u] = clamp_audio(lerp(in_l, crushed_l, s_damage));
    main_yn[i * 2u + 1u] = clamp_audio(lerp(in_r, crushed_r, s_damage));
  }
}

void MODFX_SUSPEND(void) { reset_holds(); }
void MODFX_RESUME(void) { reset_holds(); }

void MODFX_PARAM(uint8_t index, int32_t value) {
  const float normalized = clamp01(q31_to_f32(value));
  if (index == k_user_modfx_param_time) s_rate_target = normalized;
  else if (index == k_user_modfx_param_depth) s_damage_target = normalized;
}
