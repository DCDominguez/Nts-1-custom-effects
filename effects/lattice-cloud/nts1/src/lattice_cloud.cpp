#include "userrevfx.h"

#include <stdint.h>

namespace {

static const float kSampleRate = 48000.0f;
static const uint32_t kPreSize = 8192u;
static const uint32_t kPreMask = kPreSize - 1u;
static const uint32_t kFdnSize = 16384u;
static const uint32_t kFdnMask = kFdnSize - 1u;
static const float kParamSlew = 0.0012f;

// LATTICE SPACE: intentionally simpler than the previous CLOUD. CORE composes
// and ECHO multiplies; this stage supplies a dark moving room around them.
__sdram float s_pre_l[kPreSize];
__sdram float s_pre_r[kPreSize];
__sdram float s_fdn0[kFdnSize];
__sdram float s_fdn1[kFdnSize];
__sdram float s_fdn2[kFdnSize];
__sdram float s_fdn3[kFdnSize];

static uint32_t s_pre_write = 0u;
static uint32_t s_fdn_write = 0u;
static float s_lp0 = 0.0f;
static float s_lp1 = 0.0f;
static float s_lp2 = 0.0f;
static float s_lp3 = 0.0f;
static float s_phase = 0.0f;
static float s_wander_phase = 0.37f;

static float s_space_target = 0.56f;
static float s_drift_target = 0.28f;
static float s_mix_target = 0.34f;
static float s_space = 0.56f;
static float s_drift = 0.28f;
static float s_mix = 0.34f;

static inline float absf(float x) { return x < 0.0f ? -x : x; }

static inline float clamp01(float x) {
  if (x < 0.0f) return 0.0f;
  if (x > 1.0f) return 1.0f;
  return x;
}

static inline float soft_limit(float x) {
  const float ax = absf(x);
  if (ax <= 0.90f) return x;
  float y = 0.90f + (ax - 0.90f) / (1.0f + 4.0f * (ax - 0.90f));
  if (y > 0.995f) y = 0.995f;
  return x < 0.0f ? -y : y;
}

static inline float wrap01(float x) {
  if (x >= 1.0f) x -= 1.0f;
  if (x < 0.0f) x += 1.0f;
  return x;
}

static inline float read_pre(const float *buffer, float delay) {
  float pos = static_cast<float>(s_pre_write) - delay;
  while (pos < 0.0f) pos += static_cast<float>(kPreSize);
  const uint32_t whole = static_cast<uint32_t>(pos);
  const uint32_t i0 = whole & kPreMask;
  const uint32_t i1 = (i0 + 1u) & kPreMask;
  const float frac = pos - static_cast<float>(whole);
  return buffer[i0] + (buffer[i1] - buffer[i0]) * frac;
}

static inline float read_fdn(const float *buffer, float delay) {
  float pos = static_cast<float>(s_fdn_write) - delay;
  while (pos < 0.0f) pos += static_cast<float>(kFdnSize);
  const uint32_t whole = static_cast<uint32_t>(pos);
  const uint32_t i0 = whole & kFdnMask;
  const uint32_t i1 = (i0 + 1u) & kFdnMask;
  const float frac = pos - static_cast<float>(whole);
  return buffer[i0] + (buffer[i1] - buffer[i0]) * frac;
}

static void reset_state(void) {
  for (uint32_t i = 0u; i < kPreSize; ++i) {
    s_pre_l[i] = 0.0f;
    s_pre_r[i] = 0.0f;
  }
  for (uint32_t i = 0u; i < kFdnSize; ++i) {
    s_fdn0[i] = 0.0f;
    s_fdn1[i] = 0.0f;
    s_fdn2[i] = 0.0f;
    s_fdn3[i] = 0.0f;
  }

  s_pre_write = 0u;
  s_fdn_write = 0u;
  s_lp0 = s_lp1 = s_lp2 = s_lp3 = 0.0f;
  s_phase = 0.0f;
  s_wander_phase = 0.37f;
  s_space_target = s_space = 0.56f;
  s_drift_target = s_drift = 0.28f;
  s_mix_target = s_mix = 0.34f;
}

} // namespace

void REVFX_INIT(uint32_t platform, uint32_t api) {
  (void)platform;
  (void)api;
  reset_state();
}

