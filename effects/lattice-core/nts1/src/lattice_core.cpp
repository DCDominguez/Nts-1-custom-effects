#include "usermodfx.h"

#include <stdint.h>

namespace {

static const float kSampleRate = 48000.0f;
static const uint32_t kBufferSize = 32768u;
static const uint32_t kBufferMask = kBufferSize - 1u;
static const uint32_t kMaxVoices = 32u;
static const float kParamSlew = 0.0015f;

// One shared source-history buffer. Voice count scales CPU/state, not capture
// memory: all 32 loopers read independently from the same history.
__sdram float s_buffer[kBufferSize];

struct LoopVoice {
  uint32_t capture_start;
  uint32_t loop_samples;
  uint32_t repeats_left;
  float phase;
  float phase_inc;
  float gain_l;
  float gain_r;
  float voice_gain;
};

static LoopVoice s_voice[kMaxVoices];
static uint32_t s_write = 0u;
static uint32_t s_filled = 0u;
static uint32_t s_event_counter = 0u;

static float s_loops_target = 0.0f;
static float s_pattern_target = 0.0f;
static float s_loops = 0.0f;
static float s_pattern = 0.0f;

// Deliberately assertive normalization for the 32-voice stress build. Voices
// 17..24 and 25..32 are individually attenuated as secondary/ghost layers.
static const float kWetGain[kMaxVoices + 1u] = {
  0.000f,
  0.920f, 0.720f, 0.620f, 0.560f, 0.510f, 0.470f, 0.440f, 0.420f,
  0.400f, 0.385f, 0.370f, 0.356f, 0.344f, 0.333f, 0.323f, 0.314f,
  0.305f, 0.297f, 0.289f, 0.282f, 0.275f, 0.268f, 0.262f, 0.256f,
  0.250f, 0.244f, 0.239f, 0.234f, 0.229f, 0.224f, 0.219f, 0.214f
};

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

static inline float ms_to_samples(float ms) {
  return ms * (kSampleRate * 0.001f);
}

// sample_pos is capture_start + [0, loop_samples), so it can cross the ring
// boundary at most once. Avoid per-sample while loops in the 32-voice path.
static inline float read_frac(const float *buffer, float pos) {
  if (pos >= static_cast<float>(kBufferSize)) pos -= static_cast<float>(kBufferSize);
  const uint32_t whole = static_cast<uint32_t>(pos);
  const uint32_t i0 = whole & kBufferMask;
  const uint32_t i1 = (i0 + 1u) & kBufferMask;
  const float frac = pos - static_cast<float>(whole);
  return buffer[i0] + (buffer[i1] - buffer[i0]) * frac;
}

static inline uint32_t active_voice_count(float loops) {
  if (loops < 0.015f) return 0u;
  uint32_t n = 1u + static_cast<uint32_t>(loops * 31.999f);
  if (n > kMaxVoices) n = kMaxVoices;
  return n;
}

static inline uint32_t pattern_index(float pattern) {
  uint32_t p = static_cast<uint32_t>(pattern * 7.999f);
  if (p > 7u) p = 7u;
  return p;
}

static inline int interval_for_event(uint32_t pattern, uint32_t event) {
  static const int orbit[8]   = { 0, 7, 0, 12, 7, 0, -5, 12 };
  static const int glass[8]   = { 12, 0, 7, 0, -12, 7, 12, -5 };
  static const int counter[8] = { 0, 7, -5, 12, -12, 5, -7, 0 };
  static const int human[12]  = { 0, 7, 0, -5, 12, 7, 0, -12, 5, 0, 12, -7 };

  switch (pattern) {
    case 0u: return 0;
    case 1u: return ((event % 3u) == 0u) ? 7 : 0;
    case 2u:
      if ((event % 5u) == 0u) return 12;
      if ((event % 3u) == 0u) return 7;
      return 0;
    case 3u:
      if ((event % 5u) == 0u) return -12;
      if ((event % 3u) == 0u) return -5;
      return 0;
    case 4u: return orbit[(event - 1u) & 7u];
    case 5u: return glass[(event - 1u) & 7u];
    case 6u: return counter[(event - 1u) & 7u];
    default: return human[(event - 1u) % 12u];
  }
}

static inline float ratio_for_semitones(int semitones) {
  switch (semitones) {
    case -12: return 0.5f;
    case -7:  return 0.66741993f;
    case -5:  return 0.74915354f;
    case 5:   return 1.33483985f;
    case 7:   return 1.49830708f;
    case 12:  return 2.0f;
    default:  return 1.0f;
  }
}

// Cheap quartic grain window for the 32-voice build. The previous sine window
// required a transcendental call per active voice per sample. This keeps both
// ends at zero and puts the CPU budget into more simultaneous loopers instead.
static inline float grain_window(float phase) {
  const float p = phase * (1.0f - phase);
  const float w = 4.0f * p;
  return w * w;
}

static inline uint32_t repeat_count_for_voice(uint32_t i) {
  // Primary voices persist longest; later voices behave more like ghosts.
  if (i < 16u) return 5u + (i & 3u);   // 5..8
  if (i < 24u) return 4u + (i & 3u);   // 4..7
  return 3u + (i & 3u);                // 3..6
}

static inline float loop_ms_for_voice(uint32_t i) {
  // 38..~157 ms, distributed so the 32 heads do not collapse into one loop.
  return 38.0f + static_cast<float>(i & 15u) * 7.6f + static_cast<float>(i >> 4u) * 4.2f;
}

static inline float age_ms_for_voice(uint32_t i) {
  // 132..~582 ms, within the ~683 ms shared history at the implementation rate.
  return 132.0f + static_cast<float>(i) * 14.5f;
}

static inline float pan_for_voice(uint32_t i) {
  // Deterministic permutation across the stereo field rather than simple L/R alternation.
  const uint32_t slot = (i * 13u) & 31u;
  return -0.95f + 1.90f * (static_cast<float>(slot) / 31.0f);
}

static inline float hierarchy_gain(uint32_t i) {
  if (i < 16u) return 1.0f;
  if (i < 24u) return 0.74f;
  return 0.56f;
}

static void capture_voice(uint32_t i, uint32_t pattern) {
  LoopVoice &v = s_voice[i];
  const uint32_t age = static_cast<uint32_t>(ms_to_samples(age_ms_for_voice(i)));
  v.capture_start = (s_write - age) & kBufferMask;
  v.loop_samples = static_cast<uint32_t>(ms_to_samples(loop_ms_for_voice(i)));
  if (v.loop_samples < 64u) v.loop_samples = 64u;
  if (v.loop_samples > 10000u) v.loop_samples = 10000u;

  ++s_event_counter;
  const float ratio = ratio_for_semitones(interval_for_event(pattern, s_event_counter));
  v.phase = 0.0f;
  v.phase_inc = ratio / static_cast<float>(v.loop_samples);
  v.repeats_left = repeat_count_for_voice(i);
  v.voice_gain = hierarchy_gain(i);

  const float pan = pan_for_voice(i);
  v.gain_l = 0.5f * (1.0f - pan);
  v.gain_r = 0.5f * (1.0f + pan);
}

static void reset_state(void) {
  for (uint32_t i = 0u; i < kBufferSize; ++i) s_buffer[i] = 0.0f;
  s_write = 0u;
  s_filled = 0u;
  s_event_counter = 0u;
  s_loops_target = s_loops = 0.0f;
  s_pattern_target = s_pattern = 0.0f;

  for (uint32_t i = 0u; i < kMaxVoices; ++i) {
    s_voice[i].capture_start = 0u;
    s_voice[i].loop_samples = 64u;
    s_voice[i].repeats_left = 0u;
    s_voice[i].phase = 0.0f;
    s_voice[i].phase_inc = 1.0f / 64.0f;
    s_voice[i].gain_l = 0.5f;
    s_voice[i].gain_r = 0.5f;
    s_voice[i].voice_gain = hierarchy_gain(i);
  }
}

} // namespace

