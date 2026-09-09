#include "usermodfx.h"

#include <stdint.h>

namespace {

static float s_rate_target = 0.0f;
static float s_damage_target = 0.0f;
static float s_rate = 0.0f;
static float s_damage = 0.0f;

static float s_hold_l = 0.0f;
static float s_hold_r = 0.0f;
static uint32_t s_count_l = 0u;
static uint32_t s_count_r = 0u;

static const float kSlew = 0.0020f;

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

static inline uint32_t round_u32(float x) {
  if (x <= 0.0f) return 0u;
  return static_cast<uint32_t>(x + 0.5f);
}

static inline uint32_t left_hold_samples(float rate) {
  // Quadratic mapping leaves more useful resolution near the clean end.
  uint32_t n = 1u + round_u32(rate * rate * 127.0f);
  if (n < 1u) n = 1u;
  if (n > 128u) n = 128u;
  return n;
}

static inline uint32_t right_hold_samples(uint32_t left_n, float rate, float damage) {
  // Human Soon extension: at stronger settings the R reduction clock is
  // deliberately offset by up to seven samples. This is deterministic and
  // bounded, producing stereo alias decorrelation without adding noise.
  float fracture = (damage * 2.0f) - 1.0f;
  if (fracture < 0.0f) fracture = 0.0f;
  fracture *= rate;

  uint32_t extra = round_u32(fracture * 7.0f);
  if (extra > 7u) extra = 7u;
  return left_n + extra;
}

static inline uint32_t quantizer_bits(float damage) {
  int32_t bits = static_cast<int32_t>(16.0f - damage * 12.0f + 0.5f);
  if (bits < 4) bits = 4;
  if (bits > 16) bits = 16;
  return static_cast<uint32_t>(bits);
}

static inline float quantize(float x, uint32_t bits) {
  x = clamp_audio(x);

  const uint32_t steps_i = 1u << (bits - 1u);
  const float steps = static_cast<float>(steps_i);
  const float scaled = x * steps;

  int32_t q;
  if (scaled >= 0.0f) {
    q = static_cast<int32_t>(scaled + 0.5f);
  } else {
    q = static_cast<int32_t>(scaled - 0.5f);
  }

  return clamp_audio(static_cast<float>(q) / steps);
}

static inline float lerp(float a, float b, float t) {
  return a + (b - a) * t;
}

static inline void reset_holds(void) {
  s_hold_l = 0.0f;
  s_hold_r = 0.0f;
  s_count_l = 0u;
  s_count_r = 0u;
}

} // namespace

void MODFX_INIT(uint32_t platform, uint32_t api) {
  (void)platform;
  (void)api;

  s_rate_target = 0.0f;
  s_damage_target = 0.0f;
  s_rate = 0.0f;
  s_damage = 0.0f;
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

    const uint32_t hold_l = left_hold_samples(s_rate);
    const uint32_t hold_r = right_hold_samples(hold_l, s_rate, s_damage);
    const uint32_t bits = quantizer_bits(s_damage);

    const float in_l = main_xn[i * 2u];
    const float in_r = main_xn[i * 2u + 1u];

    if (s_count_l == 0u || s_count_l >= hold_l) {
      s_hold_l = in_l;
      s_count_l = 1u;
    } else {
      ++s_count_l;
    }

    if (s_count_r == 0u || s_count_r >= hold_r) {
      s_hold_r = in_r;
      s_count_r = 1u;
    } else {
      ++s_count_r;
    }

    const float crushed_l = quantize(s_hold_l, bits);
    const float crushed_r = quantize(s_hold_r, bits);

    main_yn[i * 2u] = clamp_audio(lerp(in_l, crushed_l, s_damage));
    main_yn[i * 2u + 1u] = clamp_audio(lerp(in_r, crushed_r, s_damage));
  }
}

void MODFX_SUSPEND(void) {
  reset_holds();
}

void MODFX_RESUME(void) {
  reset_holds();
}

void MODFX_PARAM(uint8_t index, int32_t value) {
  const float normalized = clamp01(q31_to_f32(value));

  switch (index) {
    case k_user_modfx_param_time:
      s_rate_target = normalized;
      break;

    case k_user_modfx_param_depth:
      s_damage_target = normalized;
      break;

    default:
      break;
  }
}
