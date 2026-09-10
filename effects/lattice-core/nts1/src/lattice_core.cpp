#include "usermodfx.h"

#include <stdint.h>

namespace {

static const float kSampleRate = 48000.0f;
static const uint32_t kBufferSize = 65536u;
static const uint32_t kBufferMask = kBufferSize - 1u;
static const uint32_t kMaxVoices = 16u;
static const float kParamSlew = 0.0015f;
static const float kQ15Scale = 1.0f / 32768.0f;
static const float kGuardCeiling = 0.86f;

// CORE 0.3 uses the same ~128 KB history budget as the previous float buffer,
// but stores mono capture as signed 16-bit samples. This doubles the available
// history to ~1.36 s at 48 kHz and gives the TIME maximum a useful freeze loop.
__sdram int16_t s_buffer[kBufferSize];

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
static uint32_t s_rng = 0x6D2B79F5u;
static uint32_t s_last_pan_zone = 3u;

static float s_time_target = 0.0f;
static float s_pattern_target = 0.0f;
static float s_time = 0.0f;
static float s_pattern = 0.0f;
static float s_guard_gain = 1.0f;

static bool s_freeze_active = false;
static uint32_t s_freeze_start = 0u;
static uint32_t s_freeze_samples = 0u;
static float s_freeze_phase = 0.0f;
static float s_freeze_mix = 0.0f;

static const float kPanZone[8] = {
  -0.96f, -0.73f, -0.47f, -0.18f,
   0.18f,  0.47f,  0.73f,  0.96f
};

static const float kWetNorm[kMaxVoices + 1u] = {
  0.000f,
  1.000f, 0.790f, 0.670f, 0.590f,
  0.535f, 0.495f, 0.462f, 0.435f,
  0.412f, 0.392f, 0.374f, 0.358f,
  0.344f, 0.332f, 0.321f, 0.311f
};

static inline float absf(float x) { return x < 0.0f ? -x : x; }
static inline float maxf(float a, float b) { return a > b ? a : b; }

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

static inline float soft_limit(float x) {
  const float ax = absf(x);
  if (ax <= 0.94f) return x;
  const float over = ax - 0.94f;
  float y = 0.94f + over / (1.0f + 6.0f * over);
  if (y > 0.995f) y = 0.995f;
  return x < 0.0f ? -y : y;
}

static inline float guard_pair(float &l, float &r) {
  const float peak = maxf(absf(l), absf(r));
  if (peak > kGuardCeiling) {
    const float needed = kGuardCeiling / peak;
    if (needed < s_guard_gain) s_guard_gain = needed;
  } else {
    s_guard_gain += (1.0f - s_guard_gain) * 0.00035f;
    if (s_guard_gain > 1.0f) s_guard_gain = 1.0f;
  }
  l *= s_guard_gain;
  r *= s_guard_gain;
  return peak;
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

static inline int16_t f32_to_q15_local(float x) {
  x = clampf(x, -1.0f, 0.999969f);
  return static_cast<int16_t>(x * 32768.0f);
}

static inline float read_frac_q15(float pos) {
  if (pos >= static_cast<float>(kBufferSize)) pos -= static_cast<float>(kBufferSize);
  const uint32_t whole = static_cast<uint32_t>(pos);
  const uint32_t i0 = whole & kBufferMask;
  const uint32_t i1 = (i0 + 1u) & kBufferMask;
  const float frac = pos - static_cast<float>(whole);
  const float a = static_cast<float>(s_buffer[i0]) * kQ15Scale;
  const float b = static_cast<float>(s_buffer[i1]) * kQ15Scale;
  return a + (b - a) * frac;
}

static inline uint32_t voice_count(float t) {
  // The bottom of TIME is a true disengaged region. Above it, sixteen voices
  // are enough; complexity comes from how differently they are scheduled.
  if (t < 0.035f) return 0u;
  const float x = clamp01((t - 0.035f) * (1.0f / 0.865f));
  uint32_t n = 1u + static_cast<uint32_t>(x * 15.999f);
  if (n > kMaxVoices) n = kMaxVoices;
  return n;
}

static inline float modulation_amount(float t) {
  return clamp01((t - 0.10f) * (1.0f / 0.82f));
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
    case -24: return 0.25f;
    case -19: return 0.33370996f;
    case -17: return 0.37457677f;
    case -12: return 0.5f;
    case -7:  return 0.66741993f;
    case -5:  return 0.74915354f;
    case 0:   return 1.0f;
    case 5:   return 1.33483985f;
    case 7:   return 1.49830708f;
    case 12:  return 2.0f;
    case 17:  return 2.66967971f;
    case 19:  return 2.99661415f;
    case 24:  return 4.0f;
    default:  return 1.0f;
  }
}

static inline float grain_window(float phase) {
  const float p = phase * (1.0f - phase);
  const float w = 4.0f * p;
  return w * w;
}

static inline float nominal_loop_ms(uint32_t i) {
  static const float kLoopMs[16] = {
    43.0f, 57.0f, 71.0f, 88.0f,
    109.0f, 132.0f, 158.0f, 184.0f,
    51.0f, 67.0f, 97.0f, 121.0f,
    147.0f, 176.0f, 203.0f, 229.0f
  };
  return kLoopMs[i & 15u];
}

static inline float nominal_age_ms(uint32_t i) {
  static const float kAgeMs[16] = {
    108.0f, 167.0f, 239.0f, 318.0f,
    402.0f, 493.0f, 589.0f, 691.0f,
    154.0f, 282.0f, 447.0f, 626.0f,
    781.0f, 914.0f, 1031.0f, 1147.0f
  };
  return kAgeMs[i & 15u];
}

static void schedule_wait(LoopVoice &v, float mod) {
  float bpm = fx_get_bpmf();
  if (bpm < 30.0f || bpm > 300.0f) bpm = 120.0f;
  const float beat = (60.0f * kSampleRate) / bpm;
  static const float kRhythm[8] = {
    0.0833333f, 0.125f, 0.1666667f, 0.25f,
    0.3333333f, 0.50f, 0.6666667f, 1.0f
  };
  const uint32_t choices = 3u + static_cast<uint32_t>(mod * 5.0f);
  const uint32_t pick = rng_u32() % (choices > 8u ? 8u : choices);
  float wait = beat * kRhythm[pick];
  wait *= 0.92f + rng01() * (0.16f + 0.28f * mod);
  if (wait < 48.0f) wait = 48.0f;
  if (wait > 48000.0f) wait = 48000.0f;
  v.wait_samples = static_cast<uint32_t>(wait);
}

static int modulated_interval(uint32_t pattern, uint32_t event, float mod) {
  int semi = interval_for_event(pattern, event);
  if (mod > 0.25f) {
    const float r = rng01();
    const float octave_prob = 0.08f + 0.27f * mod;
    if (r < octave_prob * 0.55f) semi += 12;
    else if (r < octave_prob) semi -= 12;
  }
  if (semi > 24) semi -= 12;
  if (semi < -24) semi += 12;
  return semi;
}

static void capture_voice(uint32_t i, uint32_t pattern, float mod) {
  LoopVoice &v = s_voice[i];

  float loop_ms = nominal_loop_ms(i);
  loop_ms *= 1.0f + rng_signed() * (0.018f + 0.17f * mod);
  loop_ms = clampf(loop_ms, 28.0f, 260.0f);

  float age_ms = nominal_age_ms(i);
  age_ms += rng_signed() * (5.0f + 125.0f * mod);
  if (age_ms < loop_ms + 24.0f) age_ms = loop_ms + 24.0f;
  age_ms = clampf(age_ms, 74.0f, 1310.0f);

  v.capture_start = (s_write - static_cast<uint32_t>(ms_to_samples(age_ms))) & kBufferMask;
  v.loop_samples = static_cast<uint32_t>(ms_to_samples(loop_ms));
  if (v.loop_samples < 64u) v.loop_samples = 64u;
  if (v.loop_samples > 12480u) v.loop_samples = 12480u;

  ++s_event_counter;
  const int semi = modulated_interval(pattern, s_event_counter, mod);
  const float ratio = ratio_for_semitones(semi);
  v.phase = rng01() * (0.02f + 0.12f * mod);
  v.phase_inc = ratio / static_cast<float>(v.loop_samples);

  const uint32_t span = 3u + static_cast<uint32_t>(mod * 7.0f);
  v.repeats_left = 2u + (rng_u32() % (span > 9u ? 9u : span));

  uint32_t zone = rng_u32() % 7u;
  if (zone >= s_last_pan_zone) ++zone;
  s_last_pan_zone = zone;
  float pan = kPanZone[zone] + rng_signed() * (0.012f + 0.055f * mod);
  pan = clampf(pan, -0.99f, 0.99f);
  v.gain_l = 0.5f * (1.0f - pan);
  v.gain_r = 0.5f * (1.0f + pan);
  v.voice_gain = 0.86f + 0.26f * rng01();
}

static void engage_freeze(void) {
  if (s_freeze_active || s_filled < 4096u) return;
  s_freeze_samples = s_filled;
  const uint32_t max_capture = 60000u;
  if (s_freeze_samples > max_capture) s_freeze_samples = max_capture;
  if (s_freeze_samples < 4096u) s_freeze_samples = 4096u;
  s_freeze_start = (s_write - s_freeze_samples) & kBufferMask;
  s_freeze_phase = 0.0f;
  s_freeze_active = true;
}

static inline float read_freeze(float phase) {
  if (s_freeze_samples < 64u) return 0.0f;
  const float p = phase * static_cast<float>(s_freeze_samples);
  float sample = read_frac_q15(static_cast<float>(s_freeze_start) + p);

  // Crossfade the final ~8 ms into the start to suppress arbitrary loop-boundary clicks.
  const uint32_t xfade_samples = s_freeze_samples > 768u ? 384u : 96u;
  const float edge = static_cast<float>(s_freeze_samples - xfade_samples);
  if (p > edge) {
    const float x = (p - edge) / static_cast<float>(xfade_samples);
    const float head = read_frac_q15(static_cast<float>(s_freeze_start) + (p - edge));
    sample = sample * (1.0f - x) + head * x;
  }
  return sample;
}

static void reset_state(void) {
  for (uint32_t i = 0u; i < kBufferSize; ++i) s_buffer[i] = 0;
  s_write = 0u;
  s_filled = 0u;
  s_event_counter = 0u;
  s_rng = 0x6D2B79F5u;
  s_last_pan_zone = 3u;
  s_time_target = s_time = 0.0f;
  s_pattern_target = s_pattern = 0.0f;
  s_guard_gain = 1.0f;
  s_freeze_active = false;
  s_freeze_start = 0u;
  s_freeze_samples = 0u;
  s_freeze_phase = 0.0f;
  s_freeze_mix = 0.0f;

  for (uint32_t i = 0u; i < kMaxVoices; ++i) {
    s_voice[i].capture_start = 0u;
    s_voice[i].loop_samples = 64u;
    s_voice[i].repeats_left = 0u;
    s_voice[i].wait_samples = static_cast<uint32_t>(ms_to_samples(static_cast<float>((i * 11u) % 71u)));
    s_voice[i].phase = 0.0f;
    s_voice[i].phase_inc = 1.0f / 64.0f;
    s_voice[i].gain_l = 0.5f;
    s_voice[i].gain_r = 0.5f;
    s_voice[i].voice_gain = 1.0f;
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
    s_time += (s_time_target - s_time) * kParamSlew;
    s_pattern += (s_pattern_target - s_pattern) * kParamSlew;

    const float in_l = main_xn[f * 2u];
    const float in_r = main_xn[f * 2u + 1u];
    const float mono = 0.5f * (in_l + in_r);

    // Hysteresis around the maximum TIME position makes freeze deliberate.
    if (!s_freeze_active && s_time_target > 0.985f) engage_freeze();
    if (s_freeze_active && s_time_target < 0.955f) {
      s_freeze_active = false;
      s_freeze_phase = 0.0f;
    }
    const float freeze_target = s_freeze_active ? 1.0f : 0.0f;
    s_freeze_mix += (freeze_target - s_freeze_mix) * 0.0012f;

    if (!s_freeze_active) {
      s_buffer[s_write] = f32_to_q15_local(mono);
      s_write = (s_write + 1u) & kBufferMask;
      if (s_filled < kBufferSize) ++s_filled;
    }

    const uint32_t voices = voice_count(s_time);
    const uint32_t pattern = pattern_index(s_pattern);
    const float mod = modulation_amount(s_time);

    float wet_l = 0.0f;
    float wet_r = 0.0f;
    uint32_t sounding = 0u;

    if (voices > 0u && s_filled > static_cast<uint32_t>(ms_to_samples(320.0f))) {
      for (uint32_t i = 0u; i < voices; ++i) {
        LoopVoice &v = s_voice[i];
        if (v.repeats_left == 0u) {
          if (v.wait_samples > 0u) {
            --v.wait_samples;
          } else {
            capture_voice(i, pattern, mod);
          }
        }

        if (v.repeats_left == 0u) continue;
        ++sounding;

        const float sample_pos = static_cast<float>(v.capture_start) +
                                 v.phase * static_cast<float>(v.loop_samples);
        const float sample = read_frac_q15(sample_pos);
        const float env = grain_window(v.phase) * v.voice_gain;
        wet_l += sample * env * v.gain_l;
        wet_r += sample * env * v.gain_r;

        v.phase += v.phase_inc;
        while (v.phase >= 1.0f) {
          v.phase -= 1.0f;
          if (v.repeats_left > 0u) --v.repeats_left;
          if (v.repeats_left == 0u) schedule_wait(v, mod);
        }
      }
    }

    // Retire voices above the current TIME population without stale tails.
    for (uint32_t i = voices; i < kMaxVoices; ++i) {
      s_voice[i].repeats_left = 0u;
    }

    float normal_l = in_l;
    float normal_r = in_r;
    if (sounding > 0u) {
      if (sounding > kMaxVoices) sounding = kMaxVoices;
      const float wet_drive = 0.90f + 0.28f * mod;
      const float wg = kWetNorm[sounding] * wet_drive;
      normal_l = in_l * 0.68f + wet_l * wg;
      normal_r = in_r * 0.68f + wet_r * wg;
    }

    float freeze_l = normal_l;
    float freeze_r = normal_r;
    if (s_freeze_active && s_freeze_samples > 64u) {
      const float base = read_freeze(s_freeze_phase);
      // A small phase offset makes the mono history occupy stereo space without
      // doubling the capture memory footprint.
      float rp = s_freeze_phase + (816.0f / static_cast<float>(s_freeze_samples));
      if (rp >= 1.0f) rp -= 1.0f;
      const float right = read_freeze(rp);
      freeze_l = base * 1.08f;
      freeze_r = right * 1.08f;
      s_freeze_phase += 1.0f / static_cast<float>(s_freeze_samples);
      if (s_freeze_phase >= 1.0f) s_freeze_phase -= 1.0f;
    }

    float out_l = normal_l * (1.0f - s_freeze_mix) + freeze_l * s_freeze_mix;
    float out_r = normal_r * (1.0f - s_freeze_mix) + freeze_r * s_freeze_mix;
    guard_pair(out_l, out_r);
    main_yn[f * 2u] = soft_limit(out_l);
    main_yn[f * 2u + 1u] = soft_limit(out_r);
  }
}

void MODFX_SUSPEND(void) { reset_state(); }
void MODFX_RESUME(void) { reset_state(); }

void MODFX_PARAM(uint8_t index, int32_t value) {
  const float normalized = clamp01(q31_to_f32(value));
  if (index == k_user_modfx_param_time) s_time_target = normalized;
  else if (index == k_user_modfx_param_depth) s_pattern_target = normalized;
}
