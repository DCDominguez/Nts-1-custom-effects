#include "usermodfx.h"

#include <stdint.h>

namespace {

static const float kSampleRate = 48000.0f;
static const uint32_t kBufferSize = 32768u;
static const uint32_t kBufferMask = kBufferSize - 1u;
static const uint32_t kMaxVoices = 32u;
static const uint32_t kMaxConcurrent = 16u;
static const float kParamSlew = 0.0015f;

// One shared recent-audio history. The 32 voices are an ecosystem/pool, not
// 32 permanently sounding loopers. At high ACTIVITY only up to 16 are audible
// at once; the rest wait, rotate in, and recapture with decorrelated geometry.
__sdram float s_buffer[kBufferSize];

struct LoopVoice {
  uint32_t capture_start;
  uint32_t loop_samples;
  uint32_t repeats_left;
  uint32_t wait_samples;
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
static uint32_t s_active_now = 0u;
static uint32_t s_rng = 0x6D2B79F5u;
static uint32_t s_last_pan_zone = 3u;

static float s_activity_target = 0.0f;
static float s_pattern_target = 0.0f;
static float s_activity = 0.0f;
static float s_pattern = 0.0f;

// Normalization is based on actually sounding voices, not the 32-member pool.
static const float kWetGain[kMaxConcurrent + 1u] = {
  0.000f,
  0.920f, 0.690f, 0.580f, 0.510f,
  0.460f, 0.425f, 0.397f, 0.375f,
  0.355f, 0.338f, 0.323f, 0.310f,
  0.298f, 0.287f, 0.277f, 0.268f
};

static const float kPanZone[8] = {
  -0.94f, -0.72f, -0.48f, -0.20f,
   0.20f,  0.48f,  0.72f,  0.94f
};

static inline float absf(float x) { return x < 0.0f ? -x : x; }

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

// Gentle safety curve. Unlike the 0.1-2 hard clamp, this does not turn a dense
// correlated sum into a square-ish solid tone as soon as it crosses full scale.
static inline float soft_limit(float x) {
  const float ax = absf(x);
  if (ax <= 0.85f) return x;
  float y = 0.85f + (ax - 0.85f) / (1.0f + 3.0f * (ax - 0.85f));
  if (y > 0.995f) y = 0.995f;
  return x < 0.0f ? -y : y;
}

static inline float ms_to_samples(float ms) {
  return ms * (kSampleRate * 0.001f);
}

static inline uint32_t rng_u32(void) {
  uint32_t x = s_rng;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  s_rng = x;
  return x;
}

static inline float rng01(void) {
  return static_cast<float>(rng_u32() & 0x00FFFFFFu) * (1.0f / 16777215.0f);
}

static inline float rng_signed(void) {
  return rng01() * 2.0f - 1.0f;
}

static inline float read_frac(const float *buffer, float pos) {
  if (pos >= static_cast<float>(kBufferSize)) pos -= static_cast<float>(kBufferSize);
  const uint32_t whole = static_cast<uint32_t>(pos);
  const uint32_t i0 = whole & kBufferMask;
  const uint32_t i1 = (i0 + 1u) & kBufferMask;
  const float frac = pos - static_cast<float>(whole);
  return buffer[i0] + (buffer[i1] - buffer[i0]) * frac;
}

static inline uint32_t pool_count(float activity) {
  if (activity < 0.015f) return 0u;
  uint32_t n = 1u + static_cast<uint32_t>(activity * 31.999f);
  if (n > kMaxVoices) n = kMaxVoices;
  return n;
}

static inline uint32_t concurrent_limit(uint32_t pool) {
  if (pool <= 8u) return pool;
  // 8 -> 8, 16 -> 11, 24 -> 14, 32 -> 16.
  uint32_t n = 6u + pool / 3u;
  if (n > kMaxConcurrent) n = kMaxConcurrent;
  if (n > pool) n = pool;
  return n;
}

static inline float chaos_amount(float activity) {
  // Preserve the recognizable low/mid loop behavior. Random geometry ramps in
  // mainly through the upper part of the ACTIVITY control.
  return clamp01((activity - 0.28f) * (1.0f / 0.72f));
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
  const float p = phase * (1.0f - phase);
  const float w = 4.0f * p;
  return w * w;
}

static inline float nominal_loop_ms(uint32_t i) {
  // A deliberately non-repeating geometry across the first 32 slots.
  return 42.0f + static_cast<float>((i * 11u) % 23u) * 5.1f +
         static_cast<float>(i % 3u) * 3.7f;
}

static inline float nominal_age_ms(uint32_t i) {
  return 118.0f + static_cast<float>(i) * 14.7f;
}

static inline float hierarchy_gain(uint32_t i) {
  if (i < 16u) return 1.0f;
  if (i < 24u) return 0.82f;
  return 0.68f;
}

static void schedule_wait(LoopVoice &v, float chaos) {
  // At high activity the population constantly exchanges members instead of
  // consolidating into one permanently periodic mass.
  const float wait_ms = 2.0f + rng01() * (18.0f + 135.0f * chaos);
  v.wait_samples = static_cast<uint32_t>(ms_to_samples(wait_ms));
}

static void capture_voice(uint32_t i, uint32_t pattern, float chaos) {
  LoopVoice &v = s_voice[i];

  float age_ms = nominal_age_ms(i);
  age_ms += rng_signed() * (4.0f + 58.0f * chaos);
  age_ms = clampf(age_ms, 88.0f, 630.0f);

  float loop_ms = nominal_loop_ms(i);
  const float length_jitter = 0.015f + 0.155f * chaos;
  loop_ms *= 1.0f + rng_signed() * length_jitter;
  loop_ms = clampf(loop_ms, 30.0f, 190.0f);

  v.capture_start = (s_write - static_cast<uint32_t>(ms_to_samples(age_ms))) & kBufferMask;
  v.loop_samples = static_cast<uint32_t>(ms_to_samples(loop_ms));
  if (v.loop_samples < 64u) v.loop_samples = 64u;
  if (v.loop_samples > 10000u) v.loop_samples = 10000u;

  ++s_event_counter;
  const float ratio = ratio_for_semitones(interval_for_event(pattern, s_event_counter));
  v.phase = 0.0f;
  v.phase_inc = ratio / static_cast<float>(v.loop_samples);

  if (chaos < 0.20f) {
    v.repeats_left = 5u + (i & 3u);
  } else {
    v.repeats_left = 3u + (rng_u32() % 7u); // 3..9
  }

  // Randomized-but-distributed stereo placement. Choose one of the other seven
  // zones, then apply a small local offset; this prevents center pile-up.
  uint32_t zone = rng_u32() % 7u;
  if (zone >= s_last_pan_zone) ++zone;
  s_last_pan_zone = zone;
  float pan = kPanZone[zone] + rng_signed() * (0.015f + 0.045f * chaos);
  pan = clampf(pan, -0.98f, 0.98f);
  v.gain_l = 0.5f * (1.0f - pan);
  v.gain_r = 0.5f * (1.0f + pan);

  v.voice_gain = hierarchy_gain(i) * (0.84f + 0.16f * rng01());
  v.wait_samples = 0u;
  ++s_active_now;
}

static void reset_state(void) {
  for (uint32_t i = 0u; i < kBufferSize; ++i) s_buffer[i] = 0.0f;
  s_write = 0u;
  s_filled = 0u;
  s_event_counter = 0u;
  s_active_now = 0u;
  s_rng = 0x6D2B79F5u;
  s_last_pan_zone = 3u;
  s_activity_target = s_activity = 0.0f;
  s_pattern_target = s_pattern = 0.0f;

  for (uint32_t i = 0u; i < kMaxVoices; ++i) {
    s_voice[i].capture_start = 0u;
    s_voice[i].loop_samples = 64u;
    s_voice[i].repeats_left = 0u;
    s_voice[i].wait_samples = static_cast<uint32_t>(ms_to_samples(static_cast<float>((i * 7u) % 43u)));
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
    s_activity += (s_activity_target - s_activity) * kParamSlew;
    s_pattern += (s_pattern_target - s_pattern) * kParamSlew;

    const float in_l = main_xn[f * 2u];
    const float in_r = main_xn[f * 2u + 1u];
    s_buffer[s_write] = 0.5f * (in_l + in_r);
    if (s_filled < kBufferSize) ++s_filled;

    const uint32_t pool = pool_count(s_activity);
    const uint32_t limit = concurrent_limit(pool);
    const uint32_t pattern = pattern_index(s_pattern);
    const float chaos = chaos_amount(s_activity);

    // If ACTIVITY is turned down, retire voices outside the new pool cleanly at
    // the control boundary rather than letting stale high-density voices linger.
    for (uint32_t i = pool; i < kMaxVoices; ++i) {
      if (s_voice[i].repeats_left > 0u) {
        s_voice[i].repeats_left = 0u;
        if (s_active_now > 0u) --s_active_now;
      }
      s_voice[i].wait_samples = 0u;
    }

    float wet_l = 0.0f;
    float wet_r = 0.0f;
    uint32_t sounding = 0u;

    if (pool > 0u && s_filled > static_cast<uint32_t>(ms_to_samples(650.0f))) {
      for (uint32_t i = 0u; i < pool; ++i) {
        LoopVoice &v = s_voice[i];

        if (v.repeats_left == 0u) {
          if (v.wait_samples > 0u) {
            --v.wait_samples;
          } else if (s_active_now < limit) {
            capture_voice(i, pattern, chaos);
          }
        }

        if (v.repeats_left == 0u) continue;
        ++sounding;

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
          if (v.repeats_left == 0u) {
            if (s_active_now > 0u) --s_active_now;
            schedule_wait(v, chaos);
          }
        }
      }
    }

    if (sounding == 0u) {
      main_yn[f * 2u] = in_l;
      main_yn[f * 2u + 1u] = in_r;
    } else {
      if (sounding > kMaxConcurrent) sounding = kMaxConcurrent;
      const float wg = kWetGain[sounding];
      main_yn[f * 2u] = soft_limit(in_l * 0.60f + wet_l * wg);
      main_yn[f * 2u + 1u] = soft_limit(in_r * 0.60f + wet_r * wg);
    }

    s_write = (s_write + 1u) & kBufferMask;
  }
}

void MODFX_SUSPEND(void) { reset_state(); }
void MODFX_RESUME(void) { reset_state(); }

void MODFX_PARAM(uint8_t index, int32_t value) {
  const float normalized = clamp01(q31_to_f32(value));
  if (index == k_user_modfx_param_time) s_activity_target = normalized;
  else if (index == k_user_modfx_param_depth) s_pattern_target = normalized;
}
