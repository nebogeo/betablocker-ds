// ----------------------------------------------------
// bbds arm9 synth (c) 2011 FoAM vzv GPLv3
// ----------------------------------------------------

#include <nds.h>

#ifndef QRZ
#define QRZ

extern "C"
{
    void qrz_white_noise(s16* dst, u32 clock, u32 length, u32 freq);
    u32  qrz_tabread(s16* dst, u32 length, u32 clk, u32 freq, s16 *tab);
    u32  qrz_tabread_lfo(s16* dst, u32 length, u32 clk, u32 freq, s16 *tab);
    u32  qrz_tabread_env(s16 *dst, u32 length, u32 clk, u32 speed, s16 *tab);
    u32  qrz_tabread_mod(s16* dst, u32 length, u32 clk, s16 *freq, s16 *tab);
    u32  qrz_maximus_inner(s16* dst, u32 length, u32 readpos, u32 writepos, u32 freq, s16 *tab);
    void qrz_thrsh(s16* dst, u32 length, s16 *src, s16 *thr);
}

#endif
