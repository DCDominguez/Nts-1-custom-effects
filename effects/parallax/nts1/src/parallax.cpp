#include "userdelfx.h"

#include <stdint.h>

namespace {

static const float kSampleRate = 48000.0f;
static const uint32_t kDelayLineSize = (1u << 13);
static const uint32_t kDelayLineMask = kDelayLineSize - 1u;
static const uint32_t kNumVoices = 4u;
static const float kLn2Over1200 = 0.0005776226504666211f;
static const float kPitchWindowSamples = 1024.0f;
static const float kVoiceAMaxCents = -9.0f;

struct VoiceConfig {
  float tight_delay_ms;
  float wide_delay_ms;
  float base_rate_hz;
  float max_motion_ms;
  float base_pan;
  float initial_phase;
  float gain;
};

static const VoiceConfig kVoices[kNumVoices] = {
  { 8.0f,  8.0f, 0.17f, 1.2f, -0.75f, 0.03f, 0.36f },
  {11.0f, 22.0f, 0.23f, 1.7f, -0.20f, 0.29f, 0.34f },
  {14.0f, 38.0f, 0.11f, 2.1f,  0.25f, 0.57f, 0.34f },
  {18.0f, 56.0f, 0.31f, 2.6f,  0.78f, 0.81f, 0.32f }
};

__sdram float s_delay_l[kDelayLineSize];
__sdram float s_delay_r[kDelayLineSize];

uint32_t s_write_index = 0u;
float s_phase[kNumVoices] = {0.0f, 0.0f, 0.0f, 0.0f};
float s_pitch_phase = 0.0f;

float s_spread_target = 0.35f;
float s_divergence_target = 0.35f;
float s_mix_target = 0.35f;
float s_spread = 0.35f;
float s_divergence = 0.35f;
float s_wet = 0.35f;
float s_dry = 0.65f;

static const float kParamSlew = 0.0015f;

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

static inline float lerp(float a, float b, float t) { return a + (b - a) * t; }
static inline float ms_to_samples(float ms) { return ms * (kSampleRate * 0.001f); }

static inline float wrap01(float x) {
  while (x >= 1.0f) x -= 1.0f;
  while (x < 0.0f) x += 1.0f;
  return x;
}

static inline float wrap_read_position(float pos) {
  while (pos < 0.0f) pos += static_cast<float>(kDelayLineSize);
  while (pos >= static_cast<float>(kDelayLineSize)) pos -= static_cast<float>(kDelayLineSize);
  return pos;
}

static inline float read_frac(float pos, const float *buffer) {
  pos = wrap_read_position(pos);
  const uint32_t base = static_cast<uint32_t>(pos);
  const float frac = pos - static_cast<float>(base);
  const float a = buffer[base & kDelayLineMask];
  const float b = buffer[(base + 1u) & kDelayLineMask];
  return a + (b - a) * frac;
}

static inline void pan_gains(float pan, float &left, float &right) {
  if (pan < -1.0f) pan = -1.0f;
  if (pan > 1.0f) pan = 1.0f;
  left = 0.5f * (1.0f - pan);
  right = 0.5f * (1.0f + pan);
}

static inline float window_sin2(float phase) {
  const float s = fx_sinf(0.5f * phase);
  return s * s;
}

static inline float voice_rate(uint32_t i) {
  const float common_rate = 0.19f;
  const float independence = 0.25f + 0.75f * s_divergence;
  return lerp(common_rate, kVoices[i].base_rate_hz, independence);
}

static inline float voice_pan(uint32_t i) {
  const float width = 0.35f + 0.65f * s_divergence;
  return kVoices[i].base_pan * width;
}

static inline float base_delay_samples(uint32_t i) {
  const VoiceConfig &v = kVoices[i];
  return ms_to_samples(lerp(v.tight_delay_ms, v.wide_delay_ms, s_spread));
}

static inline float chorus_delay_samples(uint32_t i) {
  const VoiceConfig &v = kVoices[i];
  const float motion_scale = 0.12f + 0.88f * s_divergence;
  const float motion_ms = v.max_motion_ms * motion_scale * fx_sinf(s_phase[i]);
  float delay_ms = lerp(v.tight_delay_ms, v.wide_delay_ms, s_spread) + motion_ms;
  if (delay_ms < 2.0f) delay_ms = 2.0f;
  const float max_ms = (static_cast<float>(kDelayLineSize - 2u) / kSampleRate) * 1000.0f;
  if (delay_ms > max_ms) delay_ms = max_ms;
  return ms_to_samples(delay_ms);
}

static inline float pitch_ratio_a() {
  // M2 introduces one stable micro-pitch voice. DIVERGENCE scales Voice A
  // continuously from unison to the intended -9 cent constellation target.
  const float cents = kVoiceAMaxCents * s_divergence;
  return 1.0f + cents * kLn2Over1200;
}

static inline float pitched_read_mono(float base_delay) {
  const float ratio = pitch_ratio_a();
  const float slope = 1.0f - ratio;
  const float p1 = s_pitch_phase;
  const float p2 = wrap01(p1 + 0.5f);
  const float w1 = window_sin2(p1);
  const float w2 = window_sin2(p2);
  float ws = w1 + w2;
  if (ws < 0.001f) ws = 0.001f;

  const float d1 = base_delay + slope * p1 * kPitchWindowSamples;
  const float d2 = base_delay + slope * p2 * kPitchWindowSamples;
  const float l = (read_frac(static_cast<float>(s_write_index) - d1, s_delay_l) * w1 +
                   read_frac(static_cast<float>(s_write_index) - d2, s_delay_l) * w2) / ws;
  const float r = (read_frac(static_cast<float>(s_write_index) - d1, s_delay_r) * w1 +
                   read_frac(static_cast<float>(s_write_index) - d2, s_delay_r) * w2) / ws;
  return 0.5f * (l + r);
}

static inline void advance_phase(uint32_t i) {
  s_phase[i] = wrap01(s_phase[i] + voice_rate(i) / kSampleRate);
}

static inline void reset_state() {
  s_write_index = 0u;
  s_pitch_phase = 0.0f;
  s_spread_target = s_spread = 0.35f;
  s_divergence_target = s_divergence = 0.35f;
  s_mix_target = 0.35f;
  s_wet = 0.35f;
  s_dry = 0.65f;
  for (uint32_t i = 0u; i < kNumVoices; ++i) s_phase[i] = kVoices[i].initial_phase;
  for (uint32_t i = 0u; i < kDelayLineSize; ++i) s_delay_l[i] = s_delay_r[i] = 0.0f;
}

} // namespace

