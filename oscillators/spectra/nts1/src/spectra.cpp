#include "userosc.h"

#include <math.h>
#include <stdint.h>

namespace {

static const uint32_t kMaxVoices = 4u;
static const float kLn2Over1200 = 0.0005776226504666211f;
static const float kMaxSpreadCents = 20.0f;
static const float kMaxDriftCents = 8.0f;
static const float kMaxChaosCents = 4.0f;
static const float kMaxChaosLevel = 0.12f;
static const float kMaxPhaseIncrement = 0.45f;
static const float kDriftSlew = 0.0008f;

static const float kIntervalModes[8][kMaxVoices] = {
  { 0.0f,  0.0f,  0.0f,  0.0f },
  { 0.0f,  7.0f, 12.0f,-12.0f },
  { 0.0f, 12.0f,-12.0f, 24.0f },
  { 0.0f,  7.0f,  4.0f, 12.0f },
  { 0.0f,  7.0f,  3.0f, 12.0f },
  { 0.0f,  7.0f,  5.0f, 12.0f },
  { 0.0f,  5.0f, 10.0f, 15.0f },
  { 0.0f,  1.0f,  7.0f, 13.0f }
};

static const float kSpreadCoeffs[4][kMaxVoices] = {
  { 0.0f,   0.0f,   0.0f, 0.0f },
  {-1.0f,  +1.0f,   0.0f, 0.0f },
  {-1.0f,   0.0f,  +1.0f, 0.0f },
  {-1.0f,  -0.333f, +0.333f, +1.0f }
};

// Two incommensurate low-rate components per voice keep the drift independent
// without resorting to audio-rate randomness.
static const float kDriftHzA[kMaxVoices] = {0.11f, 0.17f, 0.23f, 0.31f};
static const float kDriftHzB[kMaxVoices] = {0.071f, 0.113f, 0.151f, 0.197f};
static const float kDriftPhaseA[kMaxVoices] = {0.00f, 0.23f, 0.51f, 0.79f};
static const float kDriftPhaseB[kMaxVoices] = {0.41f, 0.67f, 0.12f, 0.86f};

struct SpectraState {
  uint8_t voices;
  uint8_t harm_mode;

  float spread;
  float drift;
  float drift_target;
  float motion;
  float motion_target;
  float chaos;
  float shape;
  float alt;

  float phase[kMaxVoices];
  float drift_phase_a[kMaxVoices];
  float drift_phase_b[kMaxVoices];

  float harmonic_ratio[kMaxVoices];
  float spread_ratio[kMaxVoices];
  float chaos_ratio[kMaxVoices];
  float level[kMaxVoices];
};

SpectraState s;

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
  while (x >= 1.0f) x -= 1.0f;
  while (x < 0.0f) x += 1.0f;
  return x;
}

static inline float cents_to_ratio(float cents) {
  return powf(2.0f, cents / 1200.0f);
}

static void update_harmonic_ratios() {
  const uint8_t mode = (s.harm_mode < 8u) ? s.harm_mode : 0u;
  for (uint32_t i = 0u; i < kMaxVoices; ++i) {
    const float semitones = kIntervalModes[mode][i] * s.alt;
    s.harmonic_ratio[i] = powf(2.0f, semitones / 12.0f);
  }
}

static void update_spread_ratios() {
  uint32_t count_index = 0u;
  if (s.voices > 1u) count_index = static_cast<uint32_t>(s.voices - 1u);
  if (count_index > 3u) count_index = 3u;

  for (uint32_t i = 0u; i < kMaxVoices; ++i) {
    const float cents = kSpreadCoeffs[count_index][i] * kMaxSpreadCents * s.spread;
    s.spread_ratio[i] = cents_to_ratio(cents);
  }
}

static void reset_note_variation() {
  for (uint32_t i = 0u; i < kMaxVoices; ++i) {
    const float random_phase = 0.5f * (osc_white() + 1.0f);
    s.phase[i] = wrap01(random_phase * s.chaos);
    s.drift_phase_a[i] = kDriftPhaseA[i];
    s.drift_phase_b[i] = kDriftPhaseB[i];

    const float chaos_cents = osc_white() * kMaxChaosCents * s.chaos;
    s.chaos_ratio[i] = cents_to_ratio(chaos_cents);

    const float level_delta = osc_white() * kMaxChaosLevel * s.chaos;
    s.level[i] = clampf(1.0f + level_delta, 0.70f, 1.30f);
  }
}

static inline float triangle(float phase) {
  return 1.0f - 4.0f * fabsf(phase - 0.5f);
}

static inline float render_wave(float phase, float shape, float note_for_tables) {
  shape = clamp01(shape);
  note_for_tables = clampf(note_for_tables, 0.0f, 151.0f);

  const float sine = osc_sinf(phase);
  const float tri = triangle(phase);
  const float saw = osc_bl2_sawf(phase, osc_bl_saw_idx(note_for_tables));
  const float square = osc_bl2_sqrf(phase, osc_bl_sqr_idx(note_for_tables));

  if (shape < (1.0f / 3.0f)) {
    const float t = shape * 3.0f;
    return sine + (tri - sine) * t;
  }
  if (shape < (2.0f / 3.0f)) {
    const float t = (shape - (1.0f / 3.0f)) * 3.0f;
    return tri + (saw - tri) * t;
  }
  const float t = (shape - (2.0f / 3.0f)) * 3.0f;
  return saw + (square - saw) * t;
}

