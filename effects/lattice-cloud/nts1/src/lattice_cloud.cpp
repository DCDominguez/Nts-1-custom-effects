#include "userrevfx.h"

#include <stdint.h>

namespace {

static const float kSampleRate = 48000.0f;
static const uint32_t kPreSize = 4096u;
static const uint32_t kPreMask = kPreSize - 1u;
static const uint32_t kDiffSize = 2048u;
static const uint32_t kDiffMask = kDiffSize - 1u;
static const uint32_t kFdnSize = 4096u;
static const uint32_t kFdnMask = kFdnSize - 1u;
static const float kParamSlew = 0.0012f;
static const float kGuardCeiling = 0.90f;

// SPACE 0.4 is deliberately cheaper and more direct than 0.3. The MkI
// physical pass found 0.3 too subtle at full MIX and distorted when preceded
// by modulation. One stereo diffusion stage now feeds a fixed four-line FDN;
// DRIFT moves output geometry/damping instead of moving four delay heads.
// This preserves a dark, living room while removing the most expensive
// per-sample interpolated reads and feedback-path soft-limit divisions.
__sdram float s_pre_l[kPreSize];
__sdram float s_pre_r[kPreSize];
__sdram float s_diff_l[kDiffSize];
__sdram float s_diff_r[kDiffSize];
__sdram float s_fdn0[kFdnSize];
__sdram float s_fdn1[kFdnSize];
__sdram float s_fdn2[kFdnSize];
__sdram float s_fdn3[kFdnSize];

static uint32_t s_pre_write = 0u;
static uint32_t s_diff_write = 0u;
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

static inline float clampf(float x, float lo, float hi) {
  if (x < lo) return lo;
  if (x > hi) return hi;
  return x;
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

static inline float read_fixed(const float *buffer, uint32_t write,
                               uint32_t mask, uint32_t delay) {
  return buffer[(write - delay) & mask];
}

static inline float allpass(float x, float *buffer, uint32_t mask,
                            uint32_t write, uint32_t delay, float g) {
  const uint32_t read = (write - delay) & mask;
  const float delayed = buffer[read];
  const float y = delayed - g * x;
  buffer[write] = x + g * y;
  return y;
}

static inline float feedback_bound(float x) {
  // Emergency-only bound. Normal operation should remain linear here; unlike
  // SPACE 0.3 this does not continuously waveshape every feedback write.
  return clampf(x, -0.985f, 0.985f);
}

static inline void guard_pair(float &l, float &r) {
  const float peak = maxf(absf(l), absf(r));
  const float target = peak > kGuardCeiling ? (kGuardCeiling / peak) : 1.0f;

  if (target < s_guard_gain) {
    // Catch overload immediately.
    s_guard_gain = target;
  } else {
    // Recover toward the currently safe gain even if the raw signal remains
    // above the ceiling. SPACE 0.3 could remain over-attenuated for too long
    // because it recovered only after raw peak fell below the ceiling.
    s_guard_gain += (target - s_guard_gain) * 0.0012f;
    if (s_guard_gain > 1.0f) s_guard_gain = 1.0f;
  }

  l *= s_guard_gain;
  r *= s_guard_gain;
}

static void reset_state(void) {
  for (uint32_t i = 0u; i < kPreSize; ++i) {
    s_pre_l[i] = 0.0f;
    s_pre_r[i] = 0.0f;
  }
  for (uint32_t i = 0u; i < kDiffSize; ++i) {
    s_diff_l[i] = 0.0f;
    s_diff_r[i] = 0.0f;
  }
  for (uint32_t i = 0u; i < kFdnSize; ++i) {
    s_fdn0[i] = 0.0f;
    s_fdn1[i] = 0.0f;
    s_fdn2[i] = 0.0f;
    s_fdn3[i] = 0.0f;
  }

  s_pre_write = 0u;
  s_diff_write = 0u;
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

    // Fixed predelay and only two early taps per channel. SPACE controls the
    // balance and persistence rather than continuously moving these read heads.
    const float pd_l = read_fixed(s_pre_l, s_pre_write, kPreMask, 613u);
    const float pd_r = read_fixed(s_pre_r, s_pre_write, kPreMask, 719u);
    const float e0_l = read_fixed(s_pre_l, s_pre_write, kPreMask, 311u);
    const float e0_r = read_fixed(s_pre_r, s_pre_write, kPreMask, 421u);
    const float e1_l = read_fixed(s_pre_l, s_pre_write, kPreMask, 997u);
    const float e1_r = read_fixed(s_pre_r, s_pre_write, kPreMask, 1187u);
    const float early_l = e0_l * 0.62f + e1_l * 0.38f;
    const float early_r = e0_r * 0.62f + e1_r * 0.38f;

    // One diffusion stage keeps the room dense without the second stereo
    // all-pass bank used by 0.3.
    const float diff_g = 0.48f + 0.07f * s_space;
    const float diff_in_l = pd_l * 0.68f + early_l * 0.52f;
    const float diff_in_r = pd_r * 0.68f + early_r * 0.52f;
    const float diff_l = allpass(diff_in_l, s_diff_l, kDiffMask,
                                 s_diff_write, 337u, diff_g);
    const float diff_r = allpass(diff_in_r, s_diff_r, kDiffMask,
                                 s_diff_write, 443u, diff_g);

    // Fixed integer FDN reads are substantially cheaper than 0.3's four
    // continuously interpolated modulated read heads.
    const float r0 = read_fixed(s_fdn0, s_fdn_write, kFdnMask, 1423u);
    const float r1 = read_fixed(s_fdn1, s_fdn_write, kFdnMask, 1987u);
    const float r2 = read_fixed(s_fdn2, s_fdn_write, kFdnMask, 2671u);
    const float r3 = read_fixed(s_fdn3, s_fdn_write, kFdnMask, 3433u);

    // TIME/SPACE now controls persistence and body. DRIFT controls motion in
    // damping/output geometry rather than delay-time interpolation.
    const float damping = 0.20f + 0.12f * (1.0f - s_drift);
    s_lp0 += (r0 - s_lp0) * damping;
    s_lp1 += (r1 - s_lp1) * damping;
    s_lp2 += (r2 - s_lp2) * damping;
    s_lp3 += (r3 - s_lp3) * damping;

    const float h0 = 0.5f * (s_lp0 + s_lp1 + s_lp2 + s_lp3);
    const float h1 = 0.5f * (s_lp0 - s_lp1 + s_lp2 - s_lp3);
    const float h2 = 0.5f * (s_lp0 + s_lp1 - s_lp2 - s_lp3);
    const float h3 = 0.5f * (s_lp0 - s_lp1 - s_lp2 + s_lp3);

    const float diff_mid = 0.5f * (diff_l + diff_r);
    const float diff_side = 0.5f * (diff_l - diff_r);
    const float feedback = 0.46f + 0.30f * s_space;
    const float inject = mid * 0.10f + diff_mid * (0.30f + 0.08f * s_space);
    const float inject_side = side * (0.08f + 0.08f * s_drift) +
                              diff_side * (0.16f + 0.08f * s_drift);

    s_fdn0[s_fdn_write] = feedback_bound(inject + h0 * feedback);
    s_fdn1[s_fdn_write] = feedback_bound(inject_side + h1 * feedback);
    s_fdn2[s_fdn_write] = feedback_bound(inject + h2 * feedback);
    s_fdn3[s_fdn_write] = feedback_bound(-inject_side + h3 * feedback);

    const float drift_rate = 0.022f + 0.090f * s_drift;
    s_phase = wrap01(s_phase + drift_rate / kSampleRate);
    const float wander = tri(s_phase + 0.11f) * (0.06f + 0.16f * s_drift);

    const float tail_l = 0.52f * (r0 + r2) + (0.12f + wander) * (r1 - r3);
    const float tail_r = 0.52f * (r1 + r3) + (0.12f - wander) * (r2 - r0);

    // Slow body follower keeps the room weighty without relying on hot
    // feedback saturation.
    s_body_l += (tail_l - s_body_l) * 0.050f;
    s_body_r += (tail_r - s_body_r) * 0.050f;

    const float early_voice = 0.28f + 0.18f * (1.0f - s_space);
    const float tail_voice = 0.72f + 0.20f * s_space;
    const float wet_l = early_l * early_voice + diff_l * 0.20f +
                        tail_l * tail_voice + s_body_l * 0.18f;
    const float wet_r = early_r * early_voice + diff_r * 0.20f +
                        tail_r * tail_voice + s_body_r * 0.18f;

    // Full MIX is intentionally wet-dominant now. 0.3 retained 45% dry at
    // maximum, which contributed to the physical report that SPACE was too
    // subtle even when MIX was fully raised.
    const float dry_gain = 1.0f - 0.88f * s_mix;
    const float wet_gain = (0.78f + 0.30f * s_mix) * s_mix;
    float out_l = in_l * dry_gain + wet_l * wet_gain;
    float out_r = in_r * dry_gain + wet_r * wet_gain;

    guard_pair(out_l, out_r);
    xn[f * 2u] = clampf(out_l, -0.995f, 0.995f);
    xn[f * 2u + 1u] = clampf(out_r, -0.995f, 0.995f);

    s_pre_write = (s_pre_write + 1u) & kPreMask;
    s_diff_write = (s_diff_write + 1u) & kDiffMask;
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
