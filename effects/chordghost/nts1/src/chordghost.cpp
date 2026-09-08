#include "userdelfx.h"
#include "chord_table.h"

#include <stdint.h>

namespace {

static const float kSampleRate = 48000.0f;
static const uint32_t kDelayLineSize = (1u << 17); // 131072 samples / channel
static const uint32_t kDelayLineMask = kDelayLineSize - 1u;
static const float kFeedback = 0.58f;
static const float kDelaySlew = 0.0005f;

// Quarter-note multipliers.
// 1/16, 1/8, dotted 1/8, 1/4, dotted 1/4, 1/2
static const float kDelayDivisions[] = {
  0.25f,
  0.50f,
  0.75f,
  1.00f,
  1.50f,
  2.00f
};
static const uint32_t kNumDelayDivisions = sizeof(kDelayDivisions) / sizeof(kDelayDivisions[0]);

__sdram float s_delay_l[kDelayLineSize];
__sdram float s_delay_r[kDelayLineSize];

uint32_t s_write_index = 0u;
float s_current_delay_samples = 12000.0f;
float s_target_delay_samples = 12000.0f;
float s_delay_multiplier = 1.0f;
float s_wet = 0.35f;
float s_dry = 0.65f;

chordghost::ChordState s_chord_state;

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

static inline float read_frac(float pos, const float *buffer) {
  uint32_t base = static_cast<uint32_t>(pos);
  const float frac = pos - static_cast<float>(base);
  const float a = buffer[base & kDelayLineMask];
  const float b = buffer[(base + 1u) & kDelayLineMask];
  return a + (b - a) * frac;
}

static inline void update_delay_target() {
  float bpm = fx_get_bpmf();

  // Defensive bounds. The NTS-1 should normally supply a valid BPM.
  if (bpm < 20.0f) bpm = 20.0f;
  if (bpm > 400.0f) bpm = 400.0f;

  const float quarter_note_samples = kSampleRate * (60.0f / bpm);
  float target = quarter_note_samples * s_delay_multiplier;

  if (target < 1.0f) target = 1.0f;
  const float max_delay = static_cast<float>(kDelayLineSize - 2u);
  if (target > max_delay) target = max_delay;

  s_target_delay_samples = target;
}

static inline uint8_t normalized_to_cc7(float normalized) {
  normalized = clamp01(normalized);
  int code = static_cast<int>(normalized * 127.0f + 0.5f);
  if (code < 0) code = 0;
  if (code > 127) code = 127;
  return static_cast<uint8_t>(code);
}

} // namespace

void DELFX_INIT(uint32_t platform, uint32_t api) {
  (void)platform;
  (void)api;

  s_write_index = 0u;
  s_current_delay_samples = 12000.0f;
  s_target_delay_samples = 12000.0f;
  s_delay_multiplier = 1.0f;
  s_wet = 0.35f;
  s_dry = 0.65f;

  chordghost::decode_chord_code(0u, s_chord_state); // C major default

  for (uint32_t i = 0u; i < kDelayLineSize; ++i) {
    s_delay_l[i] = 0.0f;
    s_delay_r[i] = 0.0f;
  }
}

void DELFX_PROCESS(float *xn, uint32_t frames) {
  update_delay_target();

  float *x = xn;
  float *const end = xn + (frames * 2u);

  while (x < end) {
    const float in_l = x[0];
    const float in_r = x[1];

    // Smooth delay-time changes to reduce hard discontinuities.
    s_current_delay_samples +=
        (s_target_delay_samples - s_current_delay_samples) * kDelaySlew;

    float read_pos = static_cast<float>(s_write_index) - s_current_delay_samples;
    if (read_pos < 0.0f) {
      read_pos += static_cast<float>(kDelayLineSize);
    }

    const float delayed_l = read_frac(read_pos, s_delay_l);
    const float delayed_r = read_frac(read_pos, s_delay_r);

    // M1 deliberately keeps the feedback path harmonically neutral.
    // M3 will insert the pitch shifter here, after M2 proves chord decoding.
    const float write_l = clamp_audio(in_l + delayed_l * kFeedback);
    const float write_r = clamp_audio(in_r + delayed_r * kFeedback);

    s_delay_l[s_write_index] = write_l;
    s_delay_r[s_write_index] = write_r;

    x[0] = in_l * s_dry + delayed_l * s_wet;
    x[1] = in_r * s_dry + delayed_r * s_wet;

    s_write_index = (s_write_index + 1u) & kDelayLineMask;
    x += 2;
  }
}

void DELFX_PARAM(uint8_t index, int32_t value) {
  const float normalized = clamp01(q31_to_f32(value));

  switch (index) {
    case k_user_delfx_param_time: {
      uint32_t division = static_cast<uint32_t>(normalized * static_cast<float>(kNumDelayDivisions));
      if (division >= kNumDelayDivisions) {
        division = kNumDelayDivisions - 1u;
      }
      s_delay_multiplier = kDelayDivisions[division];
      break;
    }

    case k_user_delfx_param_depth: {
      // CHORDGHOST repurposes DEPTH / CC31 as harmonic state.
      // The decoded state is intentionally not used by the audio path until M3.
      const uint8_t code = normalized_to_cc7(normalized);
      chordghost::ChordState candidate = s_chord_state;
      if (chordghost::decode_chord_code(code, candidate)) {
        s_chord_state = candidate;
      }
      break;
    }

    case k_user_delfx_param_shift_depth:
      // Conventional NTS-1 delay MIX control.
      s_wet = normalized;
      s_dry = 1.0f - normalized;
      break;

    default:
      break;
  }
}
