#include "userdelfx.h"

#include <stdint.h>

namespace {

static const float kSampleRate = 48000.0f;
static const uint32_t kFragmentSlots = 4u;
static const uint32_t kFragmentSize = 4096u;
static const uint32_t kPlaybackVoices = 16u;
static const float kParamSlew = 0.0015f;
static const float kGuardCeiling = 0.86f;

__sdram float s_fragment[kFragmentSlots][kFragmentSize];

struct FragmentSlot {
  uint32_t length;
  uint32_t generations_left;
  bool valid;
};

struct PlaybackVoice {
  uint32_t slot;
  uint32_t remaining;
  uint32_t total;
  uint32_t delay_remaining;
  float position;
  float increment;
  float gain_l;
  float gain_r;
  float level;
  bool active;
};

static FragmentSlot s_slot[kFragmentSlots];
static PlaybackVoice s_voice[kPlaybackVoices];

static uint32_t s_capture_slot = 0u;
static uint32_t s_capture_pos = 0u;
static uint32_t s_capture_target = 2048u;
static uint32_t s_last_completed_slot = 0u;
static uint32_t s_tick_remaining = 1u;
static uint32_t s_tick_samples = 12000u;
static uint32_t s_tick_counter = 0u;
static uint32_t s_refractory = 0u;
static uint32_t s_since_capture = 0u;
static uint32_t s_voice_limit = 12u;
static bool s_capturing = false;

static float s_fast_env = 0.0f;
static float s_slow_env = 0.0f;
static float s_wet_lp_l = 0.0f;
static float s_wet_lp_r = 0.0f;
static float s_guard_gain = 1.0f;

static float s_clock_target = 0.50f;
static float s_mode_target = 0.10f;
static float s_mix_target = 0.40f;
static float s_clock = 0.50f;
static float s_mode = 0.10f;
static float s_mix = 0.40f;

static uint32_t s_clock_latched = 4u;
static uint32_t s_mode_latched = 0u;

static const float kWetNorm[kPlaybackVoices + 1u] = {
  0.000f,
  1.000f, 0.840f, 0.735f, 0.665f,
  0.610f, 0.570f, 0.535f, 0.505f,
  0.480f, 0.458f, 0.438f, 0.420f,
  0.404f, 0.389f, 0.376f, 0.364f
};

static inline float absf(float x) { return x < 0.0f ? -x : x; }
static inline float maxf(float a, float b) { return a > b ? a : b; }

static inline float clamp01(float x) {
  if (x < 0.0f) return 0.0f;
  if (x > 1.0f) return 1.0f;
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

static inline void guard_pair(float &l, float &r) {
  const float peak = maxf(absf(l), absf(r));
  if (peak > kGuardCeiling) {
    const float needed = kGuardCeiling / peak;
    if (needed < s_guard_gain) s_guard_gain = needed;
  } else {
    s_guard_gain += (1.0f - s_guard_gain) * 0.00032f;
    if (s_guard_gain > 1.0f) s_guard_gain = 1.0f;
  }
  l *= s_guard_gain;
  r *= s_guard_gain;
}

static inline uint32_t clock_index(float value) {
  uint32_t i = static_cast<uint32_t>(value * 7.999f);
  if (i > 7u) i = 7u;
  return i;
}

static inline uint32_t mode_index(float value) {
  uint32_t i = static_cast<uint32_t>(value * 3.999f);
  if (i > 3u) i = 3u;
  return i;
}

static inline float division_beats(uint32_t i) {
  // Quarter-note beat units: 1/32, 1/16T, 1/16, 1/8T, 1/8, 1/4T, 1/4, 1/2.
  static const float kBeats[8] = {
    0.125f, 0.16666667f, 0.25f, 0.33333333f,
    0.50f, 0.66666667f, 1.0f, 2.0f
  };
  return kBeats[i & 7u];
}

static inline uint32_t voice_limit_for_clock(uint32_t division, uint32_t mode) {
  static const uint8_t kLimit[8] = { 8u, 9u, 10u, 11u, 12u, 14u, 16u, 16u };
  uint32_t n = kLimit[division & 7u];
  if (mode == 3u && n > 10u) n = 10u; // STUTTER gets a stricter burst budget.
  return n;
}

static inline uint32_t samples_for_clock(uint32_t division) {
  float bpm = fx_get_bpmf();
  if (bpm < 30.0f || bpm > 300.0f) bpm = 120.0f;
  const float samples = (60.0f * kSampleRate / bpm) * division_beats(division);
  if (samples < 96.0f) return 96u;
  if (samples > 192000.0f) return 192000u;
  return static_cast<uint32_t>(samples);
}

static inline uint32_t capture_length_for_tick(uint32_t tick) {
  uint32_t n = tick / 3u;
  if (n < 896u) n = 896u;
  if (n > kFragmentSize) n = kFragmentSize;
  return n;
}

static void begin_capture(void) {
  s_capture_slot = (s_last_completed_slot + 1u) & (kFragmentSlots - 1u);
  s_capture_pos = 0u;
  s_capture_target = capture_length_for_tick(s_tick_samples);
  s_capturing = true;
  s_since_capture = 0u;
  s_slot[s_capture_slot].valid = false;
  s_slot[s_capture_slot].length = 0u;
  s_slot[s_capture_slot].generations_left = 0u;
}

static void finish_capture(void) {
  FragmentSlot &slot = s_slot[s_capture_slot];
  slot.length = s_capture_pos;
  if (slot.length < 64u) slot.length = 64u;
  if (slot.length > kFragmentSize) slot.length = kFragmentSize;
  slot.generations_left = 16u;
  slot.valid = true;
  s_last_completed_slot = s_capture_slot;
  s_capturing = false;
  s_refractory = static_cast<uint32_t>(kSampleRate * 0.085f);
}

static PlaybackVoice *free_voice(void) {
  for (uint32_t i = 0u; i < s_voice_limit; ++i) {
    if (!s_voice[i].active) return &s_voice[i];
  }
  // Full pool: replace one existing tail. This prevents uncontrolled accumulation.
  return &s_voice[s_tick_counter % s_voice_limit];
}

static void spawn_voice(uint32_t slot_index, bool reverse,
                        uint32_t delay_samples, float pan, float level,
                        uint32_t length_override) {
  FragmentSlot &slot = s_slot[slot_index];
  if (!slot.valid || slot.length < 64u || s_voice_limit == 0u) return;

  PlaybackVoice *v = free_voice();
  uint32_t length = length_override;
  if (length == 0u || length > slot.length) length = slot.length;
  if (length < 32u) length = 32u;

  v->slot = slot_index;
  v->remaining = length;
  v->total = length;
  v->delay_remaining = delay_samples;
  v->increment = reverse ? -1.0f : 1.0f;
  v->position = reverse ? static_cast<float>(length - 1u) : 0.0f;
  if (pan < -1.0f) pan = -1.0f;
  if (pan > 1.0f) pan = 1.0f;
  v->gain_l = 0.5f * (1.0f - pan);
  v->gain_r = 0.5f * (1.0f + pan);
  v->level = level;
  v->active = true;
}

static void schedule_regular_tick(uint32_t mode) {
  static const float kOffset[4] = { 0.0f, 0.19f, 0.46f, 0.76f };
  static const float kLevel[4]  = { 1.00f, 0.78f, 0.61f, 0.47f };
  static const float kPan[4]    = { -0.78f, 0.71f, -0.38f, 0.89f };

  const bool reverse = (mode == 1u);
  const bool pingpong = (mode == 2u);

  for (uint32_t age = 0u; age < kFragmentSlots; ++age) {
    const uint32_t slot_index = (s_last_completed_slot + kFragmentSlots - age) & (kFragmentSlots - 1u);
    FragmentSlot &slot = s_slot[slot_index];
    if (!slot.valid || slot.generations_left == 0u) continue;

    float pan = kPan[age];
    if (pingpong) {
      const bool right = ((s_tick_counter + age) & 1u) != 0u;
      pan = right ? (0.60f + 0.10f * static_cast<float>(age))
                  : (-0.60f - 0.10f * static_cast<float>(age));
    }

    spawn_voice(slot_index, reverse,
                static_cast<uint32_t>(static_cast<float>(s_tick_samples) * kOffset[age]),
                pan, kLevel[age], 0u);

    --slot.generations_left;
    if (slot.generations_left == 0u) slot.valid = false;
  }
}

static void schedule_stutter_tick(void) {
  FragmentSlot &slot = s_slot[s_last_completed_slot];
  if (!slot.valid || slot.generations_left == 0u) return;

  uint32_t slice = s_tick_samples / 8u;
  if (slice < 224u) slice = 224u;
  if (slice > 1664u) slice = 1664u;
  if (slice > slot.length) slice = slot.length;

  // Six retriggers are enough to read as a stutter without exhausting the pool.
  for (uint32_t i = 0u; i < 6u; ++i) {
    const uint32_t offset = (s_tick_samples * i) / 6u;
    const float pan = (i & 1u) ? 0.66f : -0.66f;
    const float level = 0.92f - 0.075f * static_cast<float>(i);
    spawn_voice(s_last_completed_slot, false, offset, pan, level, slice);
  }

  --slot.generations_left;
  if (slot.generations_left == 0u) slot.valid = false;
}

static void schedule_tick(void) {
  s_clock_latched = clock_index(s_clock);
  s_mode_latched = mode_index(s_mode);
  s_tick_samples = samples_for_clock(s_clock_latched);
  s_voice_limit = voice_limit_for_clock(s_clock_latched, s_mode_latched);

  if (s_mode_latched == 3u) schedule_stutter_tick();
  else schedule_regular_tick(s_mode_latched);

  ++s_tick_counter;
}

static void reset_state(void) {
  for (uint32_t s = 0u; s < kFragmentSlots; ++s) {
    for (uint32_t i = 0u; i < kFragmentSize; ++i) s_fragment[s][i] = 0.0f;
    s_slot[s].length = 0u;
    s_slot[s].generations_left = 0u;
    s_slot[s].valid = false;
  }
  for (uint32_t i = 0u; i < kPlaybackVoices; ++i) {
    s_voice[i].slot = 0u;
    s_voice[i].remaining = 0u;
    s_voice[i].total = 0u;
    s_voice[i].delay_remaining = 0u;
    s_voice[i].position = 0.0f;
    s_voice[i].increment = 1.0f;
    s_voice[i].gain_l = 0.5f;
    s_voice[i].gain_r = 0.5f;
    s_voice[i].level = 0.0f;
    s_voice[i].active = false;
  }

  s_capture_slot = 0u;
  s_capture_pos = 0u;
  s_capture_target = 2048u;
  s_last_completed_slot = 0u;
  s_tick_remaining = 1u;
  s_tick_samples = 12000u;
  s_tick_counter = 0u;
  s_refractory = 0u;
  s_since_capture = 0u;
  s_voice_limit = 12u;
  s_capturing = false;
  s_fast_env = s_slow_env = 0.0f;
  s_wet_lp_l = s_wet_lp_r = 0.0f;
  s_guard_gain = 1.0f;

  s_clock_target = s_clock = 0.50f;
  s_mode_target = s_mode = 0.10f;
  s_mix_target = s_mix = 0.40f;
  s_clock_latched = 4u;
  s_mode_latched = 0u;
}

} // namespace

void DELFX_INIT(uint32_t platform, uint32_t api) {
  (void)platform;
  (void)api;
  reset_state();
}

void DELFX_PROCESS(float *xn, uint32_t frames) {
  for (uint32_t f = 0u; f < frames; ++f) {
    s_clock += (s_clock_target - s_clock) * kParamSlew;
    s_mode += (s_mode_target - s_mode) * kParamSlew;
    s_mix += (s_mix_target - s_mix) * kParamSlew;

    const float in_l = xn[f * 2u];
    const float in_r = xn[f * 2u + 1u];
    const float mono = 0.5f * (in_l + in_r);
    const float level = absf(mono);

    s_fast_env += (level - s_fast_env) * 0.050f;
    s_slow_env += (level - s_slow_env) * 0.0012f;
    if (s_refractory > 0u) --s_refractory;
    ++s_since_capture;

    const bool onset = !s_capturing && s_refractory == 0u &&
                       s_fast_env > (s_slow_env * 1.46f + 0.0045f) &&
                       s_fast_env > 0.008f;
    const bool fallback_capture = !s_capturing && s_refractory == 0u &&
                                  s_since_capture > static_cast<uint32_t>(kSampleRate * 0.58f) &&
                                  s_fast_env > 0.009f;
    if (onset || fallback_capture) begin_capture();

    if (s_capturing) {
      s_fragment[s_capture_slot][s_capture_pos] = mono;
      ++s_capture_pos;
      if (s_capture_pos >= s_capture_target || s_capture_pos >= kFragmentSize) finish_capture();
    }

    if (s_tick_remaining == 0u) {
      schedule_tick();
      s_tick_remaining = s_tick_samples;
    }
    if (s_tick_remaining > 0u) --s_tick_remaining;

    float wet_l = 0.0f;
    float wet_r = 0.0f;
    uint32_t sounding = 0u;

    for (uint32_t i = 0u; i < s_voice_limit; ++i) {
      PlaybackVoice &v = s_voice[i];
      if (!v.active) continue;
      if (v.delay_remaining > 0u) {
        --v.delay_remaining;
        continue;
      }

      const FragmentSlot &slot = s_slot[v.slot];
      if (slot.length < 32u || v.remaining == 0u) {
        v.active = false;
        continue;
      }

      int32_t index = static_cast<int32_t>(v.position);
      if (index < 0) index = 0;
      if (index >= static_cast<int32_t>(slot.length)) index = static_cast<int32_t>(slot.length - 1u);
      const float sample = s_fragment[v.slot][static_cast<uint32_t>(index)];

      const uint32_t played = v.total > v.remaining ? (v.total - v.remaining) : 0u;
      float env = 1.0f;
      if (played < 72u) env = static_cast<float>(played) * (1.0f / 72.0f);
      if (v.remaining < 72u) env *= static_cast<float>(v.remaining) * (1.0f / 72.0f);

      wet_l += sample * env * v.level * v.gain_l;
      wet_r += sample * env * v.level * v.gain_r;
      ++sounding;

      v.position += v.increment;
      --v.remaining;
      if (v.remaining == 0u) v.active = false;
    }

    // A small amount of darkening keeps dense repeats smooth without hiding them.
    s_wet_lp_l += (wet_l - s_wet_lp_l) * 0.42f;
    s_wet_lp_r += (wet_r - s_wet_lp_r) * 0.42f;

    if (sounding > kPlaybackVoices) sounding = kPlaybackVoices;
    const float norm = sounding > 0u ? kWetNorm[sounding] : 1.0f;

    // 0.3 deliberately makes ECHO obvious. At full MIX the dry spine retreats
    // while the scheduled fragment field can dominate; the peak guard, rather
    // than a timid wet gain, prevents chain collapse.
    const float dry_gain = 1.0f - 0.65f * s_mix;
    const float wet_gain = (0.62f + 0.68f * s_mix) * s_mix;
    float out_l = in_l * dry_gain + s_wet_lp_l * wet_gain * norm;
    float out_r = in_r * dry_gain + s_wet_lp_r * wet_gain * norm;
    guard_pair(out_l, out_r);
    xn[f * 2u] = soft_limit(out_l);
    xn[f * 2u + 1u] = soft_limit(out_r);
  }
}

void DELFX_SUSPEND(void) { reset_state(); }
void DELFX_RESUME(void) { reset_state(); }

void DELFX_PARAM(uint8_t index, int32_t value) {
  const float normalized = clamp01(q31_to_f32(value));
  if (index == k_user_delfx_param_time) s_clock_target = normalized;
  else if (index == k_user_delfx_param_depth) s_mode_target = normalized;
  else if (index == k_user_delfx_param_shift_depth) s_mix_target = normalized;
}
