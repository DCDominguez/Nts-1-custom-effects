# RAINFALL Spec
Four smoothed stochastic read heads share a stereo buffer. TIME controls how often a head receives a new target; DEPTH widens delay range, stereo scatter and feedback. MIX uses `k_user_delfx_param_shift_depth`. Deterministic PRNG makes failures reproducible from power-on.