void MODFX_INIT(uint32_t platform, uint32_t api) {
  (void)platform;
  (void)api;
  reset_state();
}

void MODFX_PROCESS(const float *main_xn, float *main_yn,
                   const float *sub_xn, float *sub_yn,
                   uint32_t frames) {
  (void)sub_xn;
  (void)sub_yn;

  for (uint32_t f = 0u; f < frames; ++f) {
    s_loops += (s_loops_target - s_loops) * kParamSlew;
    s_pattern += (s_pattern_target - s_pattern) * kParamSlew;

    const float in_l = main_xn[f * 2u];
    const float in_r = main_xn[f * 2u + 1u];
    s_buffer[s_write] = 0.5f * (in_l + in_r);
    if (s_filled < kBufferSize) ++s_filled;

    const uint32_t active = active_voice_count(s_loops);
    const uint32_t pattern = pattern_index(s_pattern);
    float wet_l = 0.0f;
    float wet_r = 0.0f;

    // Wait until even the oldest 32nd voice has valid history.
    if (active > 0u && s_filled > static_cast<uint32_t>(ms_to_samples(590.0f))) {
      for (uint32_t i = 0u; i < active; ++i) {
        LoopVoice &v = s_voice[i];
        if (v.repeats_left == 0u) capture_voice(i, pattern);

        const float sample_pos = static_cast<float>(v.capture_start) +
                                 v.phase * static_cast<float>(v.loop_samples);
        const float mono = read_frac(s_buffer, sample_pos);
        const float env = grain_window(v.phase) * v.voice_gain;
        wet_l += mono * env * v.gain_l;
        wet_r += mono * env * v.gain_r;

        v.phase += v.phase_inc;
        if (v.phase >= 1.0f) {
          v.phase -= 1.0f;
          if (v.repeats_left > 0u) --v.repeats_left;
          if (v.repeats_left == 0u) capture_voice(i, pattern);
        }
      }
    }

    if (active == 0u) {
      main_yn[f * 2u] = in_l;
      main_yn[f * 2u + 1u] = in_r;
    } else {
      const float wg = kWetGain[active];
      main_yn[f * 2u] = clamp_audio(in_l * 0.52f + wet_l * wg);
      main_yn[f * 2u + 1u] = clamp_audio(in_r * 0.52f + wet_r * wg);
    }

    s_write = (s_write + 1u) & kBufferMask;
  }
}

void MODFX_SUSPEND(void) { reset_state(); }
void MODFX_RESUME(void) { reset_state(); }

void MODFX_PARAM(uint8_t index, int32_t value) {
  const float normalized = clamp01(q31_to_f32(value));
  if (index == k_user_modfx_param_time) s_loops_target = normalized;
  else if (index == k_user_modfx_param_depth) s_pattern_target = normalized;
}
