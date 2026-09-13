#include "usermodfx.h"
#include <math.h>
#include <stdint.h>

namespace {

static constexpr float kSampleRate = 48000.f;
static constexpr float kParamSlew = 0.0015f;

static float s_energy_target = .25f;
static float s_pull_target = 0.f;
static float s_energy = .25f;
static float s_pull = 0.f;

static float s_ball_x = -.63f;
static float s_ball_y = .17f;
static float s_velocity_x = .12f;
static float s_velocity_y = .08f;
static float s_force_x = .18f;
static float s_force_y = -.11f;
static float s_force_target_x = .18f;
static float s_force_target_y = -.11f;
static float s_impulse_phase = 0.f;
static float s_next_impulse = .82f;
static uint32_t s_rng = 0x6d2b79f5u;

static inline float clamp01(const float x) {
  return x < 0.f ? 0.f : (x > 1.f ? 1.f : x);
}

static inline float clamp_audio(const float x) {
  return x < -1.f ? -1.f : (x > 1.f ? 1.f : x);
}

static inline uint32_t random_u32() {
  uint32_t x = s_rng;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  s_rng = x;
  return x;
}

static inline float random_01() {
  return static_cast<float>(random_u32() >> 8) * (1.f / 16777216.f);
}

static inline float random_signed() {
  return 2.f * random_01() - 1.f;
}

static inline void choose_force_target() {
  float x = random_signed();
  float y = random_signed();
  const float length_squared = x * x + y * y;

  // Keep each impulse useful without ever exceeding unit acceleration.
  if (length_squared < .0625f) {
    x = x < 0.f ? -.25f : .25f;
    y = y < 0.f ? -.25f : .25f;
  } else if (length_squared > 1.f) {
    const float scale = 1.f / sqrtf(length_squared);
    x *= scale;
    y *= scale;
  }

  s_force_target_x = x;
  s_force_target_y = y;
  s_next_impulse = .70f + .60f * random_01();
}

static inline void bounce_axis(float &position,
                               float &velocity,
                               float &force,
                               float &force_target,
                               const float restitution) {
  if (position > 1.f) {
    position = 2.f - position;
    velocity = -fabsf(velocity) * restitution;
    if (force > 0.f) force *= -.35f;
    if (force_target > 0.f) force_target = -force_target;
  } else if (position < -1.f) {
    position = -2.f - position;
    velocity = fabsf(velocity) * restitution;
    if (force < 0.f) force *= -.35f;
    if (force_target < 0.f) force_target = -force_target;
  }
}

static inline void integrate_ball(const float energy) {
  const float energy_squared = energy * energy;
  const float impulse_hz = .28f + 4.5f * energy_squared;
  s_impulse_phase += impulse_hz / kSampleRate;
  if (s_impulse_phase >= s_next_impulse) {
    s_impulse_phase -= s_next_impulse;
    choose_force_target();
  }

  const float force_slew = .00035f + .0045f * energy_squared;
  s_force_x += (s_force_target_x - s_force_x) * force_slew;
  s_force_y += (s_force_target_y - s_force_y) * force_slew;

  const float acceleration = .05f + 2.8f * energy_squared;
  s_velocity_x += s_force_x * acceleration / kSampleRate;
  s_velocity_y += s_force_y * acceleration / kSampleRate;

  const float drag = .08f + .35f * energy_squared;
  const float damping = 1.f - drag / kSampleRate;
  s_velocity_x *= damping;
  s_velocity_y *= damping;

  const float max_speed = .16f + 1.45f * energy;
  const float speed_squared = s_velocity_x * s_velocity_x +
                              s_velocity_y * s_velocity_y;
  if (speed_squared > max_speed * max_speed) {
    const float scale = max_speed / sqrtf(speed_squared);
    s_velocity_x *= scale;
    s_velocity_y *= scale;
  }

  s_ball_x += s_velocity_x / kSampleRate;
  s_ball_y += s_velocity_y / kSampleRate;

  const float restitution = .97f - .04f * energy;
  bounce_axis(s_ball_x, s_velocity_x, s_force_x, s_force_target_x,
              restitution);
  bounce_axis(s_ball_y, s_velocity_y, s_force_y, s_force_target_y,
              restitution);
}

static inline void reset_physics() {
  s_energy = s_energy_target;
  s_pull = s_pull_target;
  s_ball_x = -.63f;
  s_ball_y = .17f;
  s_velocity_x = .12f;
  s_velocity_y = .08f;
  s_force_x = s_force_target_x = .18f;
  s_force_y = s_force_target_y = -.11f;
  s_impulse_phase = 0.f;
  s_next_impulse = .82f;
  s_rng = 0x6d2b79f5u;
}

static inline void place_stereo_image(const float input_l,
                                      const float input_r,
                                      const float pull,
                                      float &output_l,
                                      float &output_r) {
  // At full PULL the two original channels retain 56% separation, but both
  // travel with the ball. At zero PULL their positions remain hard L/R.
  const float half_width = 1.f - .72f * pull;
  // Leave a small guard band at the walls so neither channel ever hard-switches
  // completely off and the square-root gain law keeps a bounded derivative.
  const float image_center = .94f * s_ball_x * (1.f - half_width);
  const float left_position = image_center - half_width;
  const float right_position = image_center + half_width;

  // Square-root panning is constant power for each input channel.
  const float ll = sqrtf(.5f * (1.f - left_position));
  const float lr = sqrtf(.5f * (1.f + left_position));
  const float rl = sqrtf(.5f * (1.f - right_position));
  const float rr = sqrtf(.5f * (1.f + right_position));

  // Correlated stereo (including mono duplicated to L/R) would otherwise
  // gain level as the two source positions converge. This normalization keeps
  // duplicated mono at constant total power throughout the motion.
  const float mono_l = ll + rl;
  const float mono_r = lr + rr;
  const float normalization = sqrtf(2.f /
      (mono_l * mono_l + mono_r * mono_r));

  output_l = (input_l * ll + input_r * rl) * normalization;
  output_r = (input_l * lr + input_r * rr) * normalization;
}

}  // namespace

