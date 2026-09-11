#include "usermodfx.h"
#include <stdint.h>

namespace {

static constexpr float kSampleRate = 48000.0f;
static constexpr float kSlew = 0.0015f;

static float s_rate_target = 0.25f;
static float s_depth_target = 0.0f;
static float s_rate = 0.25f;
static float s_depth = 0.0f;

static float s_x = 0.231f;
static float s_y = 0.677f;
static float s_pan = 0.0f;
static float s_pan_target = 0.0f;
static uint32_t s_counter = 0u;

static inline float clamp01(float x) { return x < 0.0f ? 0.0f : (x > 1.0f ? 1.0f : x); }
static inline float clamp11(float x) { return x < -1.0f ? -1.0f : (x > 1.0f ? 1.0f : x); }
static inline float clamp_audio(float x) { return x < -1.0f ? -1.0f : (x > 1.0f ? 1.0f : x); }

static inline void step_chaos() {
  // Coupled logistic maps: deterministic, bounded, and intentionally not a
  // periodic LFO. Cross-coupling prevents either map settling into a short loop.
  const float nx = 3.875f * s_x * (1.0f - s_x) + 0.021f * (s_y - 0.5f);
  const float ny = 3.905f * s_y * (1.0f - s_y) + 0.017f * (s_x - 0.5f);
  s_x = nx < 0.001f ? 0.001f : (nx > 0.999f ? 0.999f : nx);
  s_y = ny < 0.001f ? 0.001f : (ny > 0.999f ? 0.999f : ny);

  // Sum and difference both contribute so the orbit visits the center as well
  // as the edges instead of behaving like a two-state random pan.
  const float orbit = (s_x - s_y) * 1.55f + ((s_x + s_y) - 1.0f) * 0.42f;
  s_pan_target = clamp11(orbit);
}

static inline void constant_power_pan(float pan, float &left, float &right) {
  pan = clamp11(pan);
  const float phase = 0.125f * (pan + 1.0f); // 0..1/4 cycle = 0..pi/2
  left = fx_cosf(phase);
  right = fx_sinf(phase);
}

static inline void reset() {
  s_x = 0.231f;
  s_y = 0.677f;
  s_pan = s_pan_target = 0.0f;
  s_counter = 0u;
}

} // namespace

void MODFX_INIT(uint32_t platform, uint32_t api) {
  (void)platform;
  (void)api;
  // Initialization must be independent of any prior in-process state. The host
  // will resend the selected program's parameters after init, but leaving the
  // smoothed controls from an earlier lifetime changes how many chaotic updates
  // occur during the first slew and therefore changes the deterministic orbit.
  s_rate_target = s_rate = 0.25f;
  s_depth_target = s_depth = 0.0f;
  reset();
}

void MODFX_PROCESS(const float *main_xn, float *main_yn,
                   const float *sub_xn, float *sub_yn,
                   uint32_t frames) {
  (void)sub_xn;
  (void)sub_yn;

  for (uint32_t i = 0u; i < frames; ++i) {
    s_rate += (s_rate_target - s_rate) * kSlew;
    s_depth += (s_depth_target - s_depth) * kSlew;

    const float updates = 0.7f + 48.0f * s_rate * s_rate;
    uint32_t period = static_cast<uint32_t>(kSampleRate / updates);
    if (period < 48u) period = 48u;
    if (++s_counter >= period) {
      s_counter = 0u;
      step_chaos();
    }

    // Interpolate between chaotic targets so the orbit is continuous rather
    // than a sequence of random-looking pan jumps.
    const float follow = 0.0008f + 0.0042f * s_rate;
    s_pan += (s_pan_target - s_pan) * follow;

    const float in_l = main_xn[2u * i];
    const float in_r = main_xn[2u * i + 1u];
    const float mid = 0.5f * (in_l + in_r);
    const float side = 0.5f * (in_l - in_r);

    float pan_l = 0.70710678f;
    float pan_r = 0.70710678f;
    constant_power_pan(s_pan * s_depth, pan_l, pan_r);

    // Keep part of the original side component so stereo sources retain their
    // orientation while the common energy follows the chaotic orbit.
    const float side_keep = 1.0f - 0.55f * s_depth;
    const float moved_l = mid * pan_l * 1.41421356f + side * side_keep;
    const float moved_r = mid * pan_r * 1.41421356f - side * side_keep;

    main_yn[2u * i] = clamp_audio(in_l + (moved_l - in_l) * s_depth);
    main_yn[2u * i + 1u] = clamp_audio(in_r + (moved_r - in_r) * s_depth);
  }
}

void MODFX_SUSPEND(void) { reset(); }
void MODFX_RESUME(void) { reset(); }

void MODFX_PARAM(uint8_t index, int32_t value) {
  const float v = clamp01(q31_to_f32(value));
  if (index == k_user_modfx_param_time) s_rate_target = v;
  else if (index == k_user_modfx_param_depth) s_depth_target = v;
}
