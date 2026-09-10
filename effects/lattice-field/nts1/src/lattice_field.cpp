#include "userdelfx.h"
#include <stdint.h>

// FIELD 0.1-0: two owned captures, finite event tables, one shared bloom.
// No allocation, moving delay heads, or wet-to-capture routing.
namespace {
const uint32_t SR = 48000u;
const uint32_t SEEDS = 2u, VOICES = 8u, FRAGMENT = 4096u;
const uint32_t HISTORY = 512u, PREROLL = 256u, FADE = 96u;
const uint32_t FDN = 4096u, DIFF = 512u;
__sdram float capture[SEEDS][2][FRAGMENT];
__sdram float history[2][HISTORY];
__sdram float field[4][FDN];
__sdram float diffusion[2][DIFF];

enum State { Empty, Capturing, Playing, Retiring };
struct Seed {
  State state;
  uint32_t id, born, deadline, tick, length, written, next, event;
  uint32_t mode, limit, retire;
  bool replace;
};
struct Voice {
  bool active, ghost, reverse;
  uint32_t seed, id, age, length, fade, born;
  float position, ratio, level, panL, panR, send, lpL, lpR;
};
Seed seeds[SEEDS];
Voice voices[VOICES];
uint32_t now, historyWrite, fieldWrite, diffWrite, serial;
uint32_t refractory, quietSamples, samplesSinceSend;
float fastEnv, slowEnv;
bool attackArmed, gateArmed;
float clockTarget = 0.56f, modeTarget = 0.10f, mixTarget = 0.50f;
float mixValue = 0.50f, foregroundNorm = 1.0f, ghostNorm = 1.0f, sendNorm = 1.0f;
float wetGuard = 1.0f, bodyLP[2], damping[4], dcIn[4], dcOut[4];
float motion, bloomGain = 0.72f;
#ifdef LATTICE_TEST
uint32_t admitted, spawned, maxLive, stolen, guardHits, faults;
#endif

inline float absolute(float x) { return x < 0.0f ? -x : x; }
inline float maximum(float a, float b) { return a > b ? a : b; }
inline float clamp(float x, float lo, float hi) { return x < lo ? lo : (x > hi ? hi : x); }
inline bool finite(float x) {
  union { float f; uint32_t u; } bits;
  bits.f = x;
  return (bits.u & 0x7f800000u) != 0x7f800000u;
}
inline float clean(float x) { return finite(x) ? clamp(x, -8.0f, 8.0f) : 0.0f; }
inline bool due(uint32_t deadline) { return static_cast<int32_t>(now - deadline) >= 0; }
inline uint32_t zone(float x, uint32_t n) { return static_cast<uint32_t>(clamp(x, 0.0f, 1.0f) * (n - 0.001f)); }
inline uint32_t tickFor(uint32_t division) {
  static const float beats[8] = {0.125f, 1.0f/6.0f, 0.25f, 1.0f/3.0f, 0.5f, 2.0f/3.0f, 1.0f, 2.0f};
  float bpm = fx_get_bpmf();
  if (!finite(bpm) || bpm < 30.0f || bpm > 300.0f) bpm = 120.0f;
  return static_cast<uint32_t>((60.0f * SR / bpm) * beats[division]);
}
inline uint32_t limitFor(uint32_t division) { return division < 2u ? 4u : 8u; }

bool hasVoice(uint32_t slot) {
  for (uint32_t i = 0; i < VOICES; ++i)
    if (voices[i].active && voices[i].seed == slot) return true;
  return false;
}
void retire(uint32_t slot, bool replace) {
  Seed &s = seeds[slot];
  s.state = Retiring; s.replace = replace; s.retire = FADE;
  for (uint32_t i = 0; i < VOICES; ++i)
    if (voices[i].active && voices[i].seed == slot) voices[i].fade = FADE;
}
void startCapture(uint32_t slot) {
  Seed &s = seeds[slot];
  s.state = Capturing; s.id = ++serial; s.born = now;
  const uint32_t division = zone(clockTarget, 8u);
  s.tick = tickFor(division); s.limit = limitFor(division);
  s.mode = zone(modeTarget, 4u);
  s.length = s.tick / 2u;
  if (s.length < 1024u) s.length = 1024u;
  if (s.length > FRAGMENT) s.length = FRAGMENT;
  s.written = s.event = 0u;
  // Snapshot at admission: knob/tempo changes cannot extend this deadline.
  s.deadline = now + 10u * s.tick + FRAGMENT;
  s.replace = false;
#ifdef LATTICE_TEST
  ++admitted;
#endif
}
void admit() {
  for (uint32_t i = 0; i < SEEDS; ++i) {
    if (seeds[i].state == Empty) { startCapture(i); return; }
  }
  // Do not repeatedly restart retirement while an old capture is draining.
  if (seeds[0].state == Retiring || seeds[1].state == Retiring) return;
  const uint32_t older = (now - seeds[0].born >= now - seeds[1].born) ? 0u : 1u;
  retire(older, true);
}
uint32_t eventOffset(const Seed &s, uint32_t event) {
  // Six audible events: four clear answers, fifth/octave ghosts.
  static const uint8_t grid[4][6] = {
    {4, 8, 12, 16, 24, 32},
    {4, 12, 16, 24, 28, 32},
    {4, 8, 12, 16, 24, 32},
    {4, 5, 6, 8, 12, 16}
  };
  return (s.tick * grid[s.mode][event]) / 4u;
}
uint32_t liveCount() {
  uint32_t count = 0;
  for (uint32_t i = 0; i < VOICES; ++i) if (voices[i].active) ++count;
  return count;
}
// All eight slots are always serviced. Budget changes never strand old voices.
// Capacity replacement fades a victim; the new foreground waits for that fade.
int voiceFor(bool ghost, uint32_t limit) {
  const uint32_t live = liveCount();
  if (live < limit) {
    for (uint32_t i = 0; i < VOICES; ++i) if (!voices[i].active) return static_cast<int>(i);
  }
  if (ghost) return -1;
  int victim = -1;
  for (uint32_t i = 0; i < VOICES; ++i) {
    const Voice &v = voices[i];
    if (!v.active) continue;
    if (v.fade) return -2; // retirement is already making room
    if (victim < 0 || (v.ghost && !voices[victim].ghost) ||
        (v.ghost == voices[victim].ghost && v.level < voices[victim].level)) victim = static_cast<int>(i);
  }
  if (victim >= 0) {
    voices[victim].fade = FADE;
#ifdef LATTICE_TEST
    ++stolen;
#endif
  }
  return -2;
}
bool spawn(uint32_t slot) {
  Seed &s = seeds[slot];
  const uint32_t e = s.event;
  const bool ghost = e >= 4u;
  const int index = voiceFor(ghost, s.limit);
  if (index == -2) return false;
  if (index == -1) return true; // consume an omitted ghost, never postpone expiry
  Voice &v = voices[index];
  v = Voice(); v.active = true; v.ghost = ghost; v.seed = slot; v.id = s.id; v.born = now;
  v.ratio = e == 4u ? 1.49830708f : (e == 5u ? 2.0f : 1.0f);
  v.reverse = s.mode == 1u;
  uint32_t sourceLength = s.length;
  if (s.mode == 3u && sourceLength > s.tick / 6u) sourceLength = s.tick / 6u;
  if (sourceLength < 192u) sourceLength = 192u;
  if (sourceLength > s.length) sourceLength = s.length;
  v.length = static_cast<uint32_t>((sourceLength - 1u) / v.ratio);
  v.position = v.reverse ? static_cast<float>(sourceLength - 1u) : 0.0f;
  static const float levels[6] = {1.0f, 0.78f, 0.61f, 0.46f, 0.14f, 0.09f};
  static const float sends[6] = {0.18f, 0.30f, 0.48f, 0.72f, 1.10f, 1.35f};
  v.level = levels[e]; v.send = sends[e];
  float pan = s.mode == 2u ? ((e & 1u) ? 0.82f : -0.82f) : ((e & 1u) ? 0.18f : -0.18f);
  if (ghost) pan = (e & 1u) ? 0.65f : -0.65f;
  // Stereo balance, not mono summing: the source and center survive.
  v.panL = pan > 0.0f ? 1.0f - pan : 1.0f;
  v.panR = pan < 0.0f ? 1.0f + pan : 1.0f;
#ifdef LATTICE_TEST
  ++spawned;
  if (liveCount() > maxLive) maxLive = liveCount();
#endif
  return true;
}
void serviceSeeds() {
  for (uint32_t i = 0; i < SEEDS; ++i) {
    Seed &s = seeds[i];
    if (s.state == Retiring) {
      if (s.retire) --s.retire;
      if (!s.retire && !hasVoice(i)) {
        if (s.replace) startCapture(i);
        else s.state = Empty;
      }
    } else if (s.state == Capturing) {
      const uint32_t read = (historyWrite - PREROLL) & (HISTORY - 1u);
      capture[i][0][s.written] = history[0][read];
      capture[i][1][s.written] = history[1][read];
      if (++s.written == s.length) {
        s.state = Playing; s.next = s.born + s.length + eventOffset(s, 0u);
      }
    } else if (s.state == Playing) {
      if (due(s.deadline)) { retire(i, false); continue; }
      if (s.event < 6u && due(s.next)) {
        if (spawn(i)) {
          if (++s.event < 6u) s.next = s.born + s.length + eventOffset(s, s.event);
        }
      }
      if (s.event == 6u && !hasVoice(i)) s.state = Empty;
    }
  }
}
inline float interpolated(uint32_t slot, uint32_t channel, float p, uint32_t length) {
  p = clamp(p, 0.0f, static_cast<float>(length - 1u));
  const uint32_t i = static_cast<uint32_t>(p);
  const uint32_t j = i + 1u < length ? i + 1u : i;
  return capture[slot][channel][i] + (capture[slot][channel][j] - capture[slot][channel][i]) * (p - i);
}
inline float allpass(float x, uint32_t channel, uint32_t delay) {
  const float d = diffusion[channel][(diffWrite - delay) & (DIFF - 1u)];
  const float y = d - 0.55f * x;
  diffusion[channel][diffWrite] = x + 0.55f * y;
  return y;
}
void bloom(float l, float r, float &outL, float &outR) {
  const bool resting = samplesSinceSend >= 8u * SR;
  if (resting) {
    // Clear lazily at silence: no large audio-callback memset or residual state.
    for (uint32_t i = 0; i < 4; ++i) {
      field[i][fieldWrite] = 0.0f;
      damping[i] = dcIn[i] = dcOut[i] = 0.0f;
    }
    diffusion[0][diffWrite] = diffusion[1][diffWrite] = 0.0f;
    bodyLP[0] = bodyLP[1] = 0.0f;
    outL = outR = 0.0f;
  } else {
    const float a = allpass(l, 0u, 347u), b = allpass(r, 1u, 421u);
    static const uint32_t lengths[4] = {1493u, 2111u, 2971u, 3797u};
    float d[4];
    for (uint32_t i = 0; i < 4; ++i) {
      const float x = field[i][(fieldWrite - lengths[i]) & (FDN - 1u)];
      damping[i] += (x - damping[i]) * 0.38f;
      const float hp = 0.9975f * (damping[i] - dcIn[i]) + 0.995f * dcOut[i];
      dcIn[i] = damping[i]; dcOut[i] = hp; d[i] = hp;
    }
    const float h[4] = {0.5f*(d[0]+d[1]+d[2]+d[3]), 0.5f*(d[0]-d[1]+d[2]-d[3]),
                        0.5f*(d[0]+d[1]-d[2]-d[3]), 0.5f*(d[0]-d[1]-d[2]+d[3])};
    const float injection[4] = {a * 0.32f, b * 0.32f, a * 0.22f, -b * 0.22f};
    for (uint32_t i = 0; i < 4; ++i) {
      const float x = injection[i] + h[i] * 0.82f;
      field[i][fieldWrite] = finite(x) ? clamp(x, -2.0f, 2.0f) : 0.0f;
    }
    const float tailL = 0.55f * (d[0]+d[2]) + 0.13f * (d[1]-d[3]);
    const float tailR = 0.55f * (d[1]+d[3]) + 0.13f * (d[2]-d[0]);
    bodyLP[0] += (tailL - bodyLP[0]) * 0.10f;
    bodyLP[1] += (tailR - bodyLP[1]) * 0.10f;
    motion += 0.035f / SR; if (motion >= 1.0f) motion -= 1.0f;
    const float sway = (motion < 0.5f ? motion * 4.0f - 1.0f : 3.0f - motion * 4.0f) * 0.035f;
    // HALO uses early diffuse high frequencies; BODY uses the longer shared tail.
    float decay = 1.0f;
    if (samplesSinceSend > 6u * SR) decay = (8u * SR - samplesSinceSend) / (2.0f * SR);
    outL = (tailL * 0.78f + bodyLP[0] * 0.30f + (a - bodyLP[0]) * (0.16f + sway)) * decay;
    outR = (tailR * 0.78f + bodyLP[1] * 0.30f + (b - bodyLP[1]) * (0.16f - sway)) * decay;
  }
  fieldWrite = (fieldWrite + 1u) & (FDN - 1u);
  diffWrite = (diffWrite + 1u) & (DIFF - 1u);
}
void reset() {
  for (uint32_t s = 0; s < SEEDS; ++s) {
    seeds[s] = Seed();
    for (uint32_t c = 0; c < 2; ++c) for (uint32_t j = 0; j < FRAGMENT; ++j) capture[s][c][j] = 0;
  }
  for (uint32_t i = 0; i < VOICES; ++i) voices[i] = Voice();
  for (uint32_t c = 0; c < 2; ++c) {
    for (uint32_t j = 0; j < HISTORY; ++j) history[c][j] = 0;
    for (uint32_t j = 0; j < DIFF; ++j) diffusion[c][j] = 0;
    bodyLP[c] = 0;
  }
  for (uint32_t c = 0; c < 4; ++c) {
    for (uint32_t j = 0; j < FDN; ++j) field[c][j] = 0;
    damping[c] = dcIn[c] = dcOut[c] = 0;
  }
  now = historyWrite = fieldWrite = diffWrite = serial = refractory = quietSamples = 0;
  samplesSinceSend = 8u * SR; fastEnv = slowEnv = motion = 0; bloomGain = 0.72f;
  attackArmed = gateArmed = true;
  mixValue = mixTarget;
  foregroundNorm = ghostNorm = sendNorm = wetGuard = 1.0f;
#ifdef LATTICE_TEST
  admitted = spawned = maxLive = stolen = guardHits = faults = 0;
#endif
}
} // namespace

