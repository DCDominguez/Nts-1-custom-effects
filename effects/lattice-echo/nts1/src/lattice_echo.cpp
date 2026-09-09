#include "userdelfx.h"

#include <stdint.h>

namespace {

static const float kSampleRate = 48000.0f;
static const uint32_t kBufferSize = 65536u;
static const uint32_t kBufferMask = kBufferSize - 1u;
static const float kParamSlew = 0.0015f;

__sdram float s_delay_l[kBufferSize];
__sdram float s_delay_r[kBufferSize];

static uint32_t s_write = 0u;
static uint32_t s_event = 0u;
static uint32_t s_capture_remaining = 0u;
static uint32_t s_refractory = 0u;
static bool s_capture_this_event = false;
static bool s_flip = false;

static float s_fast_env = 0.0f;
static float s_slow_env = 0.0f;
static float s_fb_lp_l = 0.0f;
static float s_fb_lp_r = 0.0f;

static float s_distance_target = 0.42f;
static float s_pick_target = 0.45f;
static float s_mix_target = 0.35f;
static float s_distance = 0.42f;
static float s_pick = 0.45f;
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

static inline float absf(float x) { return x < 0.0f ? -x : x; }

static inline float read_frac(const float *buffer, float delay_samples) {
  float pos = static_cast<float>(s_write) - delay_samples;
  while (pos < 0.0f) pos += static_cast<float>(kBufferSize);
  const uint32_t i0 = static_cast<uint32_t>(pos) & kBufferMask;
  const uint32_t i1 = (i0 + 1u) & kBufferMask;
  const float frac = pos - static_cast<float>(static_cast<uint32_t>(pos));
  return buffer[i0] + (buffer[i1] - buffer[i0]) * frac;
}

static inline uint32_t pick_index(float p) {
  uint32_t idx = static_cast<uint32_t>(p * 7.999f);
  if (idx > 7u) idx = 7u;
  return idx;
}

static inline bool selected_event(uint32_t mode, uint32_t event) {
  switch (mode) {
    case 0u: return (event % 8u) == 0u;
    case 1u: return (event % 5u) == 0u;
    case 2u: return (event % 4u) == 0u;
    case 3u: return (event % 3u) == 0u;
    case 4u: return (event % 2u) == 0u;
    case 5u: { const uint32_t r = event % 5u; return r == 2u || r == 0u; }
    case 6u: { const uint32_t r = event & 7u; return r == 1u || r == 2u || r == 5u; }
    default: return true;
  }
}

static inline float distance_samples(float distance) {
  const float seconds = 0.14f + 0.91f * distance * distance;
  return seconds * kSampleRate;
}

static void reset_state(void) {
  for (uint32_t i = 0u; i < kBufferSize; ++i) s_delay_l[i] = s_delay_r[i] = 0.0f;
  s_write = 0u;
  s_event = 0u;
  s_capture_remaining = 0u;
  s_refractory = 0u;
  s_capture_this_event = false;
  s_flip = false;
  s_fast_env = s_slow_env = 0.0f;
  s_fb_lp_l = s_fb_lp_r = 0.0f;
  s_distance_target = s_distance = 0.42f;
  s_pick_target = s_pick = 0.45f;
  s_mix_target = s_mix = 0.35f;
}

} // namespace

void DELFX_INIT(uint32_t platform, uint32_t api) {
  (void)platform;
  (void)api;
  reset_state();
}

void DELFX_PROCESS(float *xn, uint32_t frames) {
  for (uint32_t f = 0u; f < frames; ++f) {
    s_distance += (s_distance_target - s_distance) * kParamSlew;
    s_pick += (s_pick_target - s_pick) * kParamSlew;
    s_mix += (s_mix_target - s_mix) * kParamSlew;

    const float in_l = xn[f * 2u];
    const float in_r = xn[f * 2u + 1u];
    const float mid_abs = absf(0.5f * (in_l + in_r));

    s_fast_env += (mid_abs - s_fast_env) * 0.055f;
    s_slow_env += (mid_abs - s_slow_env) * 0.0014f;
    if (s_refractory > 0u) --s_refractory;

    const bool onset = (s_refractory == 0u) &&
                       (s_fast_env > s_slow_env * 1.72f + 0.010f) &&
                       (s_fast_env > 0.018f);
    if (onset) {
      ++s_event;
      s_capture_this_event = selected_event(pick_index(s_pick), s_event);
      s_capture_remaining = static_cast<uint32_t>(kSampleRate * (0.040f + 0.055f * s_distance));
      s_refractory = static_cast<uint32_t>(kSampleRate * 0.030f);
      s_flip = !s_flip;
    }

    const float delay = distance_samples(s_distance);
    const float echo_l = read_frac(s_delay_l, delay);
    const float echo_r = read_frac(s_delay_r, delay * 1.011f + 13.0f);

    s_fb_lp_l += (echo_l - s_fb_lp_l) * 0.36f;
    s_fb_lp_r += (echo_r - s_fb_lp_r) * 0.36f;
    const float feedback = 0.43f + 0.16f * s_distance;

    float inject_l = 0.0f;
    float inject_r = 0.0f;
    if (s_capture_this_event && s_capture_remaining > 0u) {
      const float focus_l = s_flip ? 0.88f : 0.52f;
      const float focus_r = s_flip ? 0.52f : 0.88f;
      inject_l = in_l * focus_l;
      inject_r = in_r * focus_r;
      --s_capture_remaining;
      if (s_capture_remaining == 0u) s_capture_this_event = false;
    }

    s_delay_l[s_write] = clamp_audio(inject_l + (s_fb_lp_l * 0.82f + s_fb_lp_r * 0.18f) * feedback);
    s_delay_r[s_write] = clamp_audio(inject_r + (s_fb_lp_r * 0.82f + s_fb_lp_l * 0.18f) * feedback);

    const float wet_l = echo_l * 0.78f;
    const float wet_r = echo_r * 0.78f;
    xn[f * 2u] = clamp_audio(in_l * (1.0f - s_mix) + wet_l * s_mix);
    xn[f * 2u + 1u] = clamp_audio(in_r * (1.0f - s_mix) + wet_r * s_mix);

    s_write = (s_write + 1u) & kBufferMask;
  }
}

void DELFX_SUSPEND(void) { reset_state(); }
void DELFX_RESUME(void) { reset_state(); }

void DELFX_PARAM(uint8_t index, int32_t value) {
  const float normalized = clamp01(q31_to_f32(value));
  if (index == k_user_delfx_param_time) s_distance_target = normalized;
  else if (index == k_user_delfx_param_depth) s_pick_target = normalized;
  else if (index == k_user_delfx_param_shift_depth) s_mix_target = normalized;
}
