#pragma once

#include <stdint.h>

namespace chordghost {

struct ChordState {
  uint8_t code;
  uint8_t root;
  uint8_t quality;
  uint16_t pitch_class_mask;
  bool harmonic_bypass;
};

// Pitch-class masks are expressed relative to root.
// bit 0 = root, bit 1 = b2, ... bit 11 = major 7th.
static const uint16_t kQualityMasks[10] = {
  0x091, // major:      0, 4, 7
  0x089, // minor:      0, 3, 7
  0x891, // maj7:       0, 4, 7, 11
  0x489, // min7:       0, 3, 7, 10
  0x491, // dominant 7: 0, 4, 7, 10
  0x085, // sus2:       0, 2, 7
  0x0A1, // sus4:       0, 5, 7
  0x049, // diminished: 0, 3, 6
  0x111, // augmented:  0, 4, 8
  0x449  // min7b5:     0, 3, 6, 10
};

static inline uint16_t rotate12(uint16_t mask, uint8_t root) {
  mask &= 0x0FFFu;
  root %= 12u;
  if (root == 0u) {
    return mask;
  }
  return static_cast<uint16_t>(((mask << root) | (mask >> (12u - root))) & 0x0FFFu);
}

// Returns true when the caller should adopt the decoded state.
// 121 means HOLD and therefore returns false.
// 122..127 are reserved and also return false.
static inline bool decode_chord_code(uint8_t code, ChordState &state) {
  if (code == 121u || code >= 122u) {
    return false;
  }

  if (code == 120u) {
    state.code = code;
    state.root = 0xFFu;
    state.quality = 0xFFu;
    state.pitch_class_mask = 0x0FFFu;
    state.harmonic_bypass = true;
    return true;
  }

  const uint8_t quality = static_cast<uint8_t>(code / 12u);
  const uint8_t root = static_cast<uint8_t>(code % 12u);

  if (quality >= 10u) {
    return false;
  }

  state.code = code;
  state.root = root;
  state.quality = quality;
  state.pitch_class_mask = rotate12(kQualityMasks[quality], root);
  state.harmonic_bypass = false;
  return true;
}

} // namespace chordghost
