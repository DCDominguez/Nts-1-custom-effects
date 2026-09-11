#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <vector>
#include "tests/pre-handoff/common/measure.hpp"
#include "userdelfx.h"

namespace ballistic_fx {
#include "effects/ballistic/nts1/src/ballistic.cpp"
}
namespace rainfall_fx {
#include "effects/rainfall/nts1/src/rainfall.cpp"
}
namespace swarm_fx {
#include "effects/swarmdelay/nts1/src/swarmdelay.cpp"
}
namespace glitch_fx {
#include "effects/glitchrepeat/nts1/src/glitchrepeat.cpp"
}
namespace bucket_fx {
#include "effects/bucketline/nts1/src/bucketline.cpp"
}
namespace long_fx {
#include "effects/longmemory/nts1/src/longmemory.cpp"
}
namespace shard_fx {
#include "effects/shard/nts1/src/shard.cpp"
}

using Init = void (*)(uint32_t, uint32_t);
using Process = void (*)(float *, uint32_t);
using Param = void (*)(uint8_t, int32_t);

struct Unit { const char *name; Init init; Process process; Param param; };

static int32_t q(float x) {
  x = std::max(0.0f, std::min(0.999999f, x));
  return static_cast<int32_t>(x * 2147483647.0f);
}

static std::vector<float> source(std::size_t frames) {
  std::vector<float> x(frames * 2u);
  uint32_t rng = 0x9e3779b9u;
  for (std::size_t i = 0; i < frames; ++i) {
    rng ^= rng << 13; rng ^= rng >> 17; rng ^= rng << 5;
    const float noise = (static_cast<int32_t>(rng & 0xffffu) - 32768) / 32768.0f;
    const float a = 0.105f * std::sin(2.0 * hs_measure::kPi * 173.0 * i / 48000.0);
    const float b = 0.072f * std::sin(2.0 * hs_measure::kPi * 419.0 * i / 48000.0);
    const float c = 0.046f * std::sin(2.0 * hs_measure::kPi * 997.0 * i / 48000.0);
    const float l = a + b + c + 0.020f * noise;
    const float r = 0.94f * a - 0.66f * b + 0.78f * c - 0.016f * noise;
    x[2u * i] = l;
    x[2u * i + 1u] = r;
  }
  return x;
}

static void process(Unit &u, std::vector<float> &x) {
  const std::size_t frames = x.size() / 2u;
  std::size_t p = 0;
  while (p < frames) {
    const uint32_t n = static_cast<uint32_t>(std::min<std::size_t>(64u, frames - p));
    u.process(x.data() + 2u * p, n);
    p += n;
  }
}

static double rms_stereo(const std::vector<float> &x, std::size_t skip) {
  const std::size_t frames = x.size() / 2u;
  auto l = hs_measure::channel(x, 0, 2, skip, frames - skip);
  auto r = hs_measure::channel(x, 1, 2, skip, frames - skip);
  return 0.5 * (hs_measure::stats(l).rms + hs_measure::stats(r).rms);
}

static double peak_stereo(const std::vector<float> &x, std::size_t skip) {
  const std::size_t frames = x.size() / 2u;
  auto l = hs_measure::channel(x, 0, 2, skip, frames - skip);
  auto r = hs_measure::channel(x, 1, 2, skip, frames - skip);
  return std::max(hs_measure::stats(l).peak, hs_measure::stats(r).peak);
}

static double diff_rms(const std::vector<float> &a, const std::vector<float> &b, std::size_t skip) {
  double e = 0.0; std::size_t n = 0;
  for (std::size_t i = skip * 2u; i < a.size(); ++i) {
    const double d = static_cast<double>(a[i]) - static_cast<double>(b[i]);
    e += d * d; ++n;
  }
  return n ? std::sqrt(e / static_cast<double>(n)) : 0.0;
}

static void probe(Unit u, float mix) {
  constexpr std::size_t frames = 8u * 48000u;
  constexpr std::size_t skip = 2u * 48000u;
  auto in = source(frames);
  auto out = in;
  u.init(0, 0);
  u.param(k_user_delfx_param_time, q(0.55f));
  u.param(k_user_delfx_param_depth, q(0.55f));
  u.param(k_user_delfx_param_shift_depth, q(mix));
  process(u, out);
  const double in_rms = rms_stereo(in, skip);
  const double out_rms = rms_stereo(out, skip);
  const double delta = diff_rms(out, in, skip);
  const double peak = peak_stereo(out, skip);
  std::printf("%s mix=%.2f out/input=%.3f delta/input=%.3f peak=%.3f\n",
              u.name, mix, out_rms / in_rms, delta / in_rms, peak);
}

int main() {
  Unit units[] = {
    {"BALLISTIC", ballistic_fx::DELFX_INIT, ballistic_fx::DELFX_PROCESS, ballistic_fx::DELFX_PARAM},
    {"RAINFALL", rainfall_fx::DELFX_INIT, rainfall_fx::DELFX_PROCESS, rainfall_fx::DELFX_PARAM},
    {"SWARMDELAY", swarm_fx::DELFX_INIT, swarm_fx::DELFX_PROCESS, swarm_fx::DELFX_PARAM},
    {"GLITCHREPEAT", glitch_fx::DELFX_INIT, glitch_fx::DELFX_PROCESS, glitch_fx::DELFX_PARAM},
    {"BUCKETLINE", bucket_fx::DELFX_INIT, bucket_fx::DELFX_PROCESS, bucket_fx::DELFX_PARAM},
    {"LONGMEMORY", long_fx::DELFX_INIT, long_fx::DELFX_PROCESS, long_fx::DELFX_PARAM},
    {"SHARD", shard_fx::DELFX_INIT, shard_fx::DELFX_PROCESS, shard_fx::DELFX_PARAM},
  };
  for (auto &u : units) { probe(u, 0.50f); probe(u, 1.00f); }
  return 0;
}
