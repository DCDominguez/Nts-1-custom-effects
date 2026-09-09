#include "usermodfx.h"

#include <stdint.h>

namespace {

// M1 implementation assumption. Hardware QA must verify that the audible
// carrier range behaves as expected on the original NTS-1 MkI.
static const float kSampleRate = 48000.0f;
static const float kMinCarrierHz = 0.2f;
static const float kMaxCarrierHz = 4800.0f;
static const float kSlew = 0.0020f;
static const float kMaxStereoOffsetCycles = 1.0f / 12.0f; // 30 degrees per side

static float s_time_target = 0.0f;
static float s_depth_target = 0.0f;
static float s_time = 0.0f;
static float s_depth = 0.0f;
static float s_phase = 0.0f;

static inline float clamp01(float x) {
  if (x < 0.0f) return 0.0f;
  if (x > 1.0f) return 1.0f;
  return x;
}

static inline float lerp(float a, float b, float t) {
  return a + (b - a) * t;
}

static inline float wrap01(float x) {
  if (x < 0.0f) x += 1.0f;
  if (x >= 1.0f) x -= 1.0f;
  return x;
}

static inline float carrier_hz(float time) {
  // Fifth-power curve gives most of TIME's travel to tremolo and low-audio
  // rates while still reaching metallic audio-rate modulation at the top.
  const float t2 = time * time;
  const float t4 = t2 * t2;
  const float t5 = t4 * time;
  return kMinCarrierHz + t5 * (kMaxCarrierHz - kMinCarrierHz);
}

static inline float stereo_offset(float depth) {
  // Keep AM/tremolo centered. Stereo phase separation appears only after the
  // AM midpoint as the effect moves toward true bipolar ring modulation.
  float ring_region = (depth - 0.5f) * 2.0f;
  if (ring_region < 0.0f) ring_region = 0.0f;
  if (ring_region > 1.0f) ring_region = 1.0f;
  return ring_region * kMaxStereoOffsetCycles;
}

static inline float modulation_factor(float carrier, float depth) {
  const float am = 0.5f + 0.5f * carrier; // unipolar 0..1

  if (depth <= 0.5f) {
    // Dry -> full AM.
    return lerp(1.0f, am, depth * 2.0f);
  }

  // Full AM -> bipolar ring modulation.
  return lerp(am, carrier, (depth - 0.5f) * 2.0f);
}

static inline void reset_phase(void) {
  s_phase = 0.0f;
}

} // namespace

void MODFX_INIT(uint32_t platform, uint32_t api) {
  (void)platform;
  (void)api;

  s_time_target = 0.0f;
  s_depth_target = 0.0f;
  s_time = 0.0f;
  s_depth = 0.0f;
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

    const float phase_l = wrap01(s_phase - offset);
    const float phase_r = wrap01(s_phase + offset);

    const float carrier_l = fx_sinf(phase_l);
    const float carrier_r = fx_sinf(phase_r);

    const float factor_l = modulation_factor(carrier_l, s_depth);
    const float factor_r = modulation_factor(carrier_r, s_depth);

    const float in_l = main_xn[i * 2u];
    const float in_r = main_xn[i * 2u + 1u];

    // Modulation factors are bounded to [-1, 1], so CARRIER never amplifies
    // beyond the instantaneous magnitude of its input in M1.
    main_yn[i * 2u] = in_l * factor_l;
    main_yn[i * 2u + 1u] = in_r * factor_r;

    s_phase += phase_inc;
    if (s_phase >= 1.0f) s_phase -= 1.0f;
  }
}

void MODFX_SUSPEND(void) {
  reset_phase();
}

void MODFX_RESUME(void) {
  reset_phase();
}

void MODFX_PARAM(uint8_t index, int32_t value) {
  const float normalized = clamp01(q31_to_f32(value));

  switch (index) {
    case k_user_modfx_param_time:
      s_time_target = normalized;
      break;

    case k_user_modfx_param_depth:
      s_depth_target = normalized;
      break;

    default:
      break;
  }
}
