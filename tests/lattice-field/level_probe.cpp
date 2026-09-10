// Compare the real production DSP at two pinned source revisions.
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#define LATTICE_TEST
#include FIELD_SOURCE
int main(int argc, char **argv) {
  if (argc != 3) return 2;
  const float amplitude = std::strtof(argv[1], 0);
  const bool overlap = std::atoi(argv[2]) != 0;
  DELFX_INIT(0,0);
  DELFX_PARAM(k_user_delfx_param_time, 1202590848); // .56 -> eighth-note clock
  DELFX_PARAM(k_user_delfx_param_depth, 214748364); // Forward
  DELFX_PARAM(k_user_delfx_param_shift_depth, 2147483647); // wet-only comparison
  DELFX_RESUME();
  double square = 0; float peak = 0, firstPeak = 0, minimumGuard = 1;
  unsigned clipped = 0;
  for (uint32_t n=0;n<240000u;++n) {
    const unsigned local = overlap ? n % 9600u : n;
    const bool soundingInput = local < 6000u && (!overlap || n<96000u);
    const float x = soundingInput ? amplitude*std::sin(6.28318530718f*220.f*n/48000.f) : 0;
    float a[2]={x,x}; DELFX_PROCESS(a,1);
    if (!std::isfinite(a[0]) || !std::isfinite(a[1])) return 3;
    for(int c=0;c<2;++c) {
      const float p=std::fabs(a[c]);
      if(p>peak)peak=p;
      if(n<22000u && p>firstPeak)firstPeak=p;
      if(p>=.99999f)++clipped;
      square+=a[c]*a[c];
    }
    if(wetGuard<minimumGuard)minimumGuard=wetGuard;
  }
  // The network must remain bit-identical: the boost is after its injection.
  uint32_t hash=2166136261u;
  for(unsigned c=0;c<4;++c)for(unsigned n=0;n<FDN;++n){
    uint32_t bits;std::memcpy(&bits,&field[c][n],4);hash=(hash^bits)*16777619u;
  }
  std::printf("%.9f %.9f %.9f %.9f %u %u %u %u %u\n",peak,firstPeak,
      std::sqrt(square/480000.),minimumGuard,guardHits,clipped,admitted,spawned,hash);
}
