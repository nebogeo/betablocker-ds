// Copyright (C) 2010 Dave Griffiths
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#include <nds.h>
#include <maxmod9.h>
#include "qrz_instruments.h"
#include "tuning.h"

#ifndef BB_SOUND
#define BB_SOUND

#define SOUND_BANK_SIZE 8

#define WAVETABLE_SIZE 512
#define AUDIO_SCALE 0x3fff
#define FX_SHIFT 10

class qrz_ambference;
class qrz_melocore;
class qrz_pigeon;
class qrz_azid;
class qrz_crunch;
class qrz_maximus;
class qrz_ticklish;
class qrz_glitchiference;

class sound
{
public:
    sound();
    ~sound();

    void load_bank(u32 b);
    void load(u32 id);
    void unload(u32 id);
    void play(u32 id, u32 freq);
    u32 bank() { return m_current_bank; }
    void update();

private:
    void test();
    static void audio_test(s16 *buf, u32 len);

    void set_nthrt(u32 n);
    float note_to_freq(u32 note);
    static mm_word on_stream_request( mm_word length, mm_addr dest, mm_stream_formats format );

    mm_sound_effect m_sounds[SOUND_BANK_SIZE];
    float m_nthrt;
    static u32 m_current_bank;

    static qrz_ambference *m_ambference;
    static qrz_melocore *m_melocore;
    static qrz_pigeon *m_pigeon;
    static qrz_azid *m_azid;
    static qrz_crunch *m_crunch;
    static qrz_maximus *m_maximus;
    static qrz_ticklish *m_ticklish;
    static qrz_glitchiference *m_glitchiference;
};

/////////////////////////////////////////////////////////
// low level stuff

void qrz_wavetable_init();
void qrz_init_skiplut(); 
u32 qrz_note_to_skip(u32 n);

// fixed point
s32 fx(s32 i);
s32 ffx(float i);
s32 fromfx(s32 i);
s32 fxmul(s32 a, s32 b);
s32 fxdiv(s32 a, s32 b);

// too boring/hard for asm
void qrz_mul(s16* dst, s16 *a, s16 *b, u32 length);
void qrz_mul(s16* dst, s16 *a, s32 b, u32 length);
void qrz_add(s16* dst, s16 *a, s16 *b, u32 length);
void qrz_add(s16* dst, s16 *a, s16 b, u32 length);
void qrz_audio2cv(s16* dst, s16 *src, u32 length);
void qrz_cv2audio(s16* dst, s16 *src, u32 length);

void qrz_lpf(s16* dst, s16* src, u32 length, s16 f, s16 q, s16 *s);
void qrz_lpf_modf(s16* dst, s16* src, u32 length, s16* f, s16 q, s16 *s);
void qrz_lpf_modfq(s16* dst, s16* src, u32 length, s16* f, s16* q, s16 *s);
void qrz_bpf(s16* dst, s16* src, u32 length, s16 f, s16 q, s16 *s);
void qrz_bpf_modf(s16* dst, s16* src, u32 length, s16* f, s16 q, s16 *s);
void qrz_bpf_modfq(s16* dst, s16* src, u32 length, s16* f, s16* q, s16 *s);
void qrz_hpf(s16* dst, s16* src, u32 length, s16 f, s16 q, s16 *s);
void qrz_hpf_modf(s16* dst, s16* src, u32 length, s16* f, s16 q, s16 *s);
void qrz_hpf_modfq(s16* dst, s16* src, u32 length, s16* f, s16* q, s16 *s);

/// audio scenes

class qrz_maximus
{
public:
    qrz_maximus(u32 len);
    ~qrz_maximus();

    void trigger(u32 pitch);
    void run(s16 *buf, u32 len);
    s16* m_buf;
    s16* m_tab;
    u32 m_writepos;
    u32 m_readpos;
    u32 m_pitch;
};

class qrz_ambference
{
public:
    qrz_ambference(u32 len);
    ~qrz_ambference();

    void trigger(u32 note, u32 type);
    void run(s16 *buf, u32 len);
    s16* m_buf;
    s16* m_ringbuf;
    s16* m_fmbuf;
    s16* m_ringenvbuf;
    s16* m_fmenvbuf;

    u32 m_ringenvclk;
    u32 m_ringclk;
    u32 m_ringskip;

    u32 m_fmenvclk;
    u32 m_fmclk;
    u32 m_fmskip;

    u32 m_dutyclk;
    u32 m_dutyskip;
};

class qrz_melocore
{
public:
    qrz_melocore(u32 len);
    ~qrz_melocore();

