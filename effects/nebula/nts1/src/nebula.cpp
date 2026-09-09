#include "userrevfx.h"
#include <stdint.h>

namespace {

static constexpr uint32_t N = 32768u;
static float bL[N] __sdram;
static float bR[N] __sdram;
static float ph[4] = {0.0f, 0.23f, 0.51f, 0.77f};
static float del[4] = {2600.0f, 5200.0f, 8800.0f, 13000.0f};
static uint32_t w = 0u;
static uint32_t rng = 0x6a09e667u;
static float tt = 0.5f, dt = 0.45f, mt = 0.35f;
static float t = 0.5f, d = 0.45f, m = 0.35f;

static inline float c01(float x) { return x < 0.0f ? 0.0f : (x > 1.0f ? 1.0f : x); }
static inline float ca(float x) { return x < -1.0f ? -1.0f : (x > 1.0f ? 1.0f : x); }
static inline float wr(float x) { return x >= 1.0f ? x - 1.0f : x; }

static inline float softsat(float x) {
  const float x2 = x * x;
  return x * (27.0f + x2) / (27.0f + 9.0f * x2);
}

static inline uint32_t xr() {
  rng ^= rng << 13;
  rng ^= rng >> 17;
  rng ^= rng << 5;
  return rng;
}

static inline float rf() { return (xr() & 0xffffu) / 65535.0f; }

static inline float rd(float *b, float de) {
  float p = (float)w - de;
  while (p < 0.0f) p += (float)N;
  uint32_t a = (uint32_t)p & (N - 1u);
  uint32_t c = (a + 1u) & (N - 1u);
  float f = p - (uint32_t)p;
  return b[a] + (b[c] - b[a]) * f;
}

static inline float win(float p) {
  const float s = fx_sinf(0.5f * p);
  return s * s;
}

static void reset() {
  for (uint32_t i = 0u; i < N; ++i) bL[i] = bR[i] = 0.0f;
  w = 0u;
  rng = 0x6a09e667u;
  ph[0] = 0.0f;
  ph[1] = 0.23f;
  ph[2] = 0.51f;
  ph[3] = 0.77f;
}

} // namespace

void REVFX_INIT(uint32_t p, uint32_t a) {
  (void)p;
  (void)a;
  reset();
}

void REVFX_PROCESS(float *x, uint32_t n) {
  for (uint32_t i = 0u; i < n; ++i) {
    t += (tt - t) * 0.0012f;
    d += (dt - d) * 0.0012f;
    m += (mt - m) * 0.0012f;

    int active = 1 + (int)(d * 3.99f);
    float wl = 0.0f;
    float wrv = 0.0f;
    float ws = 0.0f;
    const float maxdel = 2500.0f + 27000.0f * t * t;

    for (int j = 0; j < active; ++j) {
      const float gl = 700.0f + 2100.0f * (0.25f + 0.75f * d) * (1.0f + 0.17f * j);
      const float old = ph[j];
      ph[j] = wr(ph[j] + 1.0f / gl);
      if (ph[j] < old) del[j] = 300.0f + rf() * maxdel;

      const float ww = win(ph[j]);
      const float l = rd(bL, del[j]);
      const float r = rd(bR, del[j] + (j - 1.5f) * 37.0f * d);
      wl += l * ww;
      wrv += r * ww;
      ws += ww;
    }

    if (ws < 0.05f) ws = 0.05f;
    wl /= ws;
    wrv /= ws;

    // Bound the cloud before it reaches either the feedback memory or output.
    // M1 used hard clipping at both points; this hotfix creates explicit
    // headroom and uses soft limiting only inside the wet path.
    wl = softsat(wl * 0.90f);
    wrv = softsat(wrv * 0.90f);

    const float inL = x[2u * i];
    const float inR = x[2u * i + 1u];
    const float fb = 0.10f + 0.42f * d;

    // Reduced input injection + bounded feedback prevents full-scale sources
    // from repeatedly slamming the delay memory into the ±1 hard limit.
    bL[w] = softsat(inL * 0.70f + wl * fb);
    bR[w] = softsat(inR * 0.70f + wrv * fb);

    // Wet path is deliberately capped below unity so correlated dry/wet peaks
    // have headroom throughout the MIX sweep.
    const float wet_gain = 0.78f;
    const float outL = inL * (1.0f - m) + wl * (m * wet_gain);
    const float outR = inR * (1.0f - m) + wrv * (m * wet_gain);
    x[2u * i] = ca(outL);
    x[2u * i + 1u] = ca(outR);

    w = (w + 1u) & (N - 1u);
  }
}

void REVFX_SUSPEND() { reset(); }
void REVFX_RESUME() { reset(); }

void REVFX_PARAM(uint8_t i, int32_t v) {
  const float n = c01(q31_to_f32(v));
  if (i == k_user_revfx_param_time) tt = n;
  else if (i == k_user_revfx_param_depth) dt = n;
  else if (i == k_user_revfx_param_shift_depth) mt = n;
}
