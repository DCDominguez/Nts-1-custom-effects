#include "usermodfx.h"
#include <stdint.h>

namespace {

static constexpr float kSampleRate = 48000.0f;
static constexpr float kPi = 3.14159265358979323846f;
static constexpr float kSlew = 0.0015f;

struct SvfState {
  float ic1eq;
  float ic2eq;
};

static float s_cut_target = 0.45f;
static float s_vec_target = 0.0f;
static float s_cut = 0.45f;
static float s_vec = 0.0f;
static SvfState s_l = {0.0f, 0.0f};
static SvfState s_r = {0.0f, 0.0f};

static inline float clamp01(float x) { return x < 0.0f ? 0.0f : (x > 1.0f ? 1.0f : x); }
static inline float clamp_audio(float x) { return x < -1.0f ? -1.0f : (x > 1.0f ? 1.0f : x); }
static inline float clamp_state(float x) { return x < -6.0f ? -6.0f : (x > 6.0f ? 6.0f : x); }
static inline float lerp(float a, float b, float t) { return a + (b - a) * t; }
static inline float absf(float x) { return x < 0.0f ? -x : x; }

static inline float fast_tan_small(float x) {
  // tan(x) approximation is accurate enough over the intentionally limited
  // cutoff range and avoids an expensive transcendental in the sample loop.
  const float x2 = x * x;
  return x * (1.0f + x2 * (0.3333333f + 0.1333333f * x2));
}

static inline float vector_mix(float lp, float bp, float hp, float v) {
  float y;
  if (v < 0.5f) {
    const float t = v * 2.0f;
    y = lerp(lp, bp * 1.18f, t);
  } else {
    const float t = (v - 0.5f) * 2.0f;
    y = lerp(bp * 1.18f, hp, t);
  }
  return y;
}

static inline float process_one(float x, SvfState &st, float g, float k, float v) {
  const float a1 = 1.0f / (1.0f + g * (g + k));
  const float a2 = g * a1;
  const float a3 = g * a2;

  const float v3 = x - st.ic2eq;
  const float v1 = a1 * st.ic1eq + a2 * v3;
  const float v2 = st.ic2eq + a2 * st.ic1eq + a3 * v3;

  st.ic1eq = clamp_state(2.0f * v1 - st.ic1eq);
  st.ic2eq = clamp_state(2.0f * v2 - st.ic2eq);

  const float lp = v2;
  const float bp = v1;
  const float hp = x - k * v1 - v2;
  return clamp_audio(vector_mix(lp, bp, hp, v));
}

static inline void reset_state() {
  s_l.ic1eq = s_l.ic2eq = 0.0f;
  s_r.ic1eq = s_r.ic2eq = 0.0f;
}

} // namespace

void MODFX_INIT(uint32_t platform, uint32_t api) {
  (void)platform;
  (void)api;
  reset_state();
}

void MODFX_PROCESS(const float *main_xn, float *main_yn,
                   const float *sub_xn, float *sub_yn,
                   uint32_t frames) {
  (void)sub_xn;
  (void)sub_yn;

  for (uint32_t i = 0u; i < frames; ++i) {
    s_cut += (s_cut_target - s_cut) * kSlew;
    s_vec += (s_vec_target - s_vec) * kSlew;

    const float c = clamp01(s_cut);
    const float v = clamp01(s_vec);
    const float fc = 28.0f + 10972.0f * c * c;
    const float x = kPi * fc / kSampleRate;
    float g = fast_tan_small(x);
    if (g > 0.92f) g = 0.92f;

    const float center = 1.0f - absf(v * 2.0f - 1.0f);
    const float resonance = 0.12f + 0.72f * center;
    float k = 2.0f - 1.72f * resonance;
    if (k < 0.32f) k = 0.32f;

    main_yn[2u * i] = process_one(main_xn[2u * i], s_l, g, k, v);
    main_yn[2u * i + 1u] = process_one(main_xn[2u * i + 1u], s_r, g, k, v);
  }
}

void MODFX_SUSPEND(void) { reset_state(); }
void MODFX_RESUME(void) { reset_state(); }

void MODFX_PARAM(uint8_t index, int32_t value) {
  const float v = clamp01(q31_to_f32(value));
  if (index == k_user_modfx_param_time) s_cut_target = v;
  else if (index == k_user_modfx_param_depth) s_vec_target = v;
}
