#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include "tests/pre-handoff/common/measure.hpp"
#include "effects/dust/nts1/src/dust.cpp"

static void require(bool ok, const char* message) {
  if (!ok) {
    std::fprintf(stderr, "DUST FAIL: %s\n", message);
    std::exit(2);
  }
}

static int32_t q31p(float x) {
  x = std::max(0.0f, std::min(0.999999f, x));
  return static_cast<int32_t>(x * 2147483647.0f);
}

static void set_controls(float rate, float damage) {
  MODFX_PARAM(k_user_modfx_param_time, q31p(rate));
  MODFX_PARAM(k_user_modfx_param_depth, q31p(damage));
}

static void settle(float rate, float damage, int blocks = 120) {
  set_controls(rate, damage);
  std::vector<float> z(128, 0.0f), y(128, 0.0f), sub(128, 0.0f), sy(128, 0.0f);
  for (int i = 0; i < blocks; ++i) MODFX_PROCESS(z.data(), y.data(), sub.data(), sy.data(), 64);
}

static std::vector<float> process_mono(const std::vector<float>& mono) {
  std::vector<float> in(mono.size() * 2), out(mono.size() * 2), sub(mono.size() * 2), sy(mono.size() * 2);
  for (std::size_t i = 0; i < mono.size(); ++i) in[2*i] = in[2*i+1] = mono[i];
  std::size_t pos = 0;
  while (pos < mono.size()) {
    const uint32_t n = static_cast<uint32_t>(std::min<std::size_t>(64, mono.size() - pos));
    MODFX_PROCESS(in.data() + pos*2, out.data() + pos*2,
                  sub.data() + pos*2, sy.data() + pos*2, n);
    pos += n;
  }
  return out;
}

static std::vector<float> sine(std::size_t frames, float hz, float amp) {
  std::vector<float> x(frames);
  for (std::size_t i = 0; i < frames; ++i)
    x[i] = amp * std::sin(2.0 * hs_measure::kPi * hz * static_cast<double>(i) / 48000.0);
  return x;
}

static std::vector<float> deterministic_noise(std::size_t frames) {
  std::vector<float> x(frames);
  uint32_t state = 0x51f15eadu;
  for (std::size_t i=0;i<frames;++i) {
    state = state * 1664525u + 1013904223u;
    const float u = static_cast<float>((state >> 8) & 0x00ffffffu) / 16777215.0f;
    x[i] = (u * 2.0f - 1.0f) * 0.8f;
  }
  return x;
}

