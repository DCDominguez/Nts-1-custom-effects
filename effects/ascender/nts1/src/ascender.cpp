#include "usermodfx.h"
#include <stdint.h>

namespace {
static constexpr float SR = 48000.0f;
static float zL[3][4] = {}, zR[3][4] = {};
static float ph = 0.0f;
static float rt = 0.25f, dt = 0.5f, r = 0.25f, d = 0.5f;
static float fbL = 0.0f, fbR = 0.0f;

static inline float c01(float x) { return x < 0.0f ? 0.0f : (x > 1.0f ? 1.0f : x); }
static inline float ca(float x) { return x < -1.0f ? -1.0f : (x > 1.0f ? 1.0f : x); }
static inline float wr(float x) { while (x >= 1.0f) x -= 1.0f; return x; }
static inline float ap(float x, float &z, float a) {
  const float y = -a * x + z;
  z = x + a * y;
  return y;
}

static void reset() {
  for (int j = 0; j < 3; ++j) {
    for (int k = 0; k < 4; ++k) zL[j][k] = zR[j][k] = 0.0f;
  }
  ph = 0.0f;
  fbL = fbR = 0.0f;
}
} // namespace

void MODFX_INIT(uint32_t p, uint32_t a) {
  (void)p;
  (void)a;
  reset();
}

void MODFX_PROCESS(const float *x, float *y, const float *sx, float *sy, uint32_t n) {
  (void)sx;
  (void)sy;

  for (uint32_t i = 0; i < n; ++i) {
    r += (rt - r) * 0.0015f;
    d += (dt - d) * 0.0015f;

    // Keep the climb slow enough to read as motion, but no longer so slow that
    // the direction disappears during normal playing. About 0.05..1 Hz.
    ph = wr(ph + (0.05f + 0.95f * r * r) / SR);

    const float shape = d * (2.0f - d);
    const float feedback = 0.05f + 0.22f * shape;
    const float driveL = x[2 * i] + fbL * feedback;
    const float driveR = x[2 * i + 1] + fbR * feedback;

    float sumL = 0.0f, sumR = 0.0f, ws = 0.0f;
    for (int j = 0; j < 3; ++j) {
      const float p = wr(ph + static_cast<float>(j) / 3.0f);
      float w = fx_sinf(0.5f * p);
      w *= w;

      const float aL = 0.03f + (0.93f * shape) * p;
      const float aR = 0.03f + (0.93f * shape) * wr(p + 0.06f);
      float l = driveL;
      float rr = driveR;
      for (int k = 0; k < 4; ++k) {
        l = ap(l, zL[j][k], aL);
        rr = ap(rr, zR[j][k], aR);
      }
      sumL += l * w;
      sumR += rr * w;
      ws += w;
    }

    if (ws < 0.1f) ws = 0.1f;
    sumL /= ws;
    sumR /= ws;
    fbL = sumL;
    fbR = sumR;

    // A barber-pole phaser needs an unshifted reference. The old voice reached
    // 100% wet at maximum DEPTH, which removed the dry/all-pass interference
    // just where the climb should have been most obvious. Keep the moving field
    // around the classic half-dry/half-wet region instead.
    const float mix = 0.10f + 0.40f * shape;
    y[2 * i] = ca(x[2 * i] * (1.0f - mix) + sumL * mix);
    y[2 * i + 1] = ca(x[2 * i + 1] * (1.0f - mix) + sumR * mix);
  }
}

void MODFX_SUSPEND() { reset(); }
void MODFX_RESUME() { reset(); }

void MODFX_PARAM(uint8_t i, int32_t v) {
  const float n = c01(q31_to_f32(v));
  if (i == k_user_modfx_param_time) rt = n;
  else if (i == k_user_modfx_param_depth) dt = n;
}