void DELFX_INIT(uint32_t platform, uint32_t api) {
  (void)platform; (void)api;
  clockTarget = 0.56f; modeTarget = 0.10f; mixTarget = 0.50f; reset();
}
void DELFX_SUSPEND(void) { reset(); }
void DELFX_RESUME(void) { reset(); }
void DELFX_PARAM(uint8_t index, int32_t value) {
  const float p = clamp(q31_to_f32(value), 0.0f, 1.0f);
  if (index == k_user_delfx_param_time) clockTarget = p;
  else if (index == k_user_delfx_param_depth) modeTarget = p;
  else if (index == k_user_delfx_param_shift_depth) mixTarget = p;
}
void DELFX_PROCESS(float *xn, uint32_t frames) {
  for (uint32_t f = 0; f < frames; ++f, ++now) {
    const float inL = clean(xn[2u*f]), inR = clean(xn[2u*f+1u]);
    history[0][historyWrite] = inL; history[1][historyWrite] = inR;
    const float energy = 0.5f * (absolute(inL) + absolute(inR));
    fastEnv += (energy-fastEnv) * 0.002f;
    slowEnv += (energy-slowEnv) * 0.00015f;
    if (refractory) --refractory;
    if (fastEnv < 0.003f) {
      if (quietSamples < 960u) ++quietSamples;
      if (quietSamples >= 960u) gateArmed = true;
    } else quietSamples = 0;
    if (fastEnv < slowEnv * 1.15f + 0.003f) attackArmed = true;
    const bool attack = attackArmed && fastEnv > slowEnv * 1.8f + 0.006f;
    if (!refractory && fastEnv > 0.008f && (attack || gateArmed)) {
      admit(); refractory = 5760u; attackArmed = gateArmed = false;
    }
    serviceSeeds();
    historyWrite = (historyWrite + 1u) & (HISTORY - 1u);
    float frontL=0, frontR=0, ghostL=0, ghostR=0, sendL=0, sendR=0;
    uint32_t fronts=0, ghosts=0, sounding=0;
    for (uint32_t i = 0; i < VOICES; ++i) {
      Voice &v = voices[i]; if (!v.active) continue;
      if (v.seed >= SEEDS || v.id != seeds[v.seed].id || !finite(v.position) || !finite(v.ratio)) {
        v.active = false;
#ifdef LATTICE_TEST
        ++faults;
#endif
        continue;
      }
      const Seed &s = seeds[v.seed];
      const float rawL = interpolated(v.seed, 0u, v.position, s.length);
      const float rawR = interpolated(v.seed, 1u, v.position, s.length);
      if (!finite(rawL) || !finite(rawR)) { v.active = false; continue; }
      const uint32_t remain = v.length > v.age ? v.length - v.age : 0u;
      float envelope = clamp(v.age / 96.0f, 0.0f, 1.0f) * clamp(remain / 96.0f, 0.0f, 1.0f);
      if (v.fade) { envelope *= v.fade / 96.0f; if (--v.fade == 0u) v.active = false; }
      float l = rawL, r = rawR;
      if (v.ghost) {
        v.lpL += (l-v.lpL)*0.45f; v.lpR += (r-v.lpR)*0.45f;
        l = v.lpL; r = v.lpR; // foreground is not low-pass filtered
      }
      l *= envelope * v.level * v.panL; r *= envelope * v.level * v.panR;
      if (v.ghost) { ghostL+=l; ghostR+=r; ++ghosts; }
      else { frontL+=l; frontR+=r; ++fronts; }
      sendL += l * v.send; sendR += r * v.send; ++sounding;
      v.position += v.reverse ? -v.ratio : v.ratio;
      if (++v.age >= v.length) v.active = false;
    }
    static const float reciprocal[9] = {1, 1, .5f, 1.0f/3, .25f, .2f, 1.0f/6, 1.0f/7, .125f};
    const float nf = reciprocal[fronts];
    const float ng = reciprocal[ghosts];
    const float ns = reciprocal[sounding];
    foregroundNorm += (nf-foregroundNorm)*0.01f;
    ghostNorm += (ng-ghostNorm)*0.01f; sendNorm += (ns-sendNorm)*0.01f;
    if (sounding) samplesSinceSend = 0;
    else if (samplesSinceSend < 9u * SR) ++samplesSinceSend;
    float bloomL, bloomR;
    bloom(sendL*sendNorm, sendR*sendNorm, bloomL, bloomR);
    // Lower bloom during the foreground, without ducking the dry source.
    bloomGain += ((fronts ? 0.38f : 0.72f)-bloomGain)*0.001f;
    float wetL = frontL*foregroundNorm + ghostL*ghostNorm + bloomL*bloomGain;
    float wetR = frontR*foregroundNorm + ghostR*ghostNorm + bloomR*bloomGain;
    wetL = clean(wetL); wetR = clean(wetR);
    const float peak = maximum(absolute(wetL), absolute(wetR));
    const float desired = peak > 0.90f ? 0.90f / peak : 1.0f;
    if (desired < wetGuard) {
      wetGuard = desired;
#ifdef LATTICE_TEST
      ++guardHits;
#endif
    } else wetGuard += (desired-wetGuard)*0.0004f;
    mixValue += (mixTarget-mixValue)*0.0015f;
    // Mid MIX: dry .5, main answer .4 before pan; high MIX reaches wet-only.
    const float dry = 1.0f-mixValue, wet = 0.8f*mixValue*wetGuard;
    xn[2u*f] = clamp(inL*dry + wetL*wet, -1.0f, 1.0f);
    xn[2u*f+1u] = clamp(inR*dry + wetR*wet, -1.0f, 1.0f);
  }
}