static void set_defaults() {
  s.voices = 4u;
  s.harm_mode = 0u;
  s.spread = 0.25f;
  s.drift = s.drift_target = 0.15f;
  s.motion = s.motion_target = 0.25f;
  s.chaos = 0.0f;
  s.shape = 0.45f;
  s.alt = 0.0f;

  for (uint32_t i = 0u; i < kMaxVoices; ++i) {
    s.phase[i] = 0.0f;
    s.drift_phase_a[i] = kDriftPhaseA[i];
    s.drift_phase_b[i] = kDriftPhaseB[i];
    s.harmonic_ratio[i] = 1.0f;
    s.spread_ratio[i] = 1.0f;
    s.chaos_ratio[i] = 1.0f;
    s.level[i] = 1.0f;
  }

  update_harmonic_ratios();
  update_spread_ratios();
  reset_note_variation();
}

} // namespace

void OSC_INIT(uint32_t platform, uint32_t api) {
  (void)platform;
  (void)api;
  set_defaults();
}

void OSC_CYCLE(const user_osc_param_t *const params, int32_t *yn, const uint32_t frames) {
  const uint8_t note = static_cast<uint8_t>(params->pitch >> 8);
  const uint8_t fine = static_cast<uint8_t>(params->pitch & 0xFFu);
  const float base_increment = osc_w0f_for_note(note, fine);
  const float shape_lfo = q31_to_f32(params->shape_lfo);
  const float shape = clamp01(s.shape + shape_lfo);

  float increments[kMaxVoices];
  float table_notes[kMaxVoices];
  for (uint32_t i = 0u; i < kMaxVoices; ++i) {
    float inc = base_increment * s.harmonic_ratio[i] * s.spread_ratio[i] * s.chaos_ratio[i];
    increments[i] = clampf(inc, 0.0f, kMaxPhaseIncrement);
    const float interval = kIntervalModes[s.harm_mode][i] * s.alt;
    table_notes[i] = clampf(static_cast<float>(note) + interval, 0.0f, 151.0f);
  }

  q31_t *out = reinterpret_cast<q31_t *>(yn);

  for (uint32_t frame = 0u; frame < frames; ++frame) {
    s.drift += (s.drift_target - s.drift) * kDriftSlew;
    s.motion += (s.motion_target - s.motion) * kDriftSlew;
    const float rate_scale = 0.20f + 1.80f * s.motion;

    float sum = 0.0f;
    float weight_sum = 0.0f;
    const uint32_t active = (s.voices < 1u) ? 1u : (s.voices > kMaxVoices ? kMaxVoices : s.voices);

    for (uint32_t i = 0u; i < active; ++i) {
      const float drift_wave = 0.72f * osc_sinf(s.drift_phase_a[i]) +
                               0.28f * osc_sinf(s.drift_phase_b[i]);
      const float drift_cents = drift_wave * kMaxDriftCents * s.drift;
      const float drift_ratio = 1.0f + drift_cents * kLn2Over1200;
      const float inc = clampf(increments[i] * drift_ratio, 0.0f, kMaxPhaseIncrement);

      const float voice = render_wave(s.phase[i], shape, table_notes[i]);
      sum += voice * s.level[i];
      weight_sum += s.level[i];

      s.phase[i] = wrap01(s.phase[i] + inc);
      s.drift_phase_a[i] = wrap01(s.drift_phase_a[i] + kDriftHzA[i] * rate_scale * k_samplerate_recipf);
      s.drift_phase_b[i] = wrap01(s.drift_phase_b[i] + kDriftHzB[i] * rate_scale * k_samplerate_recipf);
    }

    float signal = (weight_sum > 0.0001f) ? (sum / weight_sum) : 0.0f;
    signal *= 0.90f;
    signal = osc_softclipf(0.10f, signal);
    out[frame] = f32_to_q31(signal);
  }
}

void OSC_NOTEON(const user_osc_param_t *const params) {
  (void)params;
  reset_note_variation();
}

void OSC_NOTEOFF(const user_osc_param_t *const params) { (void)params; }

void OSC_PARAM(uint16_t index, uint16_t value) {
  switch (index) {
    case k_user_osc_param_id1:
      s.voices = static_cast<uint8_t>((value > 3u ? 3u : value) + 1u);
      update_spread_ratios();
      break;
    case k_user_osc_param_id2:
      s.spread = clamp01(static_cast<float>(value) * 0.01f);
      update_spread_ratios();
      break;
    case k_user_osc_param_id3:
      s.drift_target = clamp01(static_cast<float>(value) * 0.01f);
      break;
    case k_user_osc_param_id4:
      s.harm_mode = static_cast<uint8_t>(value > 7u ? 7u : value);
      update_harmonic_ratios();
      break;
    case k_user_osc_param_id5:
      s.motion_target = clamp01(static_cast<float>(value) * 0.01f);
      break;
    case k_user_osc_param_id6:
      s.chaos = clamp01(static_cast<float>(value) * 0.01f);
      break;
    case k_user_osc_param_shape:
      s.shape = clamp01(param_val_to_f32(value));
      break;
    case k_user_osc_param_shiftshape:
      s.alt = clamp01(param_val_to_f32(value));
      update_harmonic_ratios();
      break;
    default:
      break;
  }
}
