#include "userosc.h"

static float phi1 = 0.0f;
static float phi2 = 0.0f;

static float shape;
static uint8_t offset;

void OSC_INIT(uint32_t platform, uint32_t api) {
  (void)platform; (void)api;
}

void OSC_CYCLE(const user_osc_param_t * const params,
               int32_t *yn,
               const uint32_t frames) {
  const uint8_t note = (params->pitch) >> 8;
  const uint8_t mod  = (params->pitch) & 0x00FF;
  const float   w1   = osc_w0f_for_note(note, mod);
  const float   w2   = osc_w0f_for_note(note + offset, mod);
  const float   nidx = _osc_bl_par_idx((float)note);
  
  q31_t * __restrict y = (q31_t *)yn;
  const q31_t * y_e = y + frames;
  
  for (; y != y_e; ) {
    float sig = 0;
    sig += 0.7f * osc_sinf(osc_bl2_parf(phi1, nidx) * shape);
    sig += 0.3f * osc_sinf(phi1) * osc_sinf(phi2);
    
    phi1 += w1;
    phi1 -= (uint32_t)phi1;
    phi2 += w2;
    phi2 -= (uint32_t)phi2;
    
    *(y++) = f32_to_q31(sig);
  }
}

void OSC_NOTEON(const user_osc_param_t * const params) {
  (void)params;
}

void OSC_NOTEOFF(const user_osc_param_t * const params) {
  (void)params;
}

void OSC_PARAM(uint16_t idx, uint16_t val) { 
  switch (idx) {

  case k_user_osc_param_id1:
    break;

  case k_user_osc_param_shape:
    shape = param_val_to_f32(val) * 0.9f + 0.1f;
    break;

  case k_user_osc_param_shiftshape:
    offset = (uint8_t)(param_val_to_f32(val) * 36.0f);
    break;
  }
}
