#include "usermodfx.h"
#include <stdint.h>

namespace {

static constexpr float kSlew = 0.0015f;
static float s_char_target = 0.35f;
static float s_cor_target = 0.0f;
static float s_char = 0.35f;
static float s_cor = 0.0f;

struct ChannelState {
  float pre_lp;
  float post_lp;
  float in_env;
  float out_env;
};

static ChannelState s_l = {0.0f, 0.0f, 0.0f, 0.0f};
static ChannelState s_r = {0.0f, 0.0f, 0.0f, 0.0f};

static inline float clamp01(float x) { return x < 0.0f ? 0.0f : (x > 1.0f ? 1.0f : x); }
static inline float clamp_audio(float x) { return x < -1.0f ? -1.0f : (x > 1.0f ? 1.0f : x); }
static inline float clampf(float x, float lo, float hi) { return x < lo ? lo : (x > hi ? hi : x); }
static inline float absf(float x) { return x < 0.0f ? -x : x; }
static inline float lerp(float a, float b, float t) { return a + (b - a) * t; }

static inline float softsat(float x) {
  const float x2 = x * x;
  return x * (27.0f + x2) / (27.0f + 9.0f * x2);
}

static inline float asym_sat(float x, float asym) {
  const float bias = (asym - 0.5f) * 0.38f;
  return softsat(x + bias) - softsat(bias);
}

static inline float crossover_damage(float x, float amount) {
  const float a = absf(x);
  const float threshold = 0.006f + 0.105f * amount;
  if (a <= threshold) {
    return x * (1.0f - 0.82f * amount);
  }
  const float sign = x < 0.0f ? -1.0f : 1.0f;
  const float shifted = a - threshold * (0.30f + 0.45f * amount);
  return sign * (shifted > 0.0f ? shifted : 0.0f);
}

static inline float process_one(float x, ChannelState &st, float ch, float cor) {
  const float pre_coeff = 0.010f + 0.16f * ch;
  st.pre_lp += (x - st.pre_lp) * pre_coeff;
  const float low = st.pre_lp;
  const float high = x - low;
  const float shaped = low * (1.16f - 0.48f * ch) + high * (0.18f + 1.95f * ch);

  const float drive1 = 1.0f + 17.0f * cor * cor;
  float y = asym_sat(shaped * drive1, 0.24f + 0.66f * ch);

  const float fracture_amount = cor * (0.35f + 0.65f * ch);
  y = crossover_damage(y, fracture_amount);
  const float edge_drive = 1.0f + 7.0f * cor;
  y = softsat(y * edge_drive + high * (0.25f + 1.55f * ch) * cor);

  const float post_coeff = 0.018f + 0.24f * ch;
  st.post_lp += (y - st.post_lp) * post_coeff;
  const float cleaned = lerp(st.post_lp, y, 0.18f + 0.72f * ch);

  // Keep the corrosion texture while preventing distortion density from being
  // heard mainly as a level boost. Physical MkI testing showed IRONROT could
  // become much louder at high CORROSION, so the attenuation-only matcher is
  // allowed to work harder than before.
  const float env_coeff = 0.0010f;
  st.in_env += (absf(x) - st.in_env) * env_coeff;
  st.out_env += (absf(cleaned) - st.out_env) * env_coeff;
  float match = (st.in_env + 0.025f) / (st.out_env + 0.025f);
  match = clampf(match, 0.15f, 1.0f);

  const float severity_trim = 1.0f - 0.30f * cor;
  const float processed = cleaned * match * severity_trim;
  return clamp_audio(lerp(x, processed, cor));
}

static inline void reset() {
  s_l.pre_lp = s_l.post_lp = s_l.in_env = s_l.out_env = 0.0f;
  s_r.pre_lp = s_r.post_lp = s_r.in_env = s_r.out_env = 0.0f;
}

} // namespace

void MODFX_INIT(uint32_t platform, uint32_t api) {
  (void)platform;
  (void)api;
  reset();
}

void MODFX_PROCESS(const float *main_xn, float *main_yn,
                   const float *sub_xn, float *sub_yn,
                   uint32_t frames) {
  (void)sub_xn;
  (void)sub_yn;
  for (uint32_t i = 0u; i < frames; ++i) {
    s_char += (s_char_target - s_char) * kSlew;
    s_cor += (s_cor_target - s_cor) * kSlew;
    main_yn[2u * i] = process_one(main_xn[2u * i], s_l, s_char, s_cor);
    main_yn[2u * i + 1u] = process_one(main_xn[2u * i + 1u], s_r, s_char, s_cor);
  }
}

void MODFX_SUSPEND(void) { reset(); }
void MODFX_RESUME(void) { reset(); }

void MODFX_PARAM(uint8_t index, int32_t value) {
  const float v = clamp01(q31_to_f32(value));
  if (index == k_user_modfx_param_time) s_char_target = v;
  else if (index == k_user_modfx_param_depth) s_cor_target = v;
}
