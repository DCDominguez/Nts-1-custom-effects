#include "userdelfx.h"
#include <stdint.h>

namespace {
static constexpr uint32_t N = 65536u;
static constexpr float SR = 48000.f;
static float bL[N] __sdram, bR[N] __sdram;
static float del[4] = {5000.f, 9000.f, 15000.f, 22000.f};
static float tar[4] = {5000.f, 9000.f, 15000.f, 22000.f};
static float pan[4] = {-.7f, -.2f, .25f, .75f};
static uint32_t w = 0u, cnt = 0u, rng = 0x243f6a88u;
static int voice = 0;
static float tt = .25f, dt = .35f, mt = .35f;
static float t = .25f, d = .35f, m = .35f;

static inline float c01(float x) { return x < 0.f ? 0.f : (x > 1.f ? 1.f : x); }
static inline float ca(float x) { return x < -1.f ? -1.f : (x > 1.f ? 1.f : x); }
static inline uint32_t xr() { rng ^= rng << 13; rng ^= rng >> 17; rng ^= rng << 5; return rng; }
static inline float rf() { return (xr() & 0xffffu) / 65535.f; }
static inline float rd(float *b, float de) {
  float p = (float)w - de;
  while (p < 0.f) p += N;
  uint32_t a = (uint32_t)p & (N - 1u), c = (a + 1u) & (N - 1u);
  float f = p - (uint32_t)p;
  return b[a] + (b[c] - b[a]) * f;
}

static void reset() {
  for (uint32_t i = 0u; i < N; ++i) bL[i] = bR[i] = 0.f;
  w = 0u;
  cnt = 0u;
  voice = 0;
  rng = 0x243f6a88u;
  del[0] = tar[0] = 5000.f;
  del[1] = tar[1] = 9000.f;
  del[2] = tar[2] = 15000.f;
  del[3] = tar[3] = 22000.f;
  pan[0] = -.7f;
  pan[1] = -.2f;
  pan[2] = .25f;
  pan[3] = .75f;
  tt = t = .25f;
  dt = d = .35f;
  mt = m = .35f;
}
}

void DELFX_INIT(uint32_t p, uint32_t a) { (void)p; (void)a; reset(); }

void DELFX_PROCESS(float *x, uint32_t n) {
  for (uint32_t i = 0u; i < n; ++i) {
    t += (tt - t) * .0015f;
    d += (dt - d) * .0015f;
    m += (mt - m) * .0015f;
    uint32_t period = (uint32_t)(SR / (.5f + 11.f * t * t));
    if (++cnt >= period) {
      cnt = 0u;
      float span = 5000.f + 42000.f * d;
      tar[voice] = 900.f + rf() * span;
      pan[voice] = rf() * 1.8f - .9f;
      voice = (voice + 1) & 3;
    }
    float wl = 0.f, wr = 0.f;
    for (int j = 0; j < 4; ++j) {
      del[j] += (tar[j] - del[j]) * .00035f;
      float mono = .5f * (rd(bL, del[j]) + rd(bR, del[j]));
      float pg = pan[j] * d;
      wl += mono * (.25f * (1.f - pg));
      wr += mono * (.25f * (1.f + pg));
    }
    float inL = x[2u * i], inR = x[2u * i + 1u], fb = .18f + .38f * d;
    bL[w] = ca(inL + wl * fb);
    bR[w] = ca(inR + wr * fb);
    x[2u * i] = ca(inL * (1.f - m) + wl * m);
    x[2u * i + 1u] = ca(inR * (1.f - m) + wr * m);
    w = (w + 1u) & (N - 1u);
  }
}

void DELFX_SUSPEND() { reset(); }
void DELFX_RESUME() { reset(); }

void DELFX_PARAM(uint8_t i, int32_t v) {
  float n = c01(q31_to_f32(v));
  if (i == k_user_delfx_param_time) tt = n;
  else if (i == k_user_delfx_param_depth) dt = n;
  else if (i == k_user_delfx_param_shift_depth) mt = n;
}
