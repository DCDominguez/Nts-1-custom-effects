#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include "tests/pre-handoff/common/measure.hpp"
#include "oscillators/spectra/nts1/src/spectra.cpp"

static void require(bool ok, const char* message) {
  if (!ok) {
    std::fprintf(stderr, "SPECTRA FAIL: %s\n", message);
    std::exit(2);
  }
}

static void base_patch(uint8_t voices = 1) {
  OSC_INIT(0, 0);
  OSC_PARAM(k_user_osc_param_id1, voices - 1u); // 0..2 => 1..3 voices
  OSC_PARAM(k_user_osc_param_id2, 0);           // Spread
  OSC_PARAM(k_user_osc_param_id3, 0);           // Drift
  OSC_PARAM(k_user_osc_param_id4, 0);           // HarmMode UNISON
  OSC_PARAM(k_user_osc_param_id5, 0);           // Motion
  OSC_PARAM(k_user_osc_param_id6, 0);           // Chaos
  OSC_PARAM(k_user_osc_param_shape, 0);         // sine endpoint
  OSC_PARAM(k_user_osc_param_shiftshape, 0);    // ALT
}

static std::vector<float> render(uint8_t note, std::size_t frames, bool note_on = true) {
  user_osc_param_t p{};
  p.pitch = static_cast<uint16_t>(note) << 8;
  p.shape_lfo = 0;
  if (note_on) OSC_NOTEON(&p);
  std::vector<float> out;
  out.reserve(frames);
  const uint32_t block = 64;
  std::vector<int32_t> y(block);
  std::size_t left = frames;
  while (left) {
    const uint32_t n = static_cast<uint32_t>(std::min<std::size_t>(block, left));
    OSC_CYCLE(&p, y.data(), n);
    for (uint32_t i = 0; i < n; ++i) {
      const float v = static_cast<float>(y[i]) / 2147483648.0f;
      require(std::isfinite(v), "non-finite oscillator output");
      require(std::fabs(v) <= 1.001f, "oscillator exceeded q31 output range");
      out.push_back(v);
    }
    left -= n;
  }
  return out;
}

static double adjacent_window_delta(const std::vector<float>& x, std::size_t window) {
  std::vector<double> f;
  for (std::size_t start = 0; start + window <= x.size(); start += window) {
    const double hz = hs_measure::zero_cross_frequency(x, 48000.0, start, window);
    if (hz > 0.0) f.push_back(hz);
  }
  if (f.size() < 2) return 0.0;
  double d = 0.0;
  for (std::size_t i = 1; i < f.size(); ++i) d += std::fabs(f[i] - f[i - 1]);
  return d / static_cast<double>(f.size() - 1);
}