void MODFX_INIT(uint32_t platform, uint32_t api) {
  (void)platform;
  (void)api;
  s_energy_target = .25f;
  s_pull_target = 0.f;
  reset_physics();
}

void MODFX_PROCESS(const float *main_xn,
                   float *main_yn,
                   const float *sub_xn,
                   float *sub_yn,
                   uint32_t frames) {
  (void)sub_xn;
  (void)sub_yn;

  for (uint32_t i = 0; i < frames; ++i) {
    s_energy += (s_energy_target - s_energy) * kParamSlew;
    s_pull += (s_pull_target - s_pull) * kParamSlew;
    if (s_pull_target == 0.f && s_pull < 1.e-7f) s_pull = 0.f;

    integrate_ball(s_energy);

    const float input_l = main_xn[2u * i];
    const float input_r = main_xn[2u * i + 1u];

    // The eased mapping makes 50% DEPTH already read as an obvious pull.
    const float pull = s_pull * (2.f - s_pull);
    if (pull == 0.f) {
      main_yn[2u * i] = input_l;
      main_yn[2u * i + 1u] = input_r;
      continue;
    }

    float moved_l;
    float moved_r;
    place_stereo_image(input_l, input_r, pull, moved_l, moved_r);
    main_yn[2u * i] = clamp_audio(input_l + (moved_l - input_l) * pull);
    main_yn[2u * i + 1u] =
        clamp_audio(input_r + (moved_r - input_r) * pull);
  }
}

void MODFX_SUSPEND(void) {
  reset_physics();
}

void MODFX_RESUME(void) {
  reset_physics();
}

void MODFX_PARAM(uint8_t index, int32_t value) {
  const float normalized = clamp01(q31_to_f32(value));
  if (index == k_user_modfx_param_time) {
    s_energy_target = normalized;
  } else if (index == k_user_modfx_param_depth) {
    s_pull_target = normalized;
  }
}

