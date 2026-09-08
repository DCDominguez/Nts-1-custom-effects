#include "userdelfx.h"

#include <stdint.h>

namespace {

// NTS-1 MkI target assumption used by established logue SDK delay examples.
// Keep this isolated so it is easy to revisit if the platform target changes.
static const float kSampleRate = 48000.0f;

// 8192 samples = ~170 ms at 48 kHz, comfortably beyond the M1 field.
static const uint32_t kDelayLineSize = (1u << 13);
static const uint32_t kDelayLineMask = kDelayLineSize - 1u;
static const uint32_t kNumVoices = 4u;

struct VoiceConfig {
  float tight_delay_ms;
  float wide_delay_ms;
  float base_rate_hz;
  float max_motion_ms;
  float base_pan;
  float initial_phase;
  float gain;
};

// M1 deliberately separates timing, modulation rate/phase, and stereo position.
// True fixed pitch offsets arrive in M2.
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

float s_spread = 0.35f;
float s_divergence = 0.35f;
float s_wet = 0.35f;
float s_dry = 0.65f;

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

static inline float lerp(float a, float b, float t) {
  return a + (b - a) * t;
}

static inline float ms_to_samples(float ms) {
  return ms * (kSampleRate * 0.001f);
}

static inline float wrap_read_position(float pos) {
  while (pos < 0.0f) {
    pos += static_cast<float>(kDelayLineSize);
  }
  while (pos >= static_cast<float>(kDelayLineSize)) {
    pos -= static_cast<float>(kDelayLineSize);
  }
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
  // Lightweight prototype pan law. M1 prioritizes CPU headroom and field testing.
  if (pan < -1.0f) pan = -1.0f;
  if (pan > 1.0f) pan = 1.0f;
  left = 0.5f * (1.0f - pan);
  right = 0.5f * (1.0f + pan);
}

static inline float voice_rate(uint32_t i) {
  // Even at minimum DIVERGENCE, do not collapse to one shared rate.
  const float common_rate = 0.19f;
  const float independence = 0.25f + 0.75f * s_divergence;
  return lerp(common_rate, kVoices[i].base_rate_hz, independence);
}

static inline float voice_pan(uint32_t i) {
  // Low DIVERGENCE keeps a narrower but still distributed field.
  const float width = 0.35f + 0.65f * s_divergence;
  return kVoices[i].base_pan * width;
}

static inline float voice_delay_samples(uint32_t i) {
  const VoiceConfig &v = kVoices[i];

  const float base_ms = lerp(v.tight_delay_ms, v.wide_delay_ms, s_spread);
  const float motion_scale = 0.12f + 0.88f * s_divergence;
  const float motion_ms = v.max_motion_ms * motion_scale * fx_sinf(s_phase[i]);

  float delay_ms = base_ms + motion_ms;
  if (delay_ms < 2.0f) delay_ms = 2.0f;

  const float max_ms = (static_cast<float>(kDelayLineSize - 2u) / kSampleRate) * 1000.0f;
  if (delay_ms > max_ms) delay_ms = max_ms;

  return ms_to_samples(delay_ms);
}

static inline void advance_phase(uint32_t i) {
  s_phase[i] += voice_rate(i) / kSampleRate;
  if (s_phase[i] >= 1.0f) {
    s_phase[i] -= 1.0f;
  }
}

} // namespace

void DELFX_INIT(uint32_t platform, uint32_t api) {
  (void)platform;
  (void)api;

  s_write_index = 0u;
  s_spread = 0.35f;
  s_divergence = 0.35f;
  s_wet = 0.35f;
  s_dry = 0.65f;

  for (uint32_t i = 0u; i < kNumVoices; ++i) {
    s_phase[i] = kVoices[i].initial_phase;
  }

  for (uint32_t i = 0u; i < kDelayLineSize; ++i) {
    s_delay_l[i] = 0.0f;
    s_delay_r[i] = 0.0f;
  }
}

void DELFX_PROCESS(float *xn, uint32_t frames) {
  float *x = xn;
  float *const end = xn + (frames * 2u);

  while (x < end) {
    const float in_l = x[0];
    const float in_r = x[1];

    // One shared stereo history buffer. Voices are independent read heads.
    s_delay_l[s_write_index] = in_l;
    s_delay_r[s_write_index] = in_r;

    float wet_l = 0.0f;
    float wet_r = 0.0f;

    for (uint32_t i = 0u; i < kNumVoices; ++i) {
      const float delay_samples = voice_delay_samples(i);
      const float read_pos = static_cast<float>(s_write_index) - delay_samples;

      const float tap_l = read_frac(read_pos, s_delay_l);
      const float tap_r = read_frac(read_pos, s_delay_r);

      // M1 forms a centered voice source from the stereo tap, then places that
      // voice at an intentional stereo anchor. The dry path preserves the
      // original stereo image. Later revisions can retain more per-voice width.
      const float voice = 0.5f * (tap_l + tap_r);

      float pan_l = 0.5f;
      float pan_r = 0.5f;
      pan_gains(voice_pan(i), pan_l, pan_r);

      wet_l += voice * pan_l * kVoices[i].gain;
      wet_r += voice * pan_r * kVoices[i].gain;

      advance_phase(i);
    }

    // Safety bound only. Normal constellation gain should stay below clipping.
    wet_l = clamp_audio(wet_l);
    wet_r = clamp_audio(wet_r);

    x[0] = clamp_audio(in_l * s_dry + wet_l * s_wet);
    x[1] = clamp_audio(in_r * s_dry + wet_r * s_wet);

    s_write_index = (s_write_index + 1u) & kDelayLineMask;
    x += 2;
  }
}

void DELFX_PARAM(uint8_t index, int32_t value) {
  const float normalized = clamp01(q31_to_f32(value));

  switch (index) {
    case k_user_delfx_param_time:
      // TIME becomes SPREAD: tight chorus -> separated ensemble/doubler.
      s_spread = normalized;
      break;

    case k_user_delfx_param_depth:
      // DEPTH becomes DIVERGENCE: motion + rate separation + stereo width.
      // M2 will also scale true pitch offsets from this macro.
      s_divergence = normalized;
      break;

    case k_user_delfx_param_shift_depth:
      // Conventional NTS-1 delay MIX control.
      s_wet = normalized;
      s_dry = 1.0f - normalized;
      break;

    default:
      break;
  }
}
