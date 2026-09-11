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
  MODFX_INIT(0,0);
  settle(1.0f, 0.0f);
  auto src = sine(8192, 997.0f, 0.35f);
  auto dry = process_mono(src);
  auto dryL = hs_measure::channel(dry, 0);
  require(hs_measure::max_abs_diff(src, dryL) < 2e-6, "DEPTH=0 is not effectively dry");

  auto count_transitions = [&](float rate) {
    MODFX_INIT(0,0);
    settle(rate, 1.0f);
    std::fprintf(stderr, "DUST settle target=%.3f s_rate=%.6f s_damage=%.6f capture=%.6f\n",
                 rate, s_rate, s_damage, capture_rate(s_rate));
    auto y = process_mono(deterministic_noise(32768));
    return hs_measure::transition_count(hs_measure::channel(y,0), 1e-5);
  };
  const std::size_t rate_lo = count_transitions(0.10f);
  const std::size_t rate_mid = count_transitions(0.55f);
  const std::size_t rate_hi = count_transitions(1.00f);
  std::fprintf(stderr, "DUST RATE transitions: low=%zu mid=%zu high=%zu\n", rate_lo, rate_mid, rate_hi);
  require(rate_lo > rate_mid * 1.20, "RATE low->mid did not reduce sample-hold cadence enough");
  require(rate_mid > rate_hi * 1.20, "RATE mid->max did not reduce sample-hold cadence enough");

  auto level_count = [&](float damage) {
    MODFX_INIT(0,0);
    settle(0.0f, damage);
    std::vector<float> ramp(32768);
    for (std::size_t i=0;i<ramp.size();++i) ramp[i] = -0.95f + 1.90f * static_cast<float>(i) / static_cast<float>(ramp.size()-1);
    auto y = process_mono(ramp);
    return hs_measure::approximate_unique_levels(hs_measure::channel(y,0), 2e-4);
  };
  const std::size_t dmg0 = level_count(0.0f);
  const std::size_t dmg50 = level_count(0.50f);
  const std::size_t dmg100 = level_count(1.0f);
  require(dmg0 > dmg50 * 1.5, "DAMAGE 0->50% did not measurably reduce resolution");
  require(dmg50 > dmg100 * 1.5, "DAMAGE 50->100% did not measurably reduce resolution");
  require(dmg100 >= 8 && dmg100 <= 20, "maximum DAMAGE does not resemble bounded low-bit quantization");

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

  MODFX_INIT(0,0);
  settle(0.0f, 0.8f);
  auto low = process_mono(sine(96000, 311.0f, 0.015f));
  const auto lowStats = hs_measure::stats(hs_measure::channel(low,0));
  require(std::fabs(lowStats.mean) < 0.0025, "low-level DAMAGE created excessive DC bias");

  MODFX_INIT(0,0);
  settle(1.0f,1.0f);
  (void)process_mono(sine(4096,220.0f,0.7f));
  MODFX_SUSPEND();
  MODFX_RESUME();
  std::vector<float> silence(8192,0.0f);
  auto silentOut = process_mono(silence);
  require(hs_measure::stats(hs_measure::channel(silentOut,0)).peak < 1e-7,
          "suspend/resume left stale held audio into silence");

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
