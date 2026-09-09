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
};

static ChannelState s_l = {0.0f, 0.0f};
static ChannelState s_r = {0.0f, 0.0f};

static inline float clamp01(float x) { return x < 0.0f ? 0.0f : (x > 1.0f ? 1.0f : x); }
static inline float clamp_audio(float x) { return x < -1.0f ? -1.0f : (x > 1.0f ? 1.0f : x); }
static inline float lerp(float a, float b, float t) { return a + (b - a) * t; }

static inline float softsat(float x) {
  const float x2 = x * x;
  return x * (27.0f + x2) / (27.0f + 9.0f * x2);
}

static inline float asym_sat(float x, float asym) {
  // Bias is removed after saturation so silence remains silence.
  const float bias = (asym - 0.5f) * 0.34f;
  const float y = softsat(x + bias) - softsat(bias);
  return y;
}

static inline float process_one(float x, ChannelState &st, float ch, float cor) {
  // Stage 1: CHARACTER-selectable pre-emphasis. Low CHARACTER retains body;
  // high CHARACTER increasingly exposes edge/high-frequency material.
  const float pre_coeff = 0.010f + 0.16f * ch;
  st.pre_lp += (x - st.pre_lp) * pre_coeff;
  const float low = st.pre_lp;
  const float high = x - low;
  const float shaped = low * (1.15f - 0.45f * ch) + high * (0.20f + 1.75f * ch);

  // Stage 2: asymmetric primary corrosion.
  const float drive1 = 1.0f + 11.0f * cor * cor;
  float y = asym_sat(shaped * drive1, 0.28f + 0.62f * ch);

  // Stage 3: secondary edge break-up. This is intentionally coordinated with
  // the first stage rather than simply stacking identical clippers.
  const float edge2 = y - st.post_lp;
  const float drive2 = 1.0f + 4.5f * cor;
  y = softsat(y * drive2 + edge2 * (0.35f + 1.1f * ch) * cor);

  // Stage 4: cleanup / bass retention. Bright CHARACTER preserves more upper
  // content; dark CHARACTER removes brittle high-order residue.
  const float post_coeff = 0.020f + 0.22f * ch;
  st.post_lp += (y - st.post_lp) * post_coeff;
  const float cleaned = lerp(st.post_lp, y, 0.22f + 0.68f * ch);

  // Compensate the increasing drive before dry/wet interpolation.
  const float compensation = 1.0f / (1.0f + 0.55f * cor);
  return clamp_audio(lerp(x, cleaned * compensation, cor));
}

static inline void reset() {
  s_l.pre_lp = s_l.post_lp = 0.0f;
  s_r.pre_lp = s_r.post_lp = 0.0f;
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
