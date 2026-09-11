#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include "tests/pre-handoff/common/measure.hpp"
#include "usermodfx.h"
#include "userrevfx.h"

namespace core {
#include "effects/lattice-core/nts1/src/lattice_core.cpp"
}

namespace spacefx {
#include "effects/lattice-cloud/nts1/src/lattice_cloud.cpp"
}

static void req(bool ok, const char *m) {
  if (!ok) {
    std::fprintf(stderr, "LATTICE SPACE FAIL: %s\n", m);
    std::exit(2);
  }
}

static int32_t q(float x) {
  x = std::max(0.0f, std::min(0.999999f, x));
  return static_cast<int32_t>(x * 2147483647.0f);
}

static void set_space(float room, float drift, float mix) {
  spacefx::REVFX_PARAM(k_user_revfx_param_time, q(room));
  spacefx::REVFX_PARAM(k_user_revfx_param_depth, q(drift));
  spacefx::REVFX_PARAM(k_user_revfx_param_shift_depth, q(mix));
}

static void settle_space(float room, float drift, float mix, int blocks = 260) {
  set_space(room, drift, mix);
  std::vector<float> z(128, 0.0f);
  for (int i = 0; i < blocks; ++i) spacefx::REVFX_PROCESS(z.data(), 64);
}

static void proc_space(std::vector<float> &x) {
  std::size_t p = 0;
  const std::size_t n = x.size() / 2;
  while (p < n) {
    const uint32_t k = static_cast<uint32_t>(std::min<std::size_t>(64, n - p));
    spacefx::REVFX_PROCESS(x.data() + 2 * p, k);
    p += k;
  }
}

static std::vector<float> tone(std::size_t n, double hz = 0.0, double amp = 0.35) {
  std::vector<float> x(n * 2);
  for (std::size_t i = 0; i < n; ++i) {
    const float v = hz ? static_cast<float>(amp * std::sin(2.0 * hs_measure::kPi * hz * i / 48000.0)) : 0.0f;
    x[2 * i] = v;
    x[2 * i + 1] = v;
  }
  return x;
}

static std::vector<float> impulse_tail(float room, float drift, std::size_t frames, float mix = 1.0f) {
  spacefx::REVFX_INIT(0, 0);
  settle_space(room, drift, mix);
  std::vector<float> x(frames * 2, 0.0f);
  x[0] = x[1] = 0.7f;
  proc_space(x);
  return x;
}

static double stereo_rms(const std::vector<float> &x, std::size_t skip = 0) {
  const auto l = hs_measure::channel(x, 0, 2, skip, x.size() / 2 - skip);
  const auto r = hs_measure::channel(x, 1, 2, skip, x.size() / 2 - skip);
  return 0.5 * (hs_measure::stats(l).rms + hs_measure::stats(r).rms);
}