int main() {
  // Dry contract.
  MODFX_INIT(0,0);
  settle(1.0f, 0.0f);
  auto src = sine(8192, 997.0f, 0.35f);
  auto dry = process_mono(src);
  auto dryL = hs_measure::channel(dry, 0);
  require(hs_measure::max_abs_diff(src, dryL) < 2e-6, "DEPTH=0 is not effectively dry");

  // RATE contract. At maximum DAMAGE there is still a numerically tiny dry component because
  // the smoothed float coefficient asymptotically approaches its target. Count only output
  // changes large enough to represent held/quantized updates rather than that residual dry floor.
  auto count_held_updates = [&](float rate) {
    MODFX_INIT(0,0);
    settle(rate, 1.0f);
    auto y = process_mono(deterministic_noise(65536));
    return hs_measure::transition_count(hs_measure::channel(y,0), 0.01);
  };
  const std::size_t rate_lo = count_held_updates(0.10f);
  const std::size_t rate_mid = count_held_updates(0.55f);
  const std::size_t rate_hi = count_held_updates(1.00f);
  std::fprintf(stderr, "DUST RATE held updates: low=%zu mid=%zu high=%zu\n", rate_lo, rate_mid, rate_hi);
  require(rate_lo > rate_mid * 6, "RATE low->mid did not materially reduce held-update cadence");
  require(rate_mid > rate_hi * 2, "RATE mid->max did not materially reduce held-update cadence");

  // DAMAGE contract: compare audible departure from the dry input. The current design intentionally
  // crossfades dry->crushed as DAMAGE rises, so raw unique-level counting at intermediate settings
  // would incorrectly measure the remaining dry component rather than the quantizer's contribution.
  auto damage_error = [&](float damage) {
    MODFX_INIT(0,0);
    settle(0.0f, damage);
    auto in = sine(48000, 997.0f, 0.31f);
    auto y = hs_measure::channel(process_mono(in),0);
    double e = 0.0;
    for (std::size_t i=0;i<in.size();++i) { const double d = static_cast<double>(y[i]) - in[i]; e += d*d; }
    return std::sqrt(e / static_cast<double>(in.size()));
  };
  const double err10 = damage_error(0.10f);
  const double err50 = damage_error(0.50f);
  const double err100 = damage_error(1.0f);
  require(err50 > err10 * 2.0, "DAMAGE 10->50% did not materially increase destructive error");
  require(err100 > err50 * 1.25, "DAMAGE 50->100% did not materially increase destructive error");

  // Maximum DAMAGE should expose a bounded low-bit staircase on a ramp.
  MODFX_INIT(0,0);
  settle(0.0f, 1.0f);
  std::vector<float> ramp(32768);
  for (std::size_t i=0;i<ramp.size();++i) ramp[i] = -0.95f + 1.90f * static_cast<float>(i) / static_cast<float>(ramp.size()-1);
  auto maxDamage = hs_measure::channel(process_mono(ramp),0);
  const std::size_t maxLevels = hs_measure::approximate_unique_levels(maxDamage, 0.005);
  require(maxLevels >= 8 && maxLevels <= 24, "maximum DAMAGE does not expose bounded low-bit staircase behavior");

  // Stereo fracture contract.
  MODFX_INIT(0,0);
  settle(0.9f, 0.45f);
  auto mono = sine(48000, 701.0f, 0.4f);
  auto mild = process_mono(mono);
  const auto mildL = hs_measure::channel(mild,0), mildR = hs_measure::channel(mild,1);
  require(hs_measure::normalized_correlation(mildL,mildR) > 0.995, "mild settings fracture mono image unexpectedly");

  MODFX_INIT(0,0);
  settle(1.0f, 1.0f);
  auto strong = process_mono(mono);
  const auto strongL = hs_measure::channel(strong,0), strongR = hs_measure::channel(strong,1);
  const auto sl = hs_measure::stats(strongL), sr = hs_measure::stats(strongR);
  require(sl.rms > 1e-4 && sr.rms > 1e-4, "strong stereo fracture dropped a channel");
  require(hs_measure::normalized_correlation(strongL,strongR) < 0.985, "strong RATE+DAMAGE did not create measurable stereo fracture");
  require(sl.peak <= 1.001 && sr.peak <= 1.001, "stereo fracture violated output bound");

  // Low-level/DC contract.
  MODFX_INIT(0,0);
  settle(0.0f, 0.8f);
  auto low = process_mono(sine(96000, 311.0f, 0.015f));
  const auto lowStats = hs_measure::stats(hs_measure::channel(low,0));
  require(std::fabs(lowStats.mean) < 0.0025, "low-level DAMAGE created excessive DC bias");

  // Lifecycle contract.
  MODFX_INIT(0,0);
  settle(1.0f,1.0f);
  (void)process_mono(sine(4096,220.0f,0.7f));
  MODFX_SUSPEND();
  MODFX_RESUME();
  std::vector<float> silence(8192,0.0f);
  auto silentOut = process_mono(silence);
  require(hs_measure::stats(hs_measure::channel(silentOut,0)).peak < 1e-7,
          "suspend/resume left stale held audio into silence");

  // Abuse/soak.
  MODFX_INIT(0,0);
  std::vector<float> in(128), out(128), sub(128), sy(128);
  for (int block=0; block<6000; ++block) {
    const float r = static_cast<float>((block * 37) % 101) / 100.0f;
    const float d = static_cast<float>((block * 61) % 101) / 100.0f;
    set_controls(r,d);
    for (int i=0;i<64;++i) {
      const float v = 0.6f * std::sin(2.0 * hs_measure::kPi * (83.0 + (block%17)*41.0) * (block*64+i) / 48000.0);
      in[2*i]=v; in[2*i+1]=v*0.91f;
    }
    MODFX_PROCESS(in.data(),out.data(),sub.data(),sy.data(),64);
    for (float v:out) {
      require(std::isfinite(v), "non-finite output during abuse/soak");
      require(std::fabs(v)<=1.001f, "output bound exceeded during abuse/soak");
    }
  }

  std::puts("DUST A-class project-specific harness PASS");
  return 0;
}