void REVFX_PROCESS(float *xn, uint32_t frames) {
  for (uint32_t f = 0u; f < frames; ++f) {
    s_space += (s_space_target - s_space) * kParamSlew;
    s_drift += (s_drift_target - s_drift) * kParamSlew;
    s_mix += (s_mix_target - s_mix) * kParamSlew;

    const float in_l = xn[f * 2u];
    const float in_r = xn[f * 2u + 1u];
    const float mid = 0.5f * (in_l + in_r);
    const float side = 0.5f * (in_l - in_r);

    s_pre_l[s_pre_write] = in_l;
    s_pre_r[s_pre_write] = in_r;

    // SPACE increases both pre-delay and room scale. Early reflections are
    // intentionally irregular so the reverb feels large without becoming a
    // second rhythmic generator.
    const float pre_delay = (0.012f + 0.055f * s_space) * kSampleRate;
    const float early_scale = 0.82f + 0.55f * s_space;

    const float pd_l = read_pre(s_pre_l, pre_delay);
    const float pd_r = read_pre(s_pre_r, pre_delay * 1.037f + 11.0f);

    const float e0_l = read_pre(s_pre_l, 431.0f * early_scale);
    const float e0_r = read_pre(s_pre_r, 587.0f * early_scale);
    const float e1_l = read_pre(s_pre_l, 823.0f * early_scale);
    const float e1_r = read_pre(s_pre_r, 1031.0f * early_scale);
    const float e2_l = read_pre(s_pre_l, 1429.0f * early_scale);
    const float e2_r = read_pre(s_pre_r, 1699.0f * early_scale);
    const float e3_l = read_pre(s_pre_l, 2213.0f * early_scale);
    const float e3_r = read_pre(s_pre_r, 2671.0f * early_scale);

    const float drift_rate = 0.025f + 0.105f * s_drift;
    s_phase = wrap01(s_phase + drift_rate / kSampleRate);
    s_wander_phase = wrap01(s_wander_phase + (0.017f + 0.061f * s_drift) / kSampleRate);

    // Only a few samples of modulation at low DRIFT; at maximum the room slowly
    // bends and breathes but never becomes an obvious chorus.
    const float mod_depth = 1.5f + 18.0f * s_drift;
    const float room = 0.90f + 0.26f * s_space;

    const float r0 = read_fdn(s_fdn0, 2237.0f * room + mod_depth * fx_sinf(s_phase));
    const float r1 = read_fdn(s_fdn1, 3163.0f * room + mod_depth * fx_sinf(s_phase + 0.21f));
    const float r2 = read_fdn(s_fdn2, 4211.0f * room + mod_depth * fx_sinf(s_phase + 0.49f));
    const float r3 = read_fdn(s_fdn3, 5987.0f * room + mod_depth * fx_sinf(s_phase + 0.76f));

    // More DRIFT also darkens the tail slightly, giving the movement a worn,
    // machine-like character without adding another front-panel filter control.
    const float damping = 0.30f - 0.11f * s_drift;
    s_lp0 += (r0 - s_lp0) * damping;
    s_lp1 += (r1 - s_lp1) * damping;
    s_lp2 += (r2 - s_lp2) * damping;
    s_lp3 += (r3 - s_lp3) * damping;

    // Hadamard-style feedback mix.
    const float h0 = 0.5f * (s_lp0 + s_lp1 + s_lp2 + s_lp3);
    const float h1 = 0.5f * (s_lp0 - s_lp1 + s_lp2 - s_lp3);
    const float h2 = 0.5f * (s_lp0 + s_lp1 - s_lp2 - s_lp3);
    const float h3 = 0.5f * (s_lp0 - s_lp1 - s_lp2 + s_lp3);

    const float early_l = e0_l * 0.34f + e1_l * 0.25f + e2_l * 0.18f + e3_l * 0.13f;
    const float early_r = e0_r * 0.34f + e1_r * 0.25f + e2_r * 0.18f + e3_r * 0.13f;
    const float early_mid = 0.5f * (early_l + early_r);

    const float feedback = 0.56f + 0.30f * s_space;
    const float inject = mid * 0.17f + 0.5f * (pd_l + pd_r) * 0.22f + early_mid * 0.25f;
    const float inject_side = side * (0.06f + 0.08f * s_drift);

    s_fdn0[s_fdn_write] = soft_limit(inject + h0 * feedback);
    s_fdn1[s_fdn_write] = soft_limit(inject_side + h1 * feedback);
    s_fdn2[s_fdn_write] = soft_limit(inject + h2 * feedback);
    s_fdn3[s_fdn_write] = soft_limit(-inject_side + h3 * feedback);

    const float wander = fx_sinf(s_wander_phase) * (0.04f + 0.12f * s_drift);
    const float tail_l = 0.46f * (r0 + r2) + (0.12f + wander) * (r1 - r3);
    const float tail_r = 0.46f * (r1 + r3) + (0.12f - wander) * (r2 - r0);

    const float wet_l = early_l * 0.30f + tail_l * 0.70f;
    const float wet_r = early_r * 0.30f + tail_r * 0.70f;

    // Keep the direct signal clearly present. MIX adds the space around the
    // chain instead of replacing or crushing the source.
    const float dry_gain = 0.96f - 0.06f * s_mix;
    const float wet_gain = (0.32f + 0.80f * s_mix) * s_mix;
    xn[f * 2u] = soft_limit(in_l * dry_gain + wet_l * wet_gain);
    xn[f * 2u + 1u] = soft_limit(in_r * dry_gain + wet_r * wet_gain);

    s_pre_write = (s_pre_write + 1u) & kPreMask;
    s_fdn_write = (s_fdn_write + 1u) & kFdnMask;
  }
}

void REVFX_SUSPEND(void) { reset_state(); }
void REVFX_RESUME(void) { reset_state(); }

void REVFX_PARAM(uint8_t index, int32_t value) {
  const float normalized = clamp01(q31_to_f32(value));
  if (index == k_user_revfx_param_time) s_space_target = normalized;
  else if (index == k_user_revfx_param_depth) s_drift_target = normalized;
  else if (index == k_user_revfx_param_shift_depth) s_mix_target = normalized;
}
