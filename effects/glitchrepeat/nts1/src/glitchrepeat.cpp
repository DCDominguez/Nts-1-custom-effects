#include "userdelfx.h"
#include <stdint.h>

namespace {
static constexpr uint32_t N = 32768u;
static constexpr float SR = 48000.f;
static float bL[N] __sdram, bR[N] __sdram;
static uint32_t w = 0u, beatCnt = 0u, loopStart = 0u, loopPos = 0u, loopLen = 1000u, repeats = 0u;
static uint32_t rng = 0xa511e9b3u;
static float tt = .3f, dt = .25f, mt = .5f;
static float t = .3f, d = .25f, m = .5f;
static inline float c01(float x) { return x < 0.f ? 0.f : (x > 1.f ? 1.f : x); }
static inline float ca(float x) { return x < -1.f ? -1.f : (x > 1.f ? 1.f : x); }
static inline uint32_t xr() { rng ^= rng << 13; rng ^= rng >> 17; rng ^= rng << 5; return rng; }
static void reset() {
  for (uint32_t i = 0u; i < N; ++i) bL[i] = bR[i] = 0.f;
  w = beatCnt = loopStart = loopPos = repeats = 0u;
  loopLen = 1000u;
  rng = 0xa511e9b3u;
  tt = t = .3f;
  dt = d = .25f;
  mt = m = .5f;
}
}

void DELFX_INIT(uint32_t p, uint32_t a) { (void)p; (void)a; reset(); }

void DELFX_PROCESS(float *x, uint32_t n) {
  for (uint32_t i = 0u; i < n; ++i) {
    t += (tt - t) * .0015f;
    d += (dt - d) * .0015f;
    m += (mt - m) * .0015f;
    float bpm = fx_get_bpmf();
    if (bpm < 20.f) bpm = 120.f;
    uint32_t beat = (uint32_t)(SR * 60.f / bpm);
    uint32_t sl = (uint32_t)(160.f + 11500.f * t * t);
    if (sl > N / 2u) sl = N / 2u;
    if (++beatCnt >= beat) {
      beatCnt = 0u;
      if (repeats == 0u && ((xr() & 65535u) < (uint32_t)(d * 65535.f))) {
        loopLen = sl;
        loopStart = (w + N - loopLen) & (N - 1u);
        loopPos = 0u;
        repeats = 2u + (uint32_t)(d * 6.f);
      }
    }
    float inL = x[2u * i], inR = x[2u * i + 1u], wl = inL, wr = inR;
    if (repeats > 0u) {
      uint32_t rp = (loopStart + loopPos) & (N - 1u);
      wl = bL[rp];
      wr = bR[rp];
      if (++loopPos >= loopLen) {
        loopPos = 0u;
        if (--repeats == 0u) loopStart = 0u;
      }
    }
    bL[w] = inL;
    bR[w] = inR;
    w = (w + 1u) & (N - 1u);
    x[2u * i] = ca(inL * (1.f - m) + wl * m);
    x[2u * i + 1u] = ca(inR * (1.f - m) + wr * m);
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