int main() {
  // Hardware-derived contract: SPECTRA sounded excellent at 1-3 voices on the
  // physical MkI, while the fourth voice degraded the result. Values above the
  // published range must therefore hard-clamp to three voices.
  OSC_INIT(0, 0);
  OSC_PARAM(k_user_osc_param_id1, 3);
  require(s.voices == 3u, "Voices parameter exceeded the three-voice hardware-approved cap");

  // Pitch contract: single-voice sine should track equal-tempered test notes.
  base_patch(1);
  for (const auto& test : std::vector<std::pair<uint8_t, double>>{{57,220.0},{69,440.0},{81,880.0}}) {
    const auto x = render(test.first, 48000);
    const double hz = hs_measure::zero_cross_frequency(x, 48000.0, 2048, x.size() - 2048);
    require(std::fabs(hz - test.second) / test.second < 0.012, "single-voice pitch tracking outside 1.2% tolerance");
  }

  // SHAPE contract: square endpoint must add substantially more 3rd-harmonic energy than sine.
  base_patch(1);
  auto sine = render(69, 96000);
  const double sine1 = hs_measure::tone_power(sine, 440.0, 48000.0, 4096);
  const double sine3 = hs_measure::tone_power(sine, 1320.0, 48000.0, 4096);
  OSC_PARAM(k_user_osc_param_shape, 1023);
  auto square = render(69, 96000);
  const double square1 = hs_measure::tone_power(square, 440.0, 48000.0, 4096);
  const double square3 = hs_measure::tone_power(square, 1320.0, 48000.0, 4096);
  require(sine1 > 1e-4 && square1 > 1e-4, "SHAPE endpoints lost fundamental delivery");
  require(square3 / square1 > std::max(0.015, (sine3 / sine1) * 2.5), "SHAPE square endpoint lacks expected harmonic enrichment");

  // Voices + Spread contract: one voice ignores multi-voice spread geometry; three voices create symmetric detuned energy.
  base_patch(1);
  OSC_PARAM(k_user_osc_param_id2, 100);
  auto one_spread = render(69, 96000);
  const double lower = 440.0 * std::pow(2.0, -20.0 / 1200.0);
  const double upper = 440.0 * std::pow(2.0,  20.0 / 1200.0);
  const double one_side = hs_measure::tone_power(one_spread, lower, 48000.0, 4096) +
                          hs_measure::tone_power(one_spread, upper, 48000.0, 4096);
  base_patch(3);
  OSC_PARAM(k_user_osc_param_id2, 100);
  auto three_spread = render(69, 96000);
  const double three_side = hs_measure::tone_power(three_spread, lower, 48000.0, 4096) +
                            hs_measure::tone_power(three_spread, upper, 48000.0, 4096);
  require(three_side > one_side * 3.0 + 1e-5, "Voices/Spread did not create expected detuned side energy");

  // HarmMode + ALT contract: mode 1 at full ALT must deliver root, fifth and octave components.
  base_patch(3);
  OSC_PARAM(k_user_osc_param_id4, 1);
  OSC_PARAM(k_user_osc_param_shiftshape, 1023);
  auto harmony = render(69, 144000);
  const double p_root = hs_measure::tone_power(harmony, 440.0, 48000.0, 4096);
  const double p_fifth = hs_measure::tone_power(harmony, 440.0 * std::pow(2.0, 7.0/12.0), 48000.0, 4096);
  const double p_oct = hs_measure::tone_power(harmony, 880.0, 48000.0, 4096);
  require(p_root > 1e-4 && p_fifth > 1e-4 && p_oct > 1e-4,
          "HarmMode/ALT failed to deliver one or more specified interval voices");

  // Drift contract: zero drift remains substantially steadier than maximum drift.
  base_patch(1);
  auto steady = render(69, 384000);
  const double steady_var = hs_measure::window_frequency_variance(steady, 12000);
  base_patch(1);
  OSC_PARAM(k_user_osc_param_id3, 100);
  OSC_PARAM(k_user_osc_param_id5, 50);
  auto drifting = render(69, 384000);
  const double drift_var = hs_measure::window_frequency_variance(drifting, 12000);
  require(drift_var > steady_var + 0.02, "Drift control did not create measurable bounded pitch wander");
  require(hs_measure::stats(drifting).peak <= 1.001, "maximum Drift violated output bound");

  // Motion contract: with Drift held high, Motion must increase the rate of pitch movement, not its gross level.
  base_patch(1);
  OSC_PARAM(k_user_osc_param_id3, 100);
  OSC_PARAM(k_user_osc_param_id5, 0);
  auto slow = render(69, 480000);
  const double slow_delta = adjacent_window_delta(slow, 12000);
  base_patch(1);
  OSC_PARAM(k_user_osc_param_id3, 100);
  OSC_PARAM(k_user_osc_param_id5, 100);
  auto fast = render(69, 480000);
  const double fast_delta = adjacent_window_delta(fast, 12000);
  require(fast_delta > slow_delta * 1.35 + 0.005, "Motion did not measurably increase drift speed");

  // Chaos contract: compare independent identical patches so default Drift/Motion slew history cannot contaminate the seed test.
  base_patch(1);
  osc_rng_state = 0x12345678u;
  auto clean_a = render(69, 24000);
  base_patch(1);
  osc_rng_state = 0x87654321u;
  auto clean_b = render(69, 24000);
  require(hs_measure::max_abs_diff(clean_a, clean_b) < 1e-5, "Chaos=0 depends on random seed");

  base_patch(3);
  OSC_PARAM(k_user_osc_param_id6, 100);
  osc_rng_state = 0x12345678u;
  auto chaos_a = render(69, 24000);
  base_patch(3);
  OSC_PARAM(k_user_osc_param_id6, 100);
  osc_rng_state = 0x87654321u;
  auto chaos_b = render(69, 24000);
  require(hs_measure::max_abs_diff(chaos_a, chaos_b) > 1e-4, "Chaos=100 failed to alter note-on variation");
  const auto cs = hs_measure::stats(chaos_a);
  require(cs.rms > 1e-4 && cs.peak <= 1.001, "Chaos output lost delivery or bound");

  std::puts("SPECTRA A-class project-specific harness PASS");
  return 0;
}
