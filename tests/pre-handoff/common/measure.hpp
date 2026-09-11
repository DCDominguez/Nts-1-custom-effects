#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace hs_measure {

constexpr double kPi = 3.1415926535897932384626433832795;
constexpr double kSampleRate = 48000.0;

struct Stats {
  double mean = 0.0;
  double rms = 0.0;
  double peak = 0.0;
  double energy = 0.0;
};

inline Stats stats(const std::vector<float>& x, std::size_t start = 0,
                   std::size_t count = 0) {
  Stats s;
  if (start >= x.size()) return s;
  const std::size_t end = count == 0 ? x.size() : std::min(x.size(), start + count);
  if (end <= start) return s;
  double sum = 0.0;
  double e = 0.0;
  double p = 0.0;
  for (std::size_t i = start; i < end; ++i) {
    const double v = x[i];
    sum += v;
    e += v * v;
    p = std::max(p, std::fabs(v));
  }
  const double n = static_cast<double>(end - start);
  s.mean = sum / n;
  s.energy = e;
  s.rms = std::sqrt(e / n);
  s.peak = p;
  return s;
}

inline std::vector<float> channel(const std::vector<float>& interleaved,
                                  unsigned ch,
                                  unsigned channels = 2,
                                  std::size_t start_frame = 0,
                                  std::size_t frames = 0) {
  std::vector<float> out;
  if (channels == 0 || ch >= channels) return out;
  const std::size_t total_frames = interleaved.size() / channels;
  if (start_frame >= total_frames) return out;
  const std::size_t end = frames == 0 ? total_frames
                                      : std::min(total_frames, start_frame + frames);
  out.reserve(end - start_frame);
  for (std::size_t f = start_frame; f < end; ++f) out.push_back(interleaved[f * channels + ch]);
  return out;
}

inline double max_abs_diff(const std::vector<float>& a, const std::vector<float>& b) {
  const std::size_t n = std::min(a.size(), b.size());
  double m = 0.0;
  for (std::size_t i = 0; i < n; ++i) m = std::max(m, std::fabs(static_cast<double>(a[i]) - b[i]));
  return m;
}

inline double normalized_correlation(const std::vector<float>& a,
                                     const std::vector<float>& b) {
  const std::size_t n = std::min(a.size(), b.size());
  if (n == 0) return 0.0;
  double aa = 0.0, bb = 0.0, ab = 0.0;
  for (std::size_t i = 0; i < n; ++i) {
    aa += static_cast<double>(a[i]) * a[i];
    bb += static_cast<double>(b[i]) * b[i];
    ab += static_cast<double>(a[i]) * b[i];
  }
  if (aa <= 1e-30 || bb <= 1e-30) return 0.0;
  return ab / std::sqrt(aa * bb);
}

// Quadrature projection at an arbitrary frequency. Returns normalized power.
inline double tone_power(const std::vector<float>& x, double hz,
                         double sample_rate = kSampleRate,
                         std::size_t start = 0,
                         std::size_t count = 0) {
  if (start >= x.size() || hz <= 0.0) return 0.0;
  const std::size_t end = count == 0 ? x.size() : std::min(x.size(), start + count);
  const std::size_t n = end - start;
  if (n == 0) return 0.0;
  double c = 0.0, s = 0.0;
  const double w = 2.0 * kPi * hz / sample_rate;
  for (std::size_t i = 0; i < n; ++i) {
    const double phase = w * static_cast<double>(i);
    const double v = x[start + i];
    c += v * std::cos(phase);
    s += v * std::sin(phase);
  }
  return 4.0 * (c * c + s * s) / (static_cast<double>(n) * static_cast<double>(n));
}

// Positive-going zero-crossing frequency estimator. Best for near-periodic test signals.
inline double zero_cross_frequency(const std::vector<float>& x,
                                   double sample_rate = kSampleRate,
                                   std::size_t start = 0,
                                   std::size_t count = 0) {
  if (start >= x.size()) return 0.0;
  const std::size_t end = count == 0 ? x.size() : std::min(x.size(), start + count);
  std::vector<double> crossings;
  crossings.reserve((end - start) / 16 + 1);
  for (std::size_t i = start + 1; i < end; ++i) {
    const double a = x[i - 1];
    const double b = x[i];
    if (a < 0.0 && b >= 0.0 && b != a) {
      const double frac = -a / (b - a);
      crossings.push_back(static_cast<double>(i - 1) + frac);
    }
  }
  if (crossings.size() < 3) return 0.0;
  double period_sum = 0.0;
  for (std::size_t i = 1; i < crossings.size(); ++i) period_sum += crossings[i] - crossings[i - 1];
  const double period = period_sum / static_cast<double>(crossings.size() - 1);
  return period > 0.0 ? sample_rate / period : 0.0;
}

inline std::size_t transition_count(const std::vector<float>& x, double epsilon = 1e-6) {
  if (x.size() < 2) return 0;
  std::size_t n = 0;
  for (std::size_t i = 1; i < x.size(); ++i)
    if (std::fabs(static_cast<double>(x[i]) - x[i - 1]) > epsilon) ++n;
  return n;
}

inline std::size_t approximate_unique_levels(std::vector<float> x,
                                             double quant = 1e-4) {
  if (x.empty()) return 0;
  if (quant <= 0.0) quant = 1e-6;
  for (float& v : x) v = static_cast<float>(std::round(v / quant) * quant);
  std::sort(x.begin(), x.end());
  std::size_t levels = 1;
  for (std::size_t i = 1; i < x.size(); ++i)
    if (std::fabs(static_cast<double>(x[i]) - x[i - 1]) > quant * 0.5) ++levels;
  return levels;
}

inline double window_frequency_variance(const std::vector<float>& x,
                                        std::size_t window,
                                        double sample_rate = kSampleRate) {
  if (window < 64 || x.size() < window * 2) return 0.0;
  std::vector<double> f;
  for (std::size_t start = 0; start + window <= x.size(); start += window) {
    const double hz = zero_cross_frequency(x, sample_rate, start, window);
    if (hz > 0.0) f.push_back(hz);
  }
  if (f.size() < 2) return 0.0;
  double mean = 0.0;
  for (double v : f) mean += v;
  mean /= static_cast<double>(f.size());
  double var = 0.0;
  for (double v : f) { const double d = v - mean; var += d * d; }
  return var / static_cast<double>(f.size());
}

} // namespace hs_measure