    void trigger(u32 note, u32 type);
    void run(s16 *buf, u32 len);
    s16* m_buf;
    s16* m_bufphase;
    s16* m_envbuf;
    s16* m_pwmbuf;
    s16* m_pwmenvbuf;
    s16* m_pwmlfobuf;
    s16* m_sbuf;
    s16* m_senvbuf;
 
    u32 m_envclk;
    u32 m_clk;
    u32 m_skip;
    u32 m_clkphase;
    u32 m_skipphase;

    u32 m_pwmskip;
    u32 m_pwmclk;
    u32 m_pwmenvclk;
    u32 m_pwmlfoclk;

    u32 m_senvclk;
    u32 m_sclk;
    u32 m_sskip;
};

class qrz_pigeon
{
public:
    qrz_pigeon(u32 len);
    ~qrz_pigeon();

    void trigger(u32 note, u32 type);
    void run(s16 *buf, u32 len);
    s16* m_buf;
    s16* m_hatbuf;
    s16* m_hatenvbuf;
    s16* m_kckbuf;
    s16* m_kckenvbuf;
    s16* m_ridbuf;
    s16* m_ridenvbuf;
    s16* m_snrbuf;
    s16* m_snrenvbuf;

    u32 m_hatclk;
    u32 m_hatenvclk;
    u32 m_hatskip;

    u32 m_kckclk;
    u32 m_kckenvclk;
    u32 m_kckskip;

    u32 m_ridclk;
    u32 m_ridenvclk;
    u32 m_ridskip;

    u32 m_snrclk;
    u32 m_snrenvclk;
    u32 m_snrskip;

    s16 m_snrstate[2];
};

class qrz_azid
{
public:
    qrz_azid(u32 len);
    ~qrz_azid();

    void trigger(u32 note, u32 type);
    void run(s16 *buf, u32 len);

    s16* m_wowbuf;
    s16* m_wowenvbuf;

    s16* m_basbuf;
    s16* m_basdutybuf;
    s16* m_basenvbuf;

    s16* m_tecbuf;
    s16* m_tecdutybuf;
    s16* m_tecenvbuf;

    u32 m_wowclk;
    u32 m_wowenvclk;
    u32 m_wowskip;

    u32 m_basclk;
    u32 m_basdutyclk;
    u32 m_basenvclk;
    u32 m_basskip;

    u32 m_tecclk;
    u32 m_tecdutyclk;
    u32 m_tecenvclk;
    u32 m_tecskip;

    s16 m_wowstate[2];
};

class qrz_crunch
{
public:
    qrz_crunch(u32 len);
    ~qrz_crunch();

    void trigger(u32 note, u32 type);
    void run(s16 *buf, u32 len);
    s16* m_buf;
    s16* m_hatbuf;
    s16* m_hatenvbuf;
    s16* m_kckbuf;
    s16* m_kckenvbuf;
    s16* m_ridbuf;
    s16* m_ridenvbuf;
    s16* m_snrbuf;
    s16* m_snrenvbuf;

    u32 m_hatclk;
    u32 m_hatenvclk;
    u32 m_hatskip;

    u32 m_kckclk;
    u32 m_kckenvclk;
    u32 m_kckskip;

    u32 m_ridclk;
    u32 m_ridenvclk;
    u32 m_ridskip;

    u32 m_snrclk;
    u32 m_snrenvclk;
    u32 m_snrskip;

    s16 m_snrstate[2];

    u32 m_rngclk;
    u32 m_rngenvclk;
    u32 m_rngskip;
};

class qrz_ticklish
{
public:
    qrz_ticklish(u32 len);
    ~qrz_ticklish();

    void trigger(u32 note, u32 type);
    void run(s16 *buf, u32 len);

    s16* m_buf;
    s16* m_bufphase;
    s16* m_envbuf;

    s16* m_buzbuf;
    s16* m_buzenvbuf;

    s16* m_phabuf;
    s16* m_phbbuf;

    u32 m_envclk;
    u32 m_clk;
    u32 m_skip;
    u32 m_clkphase;
    u32 m_skipphase;

    u32 m_buzskip;
    u32 m_buzclk;
    u32 m_buzenvclk;

    u32 m_bzzskip;
    u32 m_bzzclk;
    u32 m_bzzenvclk;

    u32 m_phaclk;
    u32 m_phaenvclk;
    u32 m_phaskip;
    u32 m_phbclk;
    u32 m_phbskip;

};

class qrz_glitchiference
{
public:
    qrz_glitchiference(u32 len);
    ~qrz_glitchiference();

    void trigger(u32 note, u32 type);
    void run(s16 *buf, u32 len);

    s16* m_buf;
    s16* m_dutybuf;
    u32 m_dutyclk;
    u32 m_dutyskip;
    s16* m_readpos;
    u32 m_readclk;
};

#endif
