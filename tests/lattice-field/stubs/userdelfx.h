#pragma once
#include <stdint.h>
#define __sdram
static float test_bpm = 120.0f;
inline float fx_get_bpmf() { return test_bpm; }
inline float q31_to_f32(int32_t x) { return x / 2147483648.0f; }
enum { k_user_delfx_param_time, k_user_delfx_param_depth, k_user_delfx_param_reserved0, k_user_delfx_param_shift_depth };
