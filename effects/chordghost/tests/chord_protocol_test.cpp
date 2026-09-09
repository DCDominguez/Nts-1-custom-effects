#include "../nts1/src/chord_table.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

static uint8_t normalized_to_cc7(float normalized) {
  if (normalized < 0.0f) normalized = 0.0f;
  if (normalized > 1.0f) normalized = 1.0f;
  int32_t code = static_cast<int32_t>(normalized * 127.0f + 0.5f);
  if (code < 0) code = 0;
  if (code > 127) code = 127;
  return static_cast<uint8_t>(code);
}

int main() {
  for (uint32_t code = 0u; code < 128u; ++code) {
    const float n = static_cast<float>(code) / 127.0f;
    assert(normalized_to_cc7(n) == code);
  }

  chordghost::ChordState st = {};
  for (uint32_t code = 0u; code < 120u; ++code) {
    chordghost::ChordState next = st;
    assert(chordghost::decode_chord_code(static_cast<uint8_t>(code), next));
    assert(next.code == code);
    assert(next.root == (code % 12u));
    assert(next.quality == (code / 12u));
    assert(!next.harmonic_bypass);
    assert((next.pitch_class_mask & (1u << next.root)) != 0u);
    st = next;
  }

  chordghost::ChordState bypass = st;
  assert(chordghost::decode_chord_code(120u, bypass));
  assert(bypass.harmonic_bypass);
  assert(bypass.pitch_class_mask == 0x0FFFu);

  const chordghost::ChordState held = st;
  chordghost::ChordState candidate = held;
  assert(!chordghost::decode_chord_code(121u, candidate));
  assert(candidate.code == held.code);
  for (uint32_t code = 122u; code < 128u; ++code) {
    candidate = held;
    assert(!chordghost::decode_chord_code(static_cast<uint8_t>(code), candidate));
    assert(candidate.code == held.code);
  }

  puts("CHORDGHOST M2 protocol test PASS");
  return 0;
}
