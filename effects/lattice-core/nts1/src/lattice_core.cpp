#include "usermodfx.h"

#include <stdint.h>

namespace {

static const float kSampleRate = 48000.0f;
static const uint32_t kBufferSize = 32768u;
static const uint32_t kBufferMask = kBufferSize - 1u;
static const uint32_t kMaxVoices = 8u;
static const float kParamSlew = 0.0015f;

__sdram float s_buffer_l[kBufferSize];
__sdram float s_buffer_r[kBufferSize];

struct LoopVoice {
  uint32_t capture_start;
  uint32_t loop_samples;
  uint32_t repeats_left;
  float phase;
  float ratio;
  float pan;
};

static LoopVoice s_voice[kMaxVoices];
static uint32_t s_write = 0u;
static uint32_t s_filled = 0u;
static uint32_t s_event_counter = 0u;

static float s_loops_target = 0.0f;
static float s_pattern_target = 0.0f;
static float s_loops = 0.0f;
static float s_pattern = 0.0f;

static const float kLoopMs[kMaxVoices] = {
  46.0f, 59.0f, 73.0f, 91.0f, 112.0f, 136.0f, 161.0f, 187.0f
};

static const float kAgeMs[kMaxVoices] = {
  180.0f, 214.0f, 248.0f, 286.0f, 326.0f, 360.0f, 398.0f, 432.0f
};

static const float kPan[kMaxVoices] = {
  -0.86f, 0.64f, -0.31f, 0.88f, 0.18f, -0.67f, 0.43f, -0.08f
};

static const float kWetGain[9] = {
  0.0f, 0.43f, 0.32f, 0.27f, 0.235f, 0.215f, 0.198f, 0.185f, 0.175f
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

static inline float ms_to_samples(float ms) { return ms * (kSampleRate * 0.001f); }

static inline float read_frac(const float *buffer, float pos) {
  while (pos < 0.0f) pos += static_cast<float>(kBufferSize);
  while (pos >= static_cast<float>(kBufferSize)) pos -= static_cast<float>(kBufferSize);
  const uint32_t i0 = static_cast<uint32_t>(pos) & kBufferMask;
  const uint32_t i1 = (i0 + 1u) & kBufferMask;
  const float frac = pos - static_cast<float>(static_cast<uint32_t>(pos));
  return buffer[i0] + (buffer[i1] - buffer[i0]) * frac;
}

static inline void pan_gains(float pan, float &gl, float &gr) {
  if (pan < -1.0f) pan = -1.0f;
  if (pan > 1.0f) pan = 1.0f;
  gl = 0.5f * (1.0f - pan);
  gr = 0.5f * (1.0f + pan);
}

static inline uint32_t active_voice_count(float loops) {
  if (loops < 0.015f) return 0u;
  uint32_t n = 1u + static_cast<uint32_t>(loops * 7.999f);
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

static inline float grain_window(float phase) {
  const float s = fx_sinf(0.5f * phase);
  return s * s;
}

static inline uint32_t repeat_count_for_voice(uint32_t i) {
  return 2u + (i % 3u);
}

static void capture_voice(uint32_t i, uint32_t pattern) {
  LoopVoice &v = s_voice[i];
  const uint32_t age = static_cast<uint32_t>(ms_to_samples(kAgeMs[i]));
  v.capture_start = (s_write - age) & kBufferMask;
  v.loop_samples = static_cast<uint32_t>(ms_to_samples(kLoopMs[i]));
  if (v.loop_samples < 64u) v.loop_samples = 64u;
  if (v.loop_samples > 10000u) v.loop_samples = 10000u;
  v.phase = 0.0f;
  v.pan = kPan[i];
  v.repeats_left = repeat_count_for_voice(i);
  ++s_event_counter;
  v.ratio = ratio_for_semitones(interval_for_event(pattern, s_event_counter));
}

static void reset_state(void) {
  for (uint32_t i = 0u; i < kBufferSize; ++i) {
    s_buffer_l[i] = 0.0f;
    s_buffer_r[i] = 0.0f;
  }
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
    s_voice[i].ratio = 1.0f;
    s_voice[i].pan = kPan[i];
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
    s_buffer_l[s_write] = in_l;
    s_buffer_r[s_write] = in_r;
    if (s_filled < kBufferSize) ++s_filled;

    const uint32_t active = active_voice_count(s_loops);
    const uint32_t pattern = pattern_index(s_pattern);
    float wet_l = 0.0f;
    float wet_r = 0.0f;

    if (active > 0u && s_filled > static_cast<uint32_t>(ms_to_samples(460.0f))) {
      for (uint32_t i = 0u; i < active; ++i) {
        LoopVoice &v = s_voice[i];
        if (v.repeats_left == 0u) capture_voice(i, pattern);

        const float sample_pos = static_cast<float>(v.capture_start) + v.phase * static_cast<float>(v.loop_samples);
        const float sl = read_frac(s_buffer_l, sample_pos);
        const float sr = read_frac(s_buffer_r, sample_pos);
        const float mono = 0.5f * (sl + sr);
        const float env = grain_window(v.phase);
        float gl = 0.5f, gr = 0.5f;
        pan_gains(v.pan, gl, gr);
        wet_l += mono * env * gl;
        wet_r += mono * env * gr;

        v.phase += v.ratio / static_cast<float>(v.loop_samples);
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
      main_yn[f * 2u] = clamp_audio(in_l * 0.72f + wet_l * wg);
      main_yn[f * 2u + 1u] = clamp_audio(in_r * 0.72f + wet_r * wg);
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
