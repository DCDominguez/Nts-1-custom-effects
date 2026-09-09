#include "usermodfx.h"
#include <stdint.h>
namespace {
static constexpr uint32_t N=1024u; static constexpr float SR=48000.f, PI2=6.28318530718f;
static float bL[N] __sdram, bR[N] __sdram; static uint32_t w=0u; static float ph=0.f;
static float rt=.2f,dt=.4f,r=.2f,d=.4f;
static inline float c01(float x){return x<0?0:(x>1?1:x);} static inline float ca(float x){return x<-1?-1:(x>1?1:x);} static inline float wrap(float x){while(x>=1)x-=1;while(x<0)x+=1;return x;}
static inline float rd(float *b,float delay){float p=(float)w-delay;while(p<0)p+=N;uint32_t i0=(uint32_t)p&(N-1u),i1=(i0+1u)&(N-1u);float f=p-(uint32_t)p;return b[i0]+(b[i1]-b[i0])*f;}
static void reset(){for(uint32_t i=0;i<N;++i)bL[i]=bR[i]=0;w=0;ph=0;}
}
void MODFX_INIT(uint32_t p,uint32_t a){(void)p;(void)a;reset();}
void MODFX_PROCESS(const float *x,float *y,const float *sx,float *sy,uint32_t frames){(void)sx;(void)sy;for(uint32_t i=0;i<frames;++i){r+=(rt-r)*.0015f;d+=(dt-d)*.0015f;float hz=.04f+1.8f*r*r;ph=wrap(ph+hz/SR);float lfoL=fx_sinf(ph),lfoR=fx_sinf(wrap(ph+.18f*d));float ref=170.f,span=145.f*d;float wetL=rd(bL,ref+span*lfoL),wetR=rd(bR,ref+span*lfoR),dryL=rd(bL,ref),dryR=rd(bR,ref);float fb=.62f*d;float inL=x[2*i],inR=x[2*i+1];bL[w]=ca(inL+wetL*fb);bR[w]=ca(inR+wetR*fb);y[2*i]=ca(.72f*(dryL+wetL));y[2*i+1]=ca(.72f*(dryR+wetR));w=(w+1u)&(N-1u);}}
void MODFX_SUSPEND(){reset();} void MODFX_RESUME(){reset();}
void MODFX_PARAM(uint8_t i,int32_t v){float n=c01(q31_to_f32(v));if(i==k_user_modfx_param_time)rt=n;else if(i==k_user_modfx_param_depth)dt=n;}