void DELFX_INIT(uint32_t platform, uint32_t api) {
  (void)platform;
  (void)api;
  reset_state();
}

void DELFX_PROCESS(float *xn, uint32_t frames) {
  float *x = xn;
  float *const end = xn + frames * 2u;

  while (x < end) {
    s_spread += (s_spread_target - s_spread) * kParamSlew;
    s_divergence += (s_divergence_target - s_divergence) * kParamSlew;
    const float mix = s_wet + (s_mix_target - s_wet) * kParamSlew;
    s_wet = mix;
    s_dry = 1.0f - s_wet;

    const float in_l = x[0];
    const float in_r = x[1];
    s_delay_l[s_write_index] = in_l;
    s_delay_r[s_write_index] = in_r;

    float wet_l = 0.0f;
    float wet_r = 0.0f;

    // Voice A: true dual-head pitch shift. The tiny retained M1 motion keeps
    // the voice alive without dominating the stable cents offset.
    const float motion_a = ms_to_samples(kVoices[0].max_motion_ms * 0.20f * s_divergence * fx_sinf(s_phase[0]));
    const float voice_a = pitched_read_mono(base_delay_samples(0) + motion_a);
    float pan_l = 0.5f, pan_r = 0.5f;
    pan_gains(voice_pan(0), pan_l, pan_r);
    wet_l += voice_a * pan_l * kVoices[0].gain;
    wet_r += voice_a * pan_r * kVoices[0].gain;
    advance_phase(0);

    // Voices B-D remain the hardware-proven M1 chorus field for this milestone.
    for (uint32_t i = 1u; i < kNumVoices; ++i) {
      const float delay_samples = chorus_delay_samples(i);
      const float read_pos = static_cast<float>(s_write_index) - delay_samples;
      const float tap_l = read_frac(read_pos, s_delay_l);
      const float tap_r = read_frac(read_pos, s_delay_r);
      const float voice = 0.5f * (tap_l + tap_r);
      pan_gains(voice_pan(i), pan_l, pan_r);
      wet_l += voice * pan_l * kVoices[i].gain;
      wet_r += voice * pan_r * kVoices[i].gain;
      advance_phase(i);
    }

    wet_l = clamp_audio(wet_l);
    wet_r = clamp_audio(wet_r);
    x[0] = clamp_audio(in_l * s_dry + wet_l * s_wet);
    x[1] = clamp_audio(in_r * s_dry + wet_r * s_wet);

    s_pitch_phase = wrap01(s_pitch_phase + 1.0f / kPitchWindowSamples);
    s_write_index = (s_write_index + 1u) & kDelayLineMask;
    x += 2;
  }
}

void DELFX_SUSPEND(void) { reset_state(); }
void DELFX_RESUME(void) { reset_state(); }

void DELFX_PARAM(uint8_t index, int32_t value) {
  const float normalized = clamp01(q31_to_f32(value));
  switch (index) {
    case k_user_delfx_param_time:
      s_spread_target = normalized;
      break;
    case k_user_delfx_param_depth:
      s_divergence_target = normalized;
      break;
    case k_user_delfx_param_shift_depth:
      s_mix_target = normalized;
      break;
    default:
      break;
  }
}
