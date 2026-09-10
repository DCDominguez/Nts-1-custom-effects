#include "userrevfx.h"

#include <stdint.h>

namespace {

static const float kSampleRate = 48000.0f;
static const uint32_t kPreSize = 4096u;
static const uint32_t kPreMask = kPreSize - 1u;
static const uint32_t kFdnSize = 8192u;
static const uint32_t kFdnMask = kFdnSize - 1u;
static const uint32_t kDiff1Size = 1024u;
static const uint32_t kDiff1Mask = kDiff1Size - 1u;
static const uint32_t kDiff2Size = 2048u;
static const uint32_t kDiff2Mask = kDiff2Size - 1u;
static const float kParamSlew = 0.0012f;
static const float kGuardCeiling = 0.86f;

// SPACE 0.3 trades oversized delay memory for density: two short diffusion
// stages feed a compact four-line FDN. The goal is a thicker room, not a longer
// or more CPU-hungry generative effect.
__sdram float s_pre_l[kPreSize];
__sdram float s_pre_r[kPreSize];
__sdram float s_diff1_l[kDiff1Size];
__sdram float s_diff1_r[kDiff1Size];
__sdram float s_diff2_l[kDiff2Size];
__sdram float s_diff2_r[kDiff2Size];
__sdram float s_fdn0[kFdnSize];
__sdram float s_fdn1[kFdnSize];
__sdram float s_fdn2[kFdnSize];
__sdram float s_fdn3[kFdnSize];

static uint32_t s_pre_write = 0u;
static uint32_t s_diff1_write = 0u;
static uint32_t s_diff2_write = 0u;
static uint32_t s_fdn_write = 0u;
static float s_lp0 = 0.0f;
static float s_lp1 = 0.0f;
static float s_lp2 = 0.0f;
static float s_lp3 = 0.0f;
static float s_body_l = 0.0f;
static float s_body_r = 0.0f;
static float s_phase = 0.0f;
static float s_guard_gain = 1.0f;

static float s_space_target = 0.56f;
static float s_drift_target = 0.28f;
static float s_mix_target = 0.34f;
static float s_space = 0.56f;
static float s_drift = 0.28f;
static float s_mix = 0.34f;

static inline float absf(float x) { return x < 0.0f ? -x : x; }
static inline float maxf(float a, float b) { return a > b ? a : b; }

static inline float clamp01(float x) {
  if (x < 0.0f) return 0.0f;
  if (x > 1.0f) return 1.0f;
  return x;
}

static inline float soft_limit(float x) {
  const float ax = absf(x);
  if (ax <= 0.94f) return x;
  const float over = ax - 0.94f;
  float y = 0.94f + over / (1.0f + 6.0f * over);
  if (y > 0.995f) y = 0.995f;
  return x < 0.0f ? -y : y;
}

static inline void guard_pair(float &l, float &r) {
  const float peak = maxf(absf(l), absf(r));
  if (peak > kGuardCeiling) {
    const float needed = kGuardCeiling / peak;
    if (needed < s_guard_gain) s_guard_gain = needed;
  } else {
    s_guard_gain += (1.0f - s_guard_gain) * 0.00028f;
    if (s_guard_gain > 1.0f) s_guard_gain = 1.0f;
  }
  l *= s_guard_gain;
  r *= s_guard_gain;
}

static inline float wrap01(float x) {
  if (x >= 1.0f) x -= 1.0f;
  if (x < 0.0f) x += 1.0f;
  return x;
}

static inline float tri(float p) {
  p = wrap01(p);
  return p < 0.5f ? (p * 4.0f - 1.0f) : (3.0f - p * 4.0f);
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

static inline float allpass(float x, float *buffer, uint32_t mask,
                            uint32_t write, uint32_t delay, float g) {
  const uint32_t read = (write - delay) & mask;
  const float delayed = buffer[read];
  const float y = delayed - g * x;
  buffer[write] = x + g * y;
  return y;
}

static void reset_state(void) {
  for (uint32_t i = 0u; i < kPreSize; ++i) {
    s_pre_l[i] = 0.0f;
    s_pre_r[i] = 0.0f;
  }
  for (uint32_t i = 0u; i < kDiff1Size; ++i) {
    s_diff1_l[i] = 0.0f;
    s_diff1_r[i] = 0.0f;
  }
  for (uint32_t i = 0u; i < kDiff2Size; ++i) {
    s_diff2_l[i] = 0.0f;
    s_diff2_r[i] = 0.0f;
  }
  for (uint32_t i = 0u; i < kFdnSize; ++i) {
    s_fdn0[i] = 0.0f;
    s_fdn1[i] = 0.0f;
    s_fdn2[i] = 0.0f;
    s_fdn3[i] = 0.0f;
  }

  s_pre_write = 0u;
  s_diff1_write = 0u;
  s_diff2_write = 0u;
  s_fdn_write = 0u;
  s_lp0 = s_lp1 = s_lp2 = s_lp3 = 0.0f;
  s_body_l = s_body_r = 0.0f;
  s_phase = 0.0f;
  s_guard_gain = 1.0f;
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

    const float pre_delay = (0.008f + 0.045f * s_space) * kSampleRate;
    const float early_scale = 0.84f + 0.43f * s_space;

    const float pd_l = read_pre(s_pre_l, pre_delay);
    const float pd_r = read_pre(s_pre_r, pre_delay * 1.043f + 9.0f);

    const float e0_l = read_pre(s_pre_l, 317.0f * early_scale);
    const float e0_r = read_pre(s_pre_r, 463.0f * early_scale);
    const float e1_l = read_pre(s_pre_l, 701.0f * early_scale);
    const float e1_r = read_pre(s_pre_r, 887.0f * early_scale);
    const float e2_l = read_pre(s_pre_l, 1279.0f * early_scale);
    const float e2_r = read_pre(s_pre_r, 1531.0f * early_scale);
    const float e3_l = read_pre(s_pre_l, 2161.0f * early_scale);
    const float e3_r = read_pre(s_pre_r, 2557.0f * early_scale);

    const float early_l = e0_l * 0.36f + e1_l * 0.27f + e2_l * 0.20f + e3_l * 0.15f;
    const float early_r = e0_r * 0.36f + e1_r * 0.27f + e2_r * 0.20f + e3_r * 0.15f;

    // Two short all-pass stages turn discrete early reflections into a dense,
    // fat injection cloud before the FDN. This is the main 0.3 character change.
    const float diff_in_l = pd_l * 0.40f + early_l * 0.72f;
    const float diff_in_r = pd_r * 0.40f + early_r * 0.72f;
    const float ap1_l = allpass(diff_in_l, s_diff1_l, kDiff1Mask, s_diff1_write, 347u, 0.59f);
    const float ap1_r = allpass(diff_in_r, s_diff1_r, kDiff1Mask, s_diff1_write, 421u, 0.59f);
    const float ap2_l = allpass(ap1_l, s_diff2_l, kDiff2Mask, s_diff2_write, 683u, 0.53f);
    const float ap2_r = allpass(ap1_r, s_diff2_r, kDiff2Mask, s_diff2_write, 811u, 0.53f);

    const float drift_rate = 0.020f + 0.080f * s_drift;
    s_phase = wrap01(s_phase + drift_rate / kSampleRate);
    const float mod_depth = 1.0f + 13.0f * s_drift;
    const float room = 0.88f + 0.28f * s_space;

    const float r0 = read_fdn(s_fdn0, 1987.0f * room + mod_depth * tri(s_phase));
    const float r1 = read_fdn(s_fdn1, 2789.0f * room + mod_depth * tri(s_phase + 0.23f));
    const float r2 = read_fdn(s_fdn2, 3761.0f * room + mod_depth * tri(s_phase + 0.51f));
    const float r3 = read_fdn(s_fdn3, 5413.0f * room + mod_depth * tri(s_phase + 0.77f));

    // Keep the low-mid body while gently damping the top as DRIFT rises.
    const float damping = 0.33f - 0.10f * s_drift;
    s_lp0 += (r0 - s_lp0) * damping;
    s_lp1 += (r1 - s_lp1) * damping;
    s_lp2 += (r2 - s_lp2) * damping;
    s_lp3 += (r3 - s_lp3) * damping;

    const float h0 = 0.5f * (s_lp0 + s_lp1 + s_lp2 + s_lp3);
    const float h1 = 0.5f * (s_lp0 - s_lp1 + s_lp2 - s_lp3);
    const float h2 = 0.5f * (s_lp0 + s_lp1 - s_lp2 - s_lp3);
    const float h3 = 0.5f * (s_lp0 - s_lp1 - s_lp2 + s_lp3);

    const float diff_mid = 0.5f * (ap2_l + ap2_r);
    const float diff_side = 0.5f * (ap2_l - ap2_r);
    const float feedback = 0.54f + 0.27f * s_space;
    const float inject = mid * 0.08f + diff_mid * 0.38f;
    const float inject_side = side * (0.07f + 0.07f * s_drift) + diff_side * 0.20f;

    s_fdn0[s_fdn_write] = soft_limit(inject + h0 * feedback);
    s_fdn1[s_fdn_write] = soft_limit(inject_side + h1 * feedback);
    s_fdn2[s_fdn_write] = soft_limit(inject + h2 * feedback);
    s_fdn3[s_fdn_write] = soft_limit(-inject_side + h3 * feedback);

    const float wander = tri(s_phase + 0.11f) * (0.03f + 0.10f * s_drift);
    const float tail_l = 0.50f * (r0 + r2) + (0.10f + wander) * (r1 - r3);
    const float tail_r = 0.50f * (r1 + r3) + (0.10f - wander) * (r2 - r0);

    // A slow body follower adds weight below the bright early reflections.
    s_body_l += (tail_l - s_body_l) * 0.055f;
    s_body_r += (tail_r - s_body_r) * 0.055f;

    const float wet_l = early_l * 0.23f + ap2_l * 0.24f + tail_l * 0.70f + s_body_l * 0.16f;
    const float wet_r = early_r * 0.23f + ap2_r * 0.24f + tail_r * 0.70f + s_body_r * 0.16f;

    // SPACE is allowed to own the room at high MIX. The output guard creates
    // headroom for the next stage instead of keeping the reverb artificially thin.
    const float dry_gain = 1.0f - 0.55f * s_mix;
    const float wet_gain = (0.58f + 0.57f * s_mix) * s_mix;
    float out_l = in_l * dry_gain + wet_l * wet_gain;
    float out_r = in_r * dry_gain + wet_r * wet_gain;
    guard_pair(out_l, out_r);
    xn[f * 2u] = soft_limit(out_l);
    xn[f * 2u + 1u] = soft_limit(out_r);

    s_pre_write = (s_pre_write + 1u) & kPreMask;
    s_diff1_write = (s_diff1_write + 1u) & kDiff1Mask;
    s_diff2_write = (s_diff2_write + 1u) & kDiff2Mask;
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
