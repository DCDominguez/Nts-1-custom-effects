#include "usermodfx.h"

#include <stdint.h>

namespace {

static const float kSampleRate = 48000.0f;
static const float kMinCarrierHz = 0.2f;
static const float kMaxCarrierHz = 3600.0f;
static const float kSlew = 0.0020f;
static const float kMaxStereoOffsetCycles = 1.0f / 12.0f;
static const float kOversampleThresholdHz = 800.0f;

static float s_time_target = 0.0f;
static float s_depth_target = 0.0f;
static float s_time = 0.0f;
static float s_depth = 0.0f;
static float s_phase = 0.0f;
static float s_prev_l = 0.0f;
static float s_prev_r = 0.0f;

static inline float clamp01(float x) {
  if (x < 0.0f) return 0.0f;
  if (x > 1.0f) return 1.0f;
  return x;
}

static inline float lerp(float a, float b, float t) { return a + (b - a) * t; }

static inline float wrap01(float x) {
  while (x < 0.0f) x += 1.0f;
  while (x >= 1.0f) x -= 1.0f;
  return x;
}

static inline float carrier_hz(float time) {
  const float t2 = time * time;
  const float t4 = t2 * t2;
  const float t5 = t4 * time;
  return kMinCarrierHz + t5 * (kMaxCarrierHz - kMinCarrierHz);
}

static inline float stereo_offset(float depth) {
  float ring_region = clamp01((depth - 0.5f) * 2.0f);
  return ring_region * kMaxStereoOffsetCycles;
}

static inline float modulation_factor(float carrier, float depth) {
  const float am = 0.5f + 0.5f * carrier;
  if (depth <= 0.5f) return lerp(1.0f, am, depth * 2.0f);
  return lerp(am, carrier, (depth - 0.5f) * 2.0f);
}

static inline float modulated(float input, float phase, float offset, float depth, bool right) {
  const float p = wrap01(phase + (right ? offset : -offset));
  return input * modulation_factor(fx_sinf(p), depth);
}

static inline void reset_phase(void) {
  s_phase = 0.0f;
  s_prev_l = 0.0f;
  s_prev_r = 0.0f;
}

} // namespace

void MODFX_INIT(uint32_t platform, uint32_t api) {
  (void)platform;
  (void)api;
  s_time_target = s_depth_target = 0.0f;
  s_time = s_depth = 0.0f;
  reset_phase();
}

void MODFX_PROCESS(const float *main_xn, float *main_yn,
                   const float *sub_xn, float *sub_yn,
                   uint32_t frames) {
  (void)sub_xn;
  (void)sub_yn;

  for (uint32_t i = 0u; i < frames; ++i) {
    s_time += (s_time_target - s_time) * kSlew;
    s_depth += (s_depth_target - s_depth) * kSlew;

    const float frequency = carrier_hz(s_time);
    const float phase_inc = frequency / kSampleRate;
    const float offset = stereo_offset(s_depth);
    const float in_l = main_xn[i * 2u];
    const float in_r = main_xn[i * 2u + 1u];

    float out_l;
    float out_r;

    if (frequency >= kOversampleThresholdHz && s_depth > 0.35f) {
      // Lightweight 2x path: interpolate one midpoint, process two carrier
      // substeps, then average. This is intentionally bounded for MkI CPU.
      const float mid_l = 0.5f * (s_prev_l + in_l);
      const float mid_r = 0.5f * (s_prev_r + in_r);
      const float p_mid = wrap01(s_phase + 0.5f * phase_inc);
      const float p_end = wrap01(s_phase + phase_inc);
      const float a_l = modulated(mid_l, p_mid, offset, s_depth, false);
      const float a_r = modulated(mid_r, p_mid, offset, s_depth, true);
      const float b_l = modulated(in_l, p_end, offset, s_depth, false);
      const float b_r = modulated(in_r, p_end, offset, s_depth, true);
      out_l = 0.5f * (a_l + b_l);
      out_r = 0.5f * (a_r + b_r);
    } else {
      out_l = modulated(in_l, s_phase, offset, s_depth, false);
      out_r = modulated(in_r, s_phase, offset, s_depth, true);
    }

    main_yn[i * 2u] = out_l;
    main_yn[i * 2u + 1u] = out_r;
    s_prev_l = in_l;
    s_prev_r = in_r;
    s_phase = wrap01(s_phase + phase_inc);
  }
}

void MODFX_SUSPEND(void) { reset_phase(); }
void MODFX_RESUME(void) { reset_phase(); }

void MODFX_PARAM(uint8_t index, int32_t value) {
  const float normalized = clamp01(q31_to_f32(value));
  if (index == k_user_modfx_param_time) s_time_target = normalized;
  else if (index == k_user_modfx_param_depth) s_depth_target = normalized;
}