int main() {
  // MIX=0 remains an exact dry contract even while the room network runs.
  spacefx::REVFX_INIT(0, 0);
  settle_space(0.6f, 0.4f, 0.0f);
  auto dry = tone(50000, 701.0);
  const auto ref = dry;
  proc_space(dry);
  req(hs_measure::max_abs_diff(dry, ref) < 2e-6, "MIX=0 is not dry");

  // The 0.4 revoice must solve the physical 'barely there at full MIX' problem
  // in measurable terms. Mid MIX has to materially alter a sustained source,
  // and full MIX must retain useful output energy rather than collapse level.
  spacefx::REVFX_INIT(0, 0);
  settle_space(0.72f, 0.45f, 0.55f);
  auto mid = tone(144000, 347.0, 0.35);
  const auto mid_ref = mid;
  proc_space(mid);
  req(hs_measure::max_abs_diff(mid, mid_ref) > 0.04, "mid MIX remains too close to dry");
  req(stereo_rms(mid, 24000) > 0.075, "mid MIX output level collapsed");

  spacefx::REVFX_INIT(0, 0);
  settle_space(0.72f, 0.45f, 1.0f);
  auto full = tone(144000, 347.0, 0.35);
  const auto full_ref = full;
  proc_space(full);
  req(hs_measure::max_abs_diff(full, full_ref) > 0.10, "full MIX lacks clear wet identity");
  req(stereo_rms(full, 24000) > 0.060, "full MIX output level is too low");

  // Preserve an obvious early echo/spatial statement before the diffuse tail.
  auto early = impulse_tail(0.45f, 0.25f, 12000);
  auto el = hs_measure::channel(early, 0);
  const double early_energy = hs_measure::stats(el, 250, 1800).energy;
  req(early_energy > 1e-4, "early echo/spatial response is not materially delivered");

  // SPACE must increase room persistence rather than merely output level.
  auto small = impulse_tail(0.05f, 0.25f, 6 * 48000);
  auto large = impulse_tail(0.98f, 0.25f, 6 * 48000);
  auto sl = hs_measure::channel(small, 0);
  auto ll = hs_measure::channel(large, 0);
  const double small_late = hs_measure::stats(sl, 2 * 48000, 2 * 48000).energy;
  const double large_late = hs_measure::stats(ll, 2 * 48000, 2 * 48000).energy;
  req(large_late > small_late * 1.5 + 1e-8, "SPACE does not materially increase late-room persistence");

  // DRIFT must reach audible output and widen/move the stereo geometry without
  // requiring modulated delay reads.
  spacefx::REVFX_INIT(0, 0);
  settle_space(0.72f, 0.01f, 1.0f);
  auto still = tone(180000, 347.0);
  proc_space(still);
  spacefx::REVFX_INIT(0, 0);
  settle_space(0.72f, 1.0f, 1.0f);
  auto moving = tone(180000, 347.0);
  proc_space(moving);
  req(hs_measure::max_abs_diff(still, moving) > 0.005, "DRIFT does not reach rendered output");
  auto ml = hs_measure::channel(moving, 0, 2, 30000, 120000);
  auto mr = hs_measure::channel(moving, 1, 2, 30000, 120000);
  req(hs_measure::max_abs_diff(ml, mr) > 0.005, "SPACE output lacks stereo field");

  // Maximum SPACE must remain bounded and ultimately return toward rest.
  auto tail = impulse_tail(1.0f, 1.0f, 14 * 48000);
  auto tl = hs_measure::channel(tail, 0);
  req(hs_measure::stats(tl).peak <= 0.996, "maximum SPACE exceeded output bounds");
  const double end = hs_measure::stats(tl, 13 * 48000, 48000).rms;
  req(end < 0.0015, "maximum SPACE tail failed to decay toward rest");
  req(spacefx::s_guard_gain > 0.0f && spacefx::s_guard_gain <= 1.001f, "output guard entered invalid state");

  // Guard recovery: a hot burst may attenuate, but a later moderate signal must
  // not leave SPACE trapped at a stale low gain as could happen in 0.3.
  spacefx::REVFX_INIT(0, 0);
  settle_space(0.8f, 0.5f, 0.9f);
  auto hot = tone(24000, 193.0, 0.95);
  proc_space(hot);
  auto moderate = tone(96000, 193.0, 0.25);
  proc_space(moderate);
  req(spacefx::s_guard_gain > 0.78f, "output guard failed to recover from hot burst");
  req(stereo_rms(moderate, 24000) > 0.04, "post-overload room remained excessively attenuated");

  // Production chain stress: CORE -> SPACE. This checks actual DSP interaction
  // for bounded/finite output and useful delivery, but makes no ARM timing claim.
  core::MODFX_INIT(0, 0);
  spacefx::REVFX_INIT(0, 0);
  core::MODFX_PARAM(k_user_modfx_param_time, q(0.76f));
  core::MODFX_PARAM(k_user_modfx_param_depth, q(0.70f));
  set_space(0.78f, 0.62f, 0.82f);
  std::vector<float> in(128), modout(128), sub(128), subout(128);
  double chain_energy = 0.0;
  for (int block = 0; block < 5000; ++block) {
    for (int i = 0; i < 64; ++i) {
      const uint32_t n = static_cast<uint32_t>(block * 64 + i);
      const float env = ((n / 2400u) & 1u) ? 0.32f : 0.52f;
      const float v = env * std::sin(2.0 * hs_measure::kPi * (147.0 + (block % 7) * 23.0) * n / 48000.0);
      in[2 * i] = v;
      in[2 * i + 1] = v * 0.92f;
    }
    core::MODFX_PROCESS(in.data(), modout.data(), sub.data(), subout.data(), 64);
    spacefx::REVFX_PROCESS(modout.data(), 64);
    for (float v : modout) {
      req(std::isfinite(v), "CORE -> SPACE produced non-finite output");
      req(std::fabs(v) <= 0.996f, "CORE -> SPACE exceeded output bound");
      chain_energy += static_cast<double>(v) * static_cast<double>(v);
    }
  }
  req(chain_energy > 100.0, "CORE -> SPACE chain lost useful audio delivery");

  // Suspend/resume clears predelay/diffusion/FDN state.
  spacefx::REVFX_SUSPEND();
  spacefx::REVFX_RESUME();
  std::vector<float> z(16000 * 2, 0.0f);
  proc_space(z);
  req(hs_measure::stats(hs_measure::channel(z, 0)).peak < 1e-7, "reset emitted stale room history");

  // Long control/wrap soak covers every circular buffer and guard recovery.
  spacefx::REVFX_INIT(0, 0);
  std::vector<float> b(128);
  for (int bb = 0; bb < 11000; ++bb) {
    set_space(((bb * 17) % 101) / 100.0f,
              ((bb * 43) % 101) / 100.0f,
              ((bb * 79) % 101) / 100.0f);
    for (int i = 0; i < 64; ++i) {
      const uint32_t n = static_cast<uint32_t>(bb * 64 + i);
      const float v = 0.78f * std::sin(2.0 * hs_measure::kPi *
                                     (97.0 + (bb % 17) * 107.0) * n / 48000.0);
      b[2 * i] = v;
      b[2 * i + 1] = -0.72f * v;
    }
    spacefx::REVFX_PROCESS(b.data(), 64);
    for (float v : b) {
      req(std::isfinite(v), "non-finite soak output");
      req(std::fabs(v) <= 0.996f, "soak output exceeded bounds");
    }
  }

  std::puts("LATTICE SPACE 0.4 A-class harness PASS (presence + CORE-chain stress included)");
  return 0;
}
