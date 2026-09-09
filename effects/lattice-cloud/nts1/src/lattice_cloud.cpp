#include "userrevfx.h"

#include <stdint.h>

namespace {

static const float kSampleRate = 48000.0f;
static const uint32_t kPreSize = 8192u;
static const uint32_t kPreMask = kPreSize - 1u;
static const uint32_t kFdnSize = 16384u;
static const uint32_t kFdnMask = kFdnSize - 1u;
static const float kParamSlew = 0.0012f;

__sdram float s_pre_l[kPreSize];
__sdram float s_pre_r[kPreSize];
__sdram float s_fdn0[kFdnSize];
__sdram float s_fdn1[kFdnSize];
__sdram float s_fdn2[kFdnSize];
__sdram float s_fdn3[kFdnSize];

static uint32_t s_pre_write = 0u;
static uint32_t s_fdn_write = 0u;
static float s_lp0 = 0.0f, s_lp1 = 0.0f, s_lp2 = 0.0f, s_lp3 = 0.0f;
static float s_phase = 0.0f;

static float s_space_target = 0.58f;
static float s_coalesce_target = 0.48f;
static float s_mix_target = 0.35f;
static float s_space = 0.58f;
static float s_coalesce = 0.48f;
static float s_mix = 0.35f;

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

static inline float wrap01(float x) {
  if (x >= 1.0f) x -= 1.0f;
  if (x < 0.0f) x += 1.0f;
  return x;
}

static inline float read_pre(const float *buffer, float delay) {
  float pos = static_cast<float>(s_pre_write) - delay;
  while (pos < 0.0f) pos += static_cast<float>(kPreSize);
  const uint32_t i0 = static_cast<uint32_t>(pos) & kPreMask;
  const uint32_t i1 = (i0 + 1u) & kPreMask;
  const float frac = pos - static_cast<float>(static_cast<uint32_t>(pos));
  return buffer[i0] + (buffer[i1] - buffer[i0]) * frac;
}

static inline float read_fdn(const float *buffer, float delay) {
  float pos = static_cast<float>(s_fdn_write) - delay;
  while (pos < 0.0f) pos += static_cast<float>(kFdnSize);
  const uint32_t i0 = static_cast<uint32_t>(pos) & kFdnMask;
  const uint32_t i1 = (i0 + 1u) & kFdnMask;
  const float frac = pos - static_cast<float>(static_cast<uint32_t>(pos));
  return buffer[i0] + (buffer[i1] - buffer[i0]) * frac;
}

static void reset_state(void) {
  for (uint32_t i = 0u; i < kPreSize; ++i) s_pre_l[i] = s_pre_r[i] = 0.0f;
  for (uint32_t i = 0u; i < kFdnSize; ++i) s_fdn0[i] = s_fdn1[i] = s_fdn2[i] = s_fdn3[i] = 0.0f;
  s_pre_write = s_fdn_write = 0u;
  s_lp0 = s_lp1 = s_lp2 = s_lp3 = 0.0f;
  s_phase = 0.0f;
  s_space_target = s_space = 0.58f;
  s_coalesce_target = s_coalesce = 0.48f;
  s_mix_target = s_mix = 0.35f;
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
    s_coalesce += (s_coalesce_target - s_coalesce) * kParamSlew;
    s_mix += (s_mix_target - s_mix) * kParamSlew;

    const float in_l = xn[f * 2u];
    const float in_r = xn[f * 2u + 1u];
    const float mid = 0.5f * (in_l + in_r);
    const float side = 0.5f * (in_l - in_r);

    s_pre_l[s_pre_write] = in_l;
    s_pre_r[s_pre_write] = in_r;

    const float pre_scale = 0.66f + 0.86f * s_space;
    const float e0 = 0.5f * (read_pre(s_pre_l, 733.0f * pre_scale) + read_pre(s_pre_r, 911.0f * pre_scale));
    const float e1 = 0.5f * (read_pre(s_pre_l, 1187.0f * pre_scale) + read_pre(s_pre_r, 1423.0f * pre_scale));
    const float e2 = 0.5f * (read_pre(s_pre_l, 1811.0f * pre_scale) + read_pre(s_pre_r, 2113.0f * pre_scale));
    const float e3 = 0.5f * (read_pre(s_pre_l, 2609.0f * pre_scale) + read_pre(s_pre_r, 3049.0f * pre_scale));

    s_phase = wrap01(s_phase + (0.018f + 0.070f * s_coalesce) / kSampleRate);
    const float mod = 7.0f + 25.0f * s_coalesce;
    const float room = 0.80f + 0.42f * s_space;
    const float r0 = read_fdn(s_fdn0, 2953.0f * room + mod * fx_sinf(s_phase));
    const float r1 = read_fdn(s_fdn1, 3821.0f * room + mod * fx_sinf(s_phase + 0.23f));
    const float r2 = read_fdn(s_fdn2, 4999.0f * room + mod * fx_sinf(s_phase + 0.51f));
    const float r3 = read_fdn(s_fdn3, 6421.0f * room + mod * fx_sinf(s_phase + 0.77f));

    const float damping = 0.30f - 0.12f * s_coalesce;
    s_lp0 += (r0 - s_lp0) * damping;
    s_lp1 += (r1 - s_lp1) * damping;
    s_lp2 += (r2 - s_lp2) * damping;
    s_lp3 += (r3 - s_lp3) * damping;

    const float h0 = 0.5f * (s_lp0 + s_lp1 + s_lp2 + s_lp3);
    const float h1 = 0.5f * (s_lp0 - s_lp1 + s_lp2 - s_lp3);
    const float h2 = 0.5f * (s_lp0 + s_lp1 - s_lp2 - s_lp3);
    const float h3 = 0.5f * (s_lp0 - s_lp1 - s_lp2 + s_lp3);

    const float early = 0.25f * (e0 + e1 + e2 + e3);

    // 0.1-1: stronger injection and feedback so the generated phrase actually
    // blooms. Low COALESCE keeps early fragments; high COALESCE leans hard into
    // the FDN until those fragments become one harmonic field.
    const float inject_dense = mid * (0.24f + 0.34f * s_coalesce) + early * (0.22f + 0.52f * s_coalesce);
    const float inject_side = side * (0.16f + 0.22f * s_coalesce);
    const float feedback = 0.58f + 0.30f * s_space + 0.075f * s_coalesce;

    s_fdn0[s_fdn_write] = clamp_audio(inject_dense + h0 * feedback);
    s_fdn1[s_fdn_write] = clamp_audio(inject_side + h1 * feedback);
    s_fdn2[s_fdn_write] = clamp_audio(inject_dense + h2 * feedback);
    s_fdn3[s_fdn_write] = clamp_audio(-inject_side + h3 * feedback);

    const float sparse_l = 0.70f * e0 + 0.42f * e2;
    const float sparse_r = 0.70f * e1 + 0.42f * e3;
    const float dense_l = 0.55f * (r0 + r2) + 0.17f * (r1 - r3);
    const float dense_r = 0.55f * (r1 + r3) + 0.17f * (r2 - r0);
    const float wet_l = sparse_l * (1.0f - s_coalesce) + dense_l * s_coalesce;
    const float wet_r = sparse_r * (1.0f - s_coalesce) + dense_r * s_coalesce;

    // The previous equal crossfade reduced the whole chain as MIX rose. Keep a
    // strong dry spine and add the cloud in parallel instead.
    const float dry_gain = 1.0f - 0.30f * s_mix;
    const float wet_gain = 0.60f + 0.95f * s_mix;
    xn[f * 2u] = clamp_audio(in_l * dry_gain + wet_l * wet_gain * s_mix);
    xn[f * 2u + 1u] = clamp_audio(in_r * dry_gain + wet_r * wet_gain * s_mix);

    s_pre_write = (s_pre_write + 1u) & kPreMask;
    s_fdn_write = (s_fdn_write + 1u) & kFdnMask;
  }
}

void REVFX_SUSPEND(void) { reset_state(); }
void REVFX_RESUME(void) { reset_state(); }

void REVFX_PARAM(uint8_t index, int32_t value) {
  const float normalized = clamp01(q31_to_f32(value));
  if (index == k_user_revfx_param_time) s_space_target = normalized;
  else if (index == k_user_revfx_param_depth) s_coalesce_target = normalized;
  else if (index == k_user_revfx_param_shift_depth) s_mix_target = normalized;
}
