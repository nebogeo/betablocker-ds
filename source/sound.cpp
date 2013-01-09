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

#include <stdio.h>
#include <math.h>
#include <maxmod9.h>
#include "sound.h"
#include "soundbank.h"
#include "tuning.h"

qrz_ambference *sound::m_ambference=NULL;
qrz_melocore *sound::m_melocore=NULL;
qrz_maximus *sound::m_maximus=NULL;
qrz_pigeon *sound::m_pigeon=NULL;
qrz_azid *sound::m_azid=NULL;
qrz_crunch *sound::m_crunch=NULL;
qrz_ticklish *sound::m_ticklish=NULL;
qrz_glitchiference *sound::m_glitchiference=NULL;
u32 sound::m_current_bank=9999;

static s16 *qrz_sinetab;
static s16 *qrz_sqrtab;
static s16 *qrz_sawtab;
static s16 *qrz_decayenv;
static s16 *qrz_hillenv;
static s16 *qrz_sinelfo;

static s16 s[2];
s16 fbuf[512];
s16 qbuf[512];
s16 gbuf[512];

sound::sound() 
{
    set_nthrt(12);
    qrz_wavetable_init();
    qrz_init_skiplut();
    
    test();

	mm_ds_system sys;
	sys.mod_count 			= 0;
	sys.samp_count			= 0;
	sys.mem_bank			= 0;
	sys.fifo_channel		= FIFO_MAXMOD;
	mmInit( &sys );
	
	mm_stream mystream;
	mystream.sampling_rate	= 25000;					// sampling rate = 25khz
	mystream.buffer_length	= 1024;						// buffer length = 1200 samples
	mystream.callback		= sound::on_stream_request;   // set callback function
	mystream.format			= MM_STREAM_16BIT_MONO;	// format = stereo 16-bit
	mystream.timer			= MM_TIMER0;				// use hardware timer 0
	mystream.manual			= true;						// use manual filling
	mmStreamOpen( &mystream );

    m_ambference=new qrz_ambference(mystream.buffer_length); 
    m_melocore=new qrz_melocore(mystream.buffer_length); 
    m_maximus=new qrz_maximus(mystream.buffer_length); 
    m_pigeon=new qrz_pigeon(mystream.buffer_length); 
    m_azid=new qrz_azid(mystream.buffer_length); 
    m_crunch=new qrz_crunch(mystream.buffer_length); 
    m_ticklish=new qrz_ticklish(mystream.buffer_length); 
    m_glitchiference=new qrz_glitchiference(mystream.buffer_length); 
}

sound::~sound()
{
}

void sound::load_bank(u32 b)
{
    m_current_bank=b%9;

/*
    b=b%(MSL_NSAMPS/SOUND_BANK_SIZE);
    iprintf("loading bank %d\n",b); 
    if (m_current_bank!=9999)
    {
        iprintf("unloading bank %d\n",m_current_bank); 
        for (u32 i=m_current_bank*SOUND_BANK_SIZE; 
             i<=(m_current_bank+1)*SOUND_BANK_SIZE; i++) 
        {
            unload(i);
        }
    }
    m_current_bank=b;
    u32 from=m_current_bank*SOUND_BANK_SIZE;
    u32 to=(m_current_bank+1)*SOUND_BANK_SIZE;
    for (u32 i=from; i<to; i++) load(i);
*/
}

void sound::load(u32 id)
{
//    iprintf("loading %d\n", id);

/*    if (id<MSL_NSAMPS)
    {
        u32 lid=id%SOUND_BANK_SIZE;
        mmLoadEffect(id);
        m_sounds[lid].id=id;
        m_sounds[lid].rate=(int)(1.0f * (1<<10));
        m_sounds[lid].handle=0;
        m_sounds[lid].volume=127;
        m_sounds[lid].panning=127;
*/        
/*

            {
                { id } ,			// id
                (int)(1.0f * (1<<10)),	// rate
                0,		// handle
                127,	// volume
                127,		// panning
            };

            } */
}

void sound::unload(u32 id)
{
//    mmUnloadEffect(id);
}

void sound::set_nthrt(u32 n)
{
    m_nthrt=powf(1,1/(float)n);
    m_nthrt=0.2;
    iprintf("nthrt: %f\n", m_nthrt);
}

void sound::play(u32 id, u32 n) 
{
    u32 slot=id/5;
    bool higher=true;
    if (slot&0x1) higher=false;
    u32 sid=id%5;

    iprintf("higher=%d, id=%d\n",higher,id);

    switch(m_current_bank%9)
    {
    case 0:
        if (higher) m_glitchiference->trigger(n,id);
        else m_glitchiference->trigger(n,id);
        break;
    case 1:
        if (higher) m_glitchiference->trigger(n,id);
        else m_ambference->trigger(n,sid);
        break;
    case 2:
        if (higher) m_melocore->trigger(n,sid);
        else m_ambference->trigger(n,sid);
        break;
    case 3:
        if (higher) m_melocore->trigger(n,sid);
        else m_pigeon->trigger(n,sid);
        break;
    case 4:
        if (higher) m_ticklish->trigger(n,sid);
        else m_pigeon->trigger(n,sid);
        break;
    case 5:
        if (higher) m_ticklish->trigger(n,sid);
        else m_azid->trigger(n,sid);
        break;
    case 6:
        if (higher) m_crunch->trigger(n,sid);
        else m_azid->trigger(n,sid);
        break;
    case 7:
        if (higher) m_crunch->trigger(n,id);
        else m_maximus->trigger(n);
        break;
    case 8:
        m_maximus->trigger(n);
        break;
    };
    
/*    u32 note=n%32;
    id=id%SOUND_BANK_SIZE;
    u32 gid=id+m_current_bank*SOUND_BANK_SIZE;
    if (gid>=MSL_NSAMPS) id=0;

    //if (freq>0) //&& id>0)
    {
        iprintf("playing %d %d\n", note, gid);
        m_sounds[id].rate=note_to_freq(note)*(1<<10);
        s8 pan=id%2;
        if (pan==0) pan=-1;
        m_sounds[id].panning=127+pan*20;
        mmEffectEx(&m_sounds[id]);
    }*/
}

void sound::update()
{
    mmStreamUpdate();
}

mm_word sound::on_stream_request( mm_word length, mm_addr dest, mm_stream_formats format ) 
{
    s16* buf=(s16*)dest;

    memset(buf,0,length*2);

    if (m_maximus!=NULL)
    {        
        bool run_test=false;
        if (run_test)
        {
            audio_test(buf,length);
        }
        else
        {
            switch(m_current_bank%9)
            {
            case 0:
                m_glitchiference->run(buf,length);
                break;
            case 1:
                m_glitchiference->run(buf,length);
                m_ambference->run(buf,length);
                break;
            case 2:
                m_melocore->run(buf,length);
                m_ambference->run(buf,length);
                break;
            case 3:
                m_melocore->run(buf,length);
                m_pigeon->run(buf,length);
                break;
            case 4:
                m_ticklish->run(buf,length);
                m_pigeon->run(buf,length);
                break;
            case 5:
                m_ticklish->run(buf,length);
                m_azid->run(buf,length);
                break;
            case 6:
                m_crunch->run(buf,length);
                m_azid->run(buf,length);
                break;
            case 7:
                m_crunch->run(buf,length);
                m_maximus->run(buf,length);
                break;
            case 8:
                m_maximus->run(buf,length);
                break;
            };
        }
    }
    return length;
}

//------------------------------------------------------

void sound::test()
{
    s16* abuf=new s16[30];
    s16* test=new s16[30];
    int clk=0;
    int aclk=0;

    clk=0;
//    clk=qrz_tabread_lfo(test,30,clk,2000000,qrz_sinetab);

    aclk=qrz_tabread_env(abuf,30,aclk,4,qrz_decayenv);
    clk=qrz_tabread_mod(test,30,clk,abuf,qrz_sinetab);

    iprintf("%d\n",clk);
    for (int i=0; i<30; i++) iprintf("%d ",test[i]);
    iprintf("\n");

    aclk=qrz_tabread_env(abuf,30,aclk,400,qrz_decayenv);
    clk=qrz_tabread_mod(test,30,clk,abuf,qrz_sinetab);

    iprintf("%d\n",clk);
    for (int i=0; i<30; i++) iprintf("%d ",test[i]);
    iprintf("\n");




/*
    clk=qrz_tabread_env(test,30,clk,2,qrz_decayenv);
    iprintf("%d\n",clk);
    for (int i=0; i<30; i++) iprintf("%d ",test[i]);
    iprintf("\n");
*/
    clk=qrz_tabread_env(test,30,10000,100,qrz_hillenv);
//    for (int i=0; i<30; i++) iprintf("%d ",test[i]);
    iprintf("\n");

/*    clk=qrz_saw_tooth(test,clk,30,4096/16);
    for (int i=0; i<30; i++) iprintf("%d ",test[i]);
    iprintf("\n");
    iprintf("%d\n",clk);
    clk=qrz_saw_tooth(test,clk,30,4096/16);
    for (int i=0; i<30; i++) iprintf("%d ",test[i]);
    iprintf("\n");
*/



//    for (int i=0; i<30; i++) iprintf("%d ",qrz_wavetable_sine[i]);
//    iprintf("\n");

    //    for (int i=0; i<30; i++) iprintf("%d ",test[i]);
    //iprintf("\n");

/*
    s16* tab=new s16[512];
    iprintf("%d\n",tab);
    clk=qrz_maximus_inner(test,30,clk,0,2,tab);
    iprintf("%d\n",clk);
    for (int i=0; i<30; i++) iprintf("%d ",test[i]);
    iprintf("\n");
    for (int i=0; i<30; i++) iprintf("%d ",tab[i]);
    iprintf("\n");
*/

//    qrz_stretch_2_sqr(test,10,test,level);
//    for (int i=0; i<10; i++) iprintf("%d ",test[i]);
//    iprintf("\n");

}

void sound::audio_test(s16 *buf, u32 len)
{
    static u32 clk=0;
    static s16 abuf[512];
    static u32 aclk=0;
    static u32 lfo=0;
    static u32 lfo2=0;
    
    static int c=101;
    static int note=0;
    static u32 dur=10;

//bgGetMapPtr(0)

    if (c>100)
    {
        c=0;
        note++;
        dur=qrz_note_to_skip(note);
        iprintf("%d\n", note);   
        aclk=0;
        lfo=0;
    }
    c++;

    aclk=qrz_tabread_env(abuf,len,aclk,40,qrz_decayenv);
    clk=qrz_tabread_mod(buf,len,clk,abuf,qrz_sinetab);
    // qrz_mul(buf,buf,ffx(0.2),len);
           
    // (s16*)bgGetGfxPtr(0)+1000

    //aclk=qrz_tabread_env(abuf,len,aclk,16,qrz_hillenv);
    //clk=qrz_tabread(buf,len,clk,50,qrz_sinetab);
    //qrz_mul(buf,buf,abuf,len);
    //lfo=qrz_tabread_env(fbuf,len,lfo,128,qrz_decayenv);
    //lfo2=qrz_tabread_lfo(qbuf,len,lfo2,4,qrz_sinetab);
    //qrz_lpf_modf(buf,buf,len,fbuf,ffx(0.9),s);        
}

/////////////////////////////////////////////////////////////
 
// .10
inline s32 fx(s32 i) { return i<<FX_SHIFT; }
inline s32 ffx(float i) { return (s32)((1<<FX_SHIFT)*i); }
inline s32 fromfx(s32 i) { return i>>FX_SHIFT; }
inline s32 fxmul(s32 a, s32 b) { return (a*b)>>FX_SHIFT; }
inline s32 fxdiv(s32 a, s32 b) { return (a<<FX_SHIFT)/b; }

void qrz_mul(s16* dst, s16 *a, s16 *b, u32 length)
{
    for (u32 i=0; i<length; ++i)
    {
        dst[i]=(s16)fxmul((s32)a[i],(s32)b[i]);
    }
}

void qrz_mul(s16* dst, s16 *a, s32 b, u32 length)
{
    for (u32 i=0; i<length; ++i)
    {
        dst[i]=(s16)fxmul((s32)a[i],b);
    }
}

void qrz_add(s16* dst, s16 *a, s16 *b, u32 length)
{
    for (u32 i=0; i<length; ++i)
    {
        s32 t=a[i]+b[i];
        if (t>32767) t=32767;
        if (t<-32767) t=-32767;
        dst[i]=(s16)t;
        // dst[i]=a[i]+b[i];
    }
}

void qrz_add(s16* dst, s16 *a, s16 b, u32 length)
{
    for (u32 i=0; i<length; ++i)
    {
        //iprintf("%d %d\n",i,length);
        dst[i]=a[i]+b;
    }
}

void qrz_audio2cv(s16* dst, s16 *src, u32 length)
{
    for (u32 i=0; i<length; ++i)
    {
        dst[i]=src[i]>>4;
    }
}

void qrz_cv2audio(s16* dst, s16 *src, u32 length)
{
    for (u32 i=0; i<length; ++i)
    {
        dst[i]=src[i]<<4;
    }
}


///////////////////////////////////////////////////////
// filters

void qrz_lpf(s16* dst, s16* src, u32 length, s16 f, s16 q, s16 *s)
{
    u32 fb = q+fxmul(q,fx(1)-f);
    for (u32 i=0; i<length; ++i)
    {
        s[0]+=fxmul(f,src[i]-s[0]+fxmul(fb,s[0]-s[1]));
        s[1]+=fxmul(f,s[0]-s[1]);
        dst[i]=s[1];
    }
}

void qrz_lpf_modf(s16* dst, s16* src, u32 length, s16* f, s16 q, s16 *s)
{
    for (u32 i=0; i<length; ++i)
    {
        u32 fb=q+fxmul(q,fx(1)-f[i]);
        s[0]+=fxmul(f[i],src[i]-s[0]+fxmul(fb,s[0]-s[1]));
        s[1]+=fxmul(f[i],s[0]-s[1]);
        dst[i]=s[1];
    }
}

void qrz_lpf_modfq(s16* dst, s16* src, u32 length, s16* f, s16* q, s16 *s)
{
    for (u32 i=0; i<length; ++i)
    {
        u32 fb=q[i]+fxmul(q[i],fx(1)-f[i]);
        s[0]+=fxmul(f[i],src[i]-s[0]+fxmul(fb,s[0]-s[1]));
        s[1]+=fxmul(f[i],s[0]-s[1]);
        dst[i]=s[1];
    }
}

void qrz_bpf(s16* dst, s16* src, u32 length, s16 f, s16 q, s16 *s)
{
}

void qrz_bpf_modf(s16* dst, s16* src, u32 length, s16* f, s16 q, s16 *s)
{
    for (u32 i=0; i<length; ++i)
    {
        u32 bp=s[0]-s[1];
        u32 fb=q+fxmul(q,fx(1)-f[i]);
        s[0]+=fxmul(f[i],src[i]-s[0]+fxmul(fb,bp));
        s[1]+=fxmul(f[i],s[0]-s[1]);
        dst[i]=bp;
    }
}

void qrz_bpf_modfq(s16* dst, s16* src, u32 length, s16* f, s16* q, s16 *s)
{
}

void qrz_hpf(s16* dst, s16* src, u32 length, s16 f, s16 q, s16 *s)
{
}

void qrz_hpf_modf(s16* dst, s16* src, u32 length, s16* f, s16 q, s16 *s)
{
    for (u32 i=0; i<length; ++i)
    {
        u32 hp=src[i]-s[0];
        u32 fb=q+fxmul(q,fx(1)-f[i]);
        s[0]+=fxmul(f[i],hp+fxmul(fb,s[0]-s[1]));
        s[1]+=fxmul(f[i],s[0]-s[1]);
        dst[i]=hp;
    }
}

void qrz_hpf_modfq(s16* dst, s16* src, u32 length, s16* f, s16* q, s16 *s)
{
}


void qrz_wavetable_init()
{
    qrz_sinetab = new s16[WAVETABLE_SIZE];
    qrz_sqrtab = new s16[WAVETABLE_SIZE];
    qrz_sawtab = new s16[WAVETABLE_SIZE];
    qrz_decayenv = new s16[WAVETABLE_SIZE];
    qrz_hillenv = new s16[WAVETABLE_SIZE];
    qrz_sinelfo = new s16[WAVETABLE_SIZE];

    float pi=3.14159265;

    for(u32 i=0; i<WAVETABLE_SIZE; i++)
    {
        float t=i/(float)WAVETABLE_SIZE;
        qrz_sinetab[i]=AUDIO_SCALE*sin(t*2*pi);
        if (qrz_sinetab[i]>0) qrz_sqrtab[i]=AUDIO_SCALE;
        else qrz_sqrtab[i]=-AUDIO_SCALE;
        qrz_sawtab[i]=AUDIO_SCALE*t;
        qrz_decayenv[i]=fx(1)*(1-t)*(1-t)*(1-t);
        qrz_hillenv[i]=fx(1)*(sin(t*2*pi+pi*1.5)/2+0.5);
        qrz_sinelfo[i]=ffx(0.5)*sin(t*2*pi)+ffx(0.5);
    }
}

u32 *qrz_skiplut;
u32 qrz_ionian[]={2, 2, 1, 2, 2, 2, 1};
u32 qrz_gypsy[]={2, 1, 3, 1, 1, 2, 2};

void qrz_init_skiplut()
{ 
    qrz_skiplut=qrz_get_skiplut(WAVETABLE_SIZE, 25000);
}

u32 qrz_note_to_skip(u32 n)
{ 
    return qrz_skiplut[n%SKIPLUT_SIZE]; 
}

void qrz_thrsh2(s16* dst, u32 length, s16 *src, s16 *thr)
{
    for (u32 i=0; i<length; i++)
    {
        if (abs(src[i])<(abs(fxdiv(thr[i],ffx(1.2))))) dst[i]=-AUDIO_SCALE;
        else dst[i]=AUDIO_SCALE;
    }
}

//////////////////////////////////////////////////

qrz_maximus::qrz_maximus(u32 len) :
    m_writepos(0),
    m_readpos(0),
    m_pitch(0)
{
    m_buf=new s16[len];
    m_tab=new s16[512];
    qrz_tabread(m_tab,512,0,2,qrz_sinetab);    
}

qrz_maximus::~qrz_maximus()
{
    delete[] m_buf;
    delete[] m_tab;
}

void qrz_maximus::trigger(u32 pitch)
{
    m_pitch=pitch+256;
    qrz_tabread(m_tab,512,0,2,qrz_sinetab);    
}

void qrz_maximus::run(s16* buf, u32 len)
{
    m_readpos=qrz_maximus_inner(m_buf,len,m_readpos,m_writepos,m_pitch,m_tab);
    qrz_mul(m_buf,m_buf,ffx(3),len);
    qrz_add(buf,buf,m_buf,len);
    m_writepos+=len;
    m_writepos=m_writepos%512;
}

//////////////////////////////////////////////////

qrz_ambference::qrz_ambference(u32 len)
{
    m_buf=new s16[len];
    m_ringbuf=new s16[len];
    m_fmbuf=new s16[len];
    m_ringenvbuf=new s16[len];
    m_fmenvbuf=new s16[len];
}
    
qrz_ambference::~qrz_ambference()
{
    delete[] m_buf;
    delete[] m_ringbuf;
    delete[] m_fmbuf;
    delete[] m_ringenvbuf;
    delete[] m_fmenvbuf;
}

void qrz_ambference::trigger(u32 note, u32 type)
{
    switch(type%3)
    {
    case 0: 
        m_dutyskip=qrz_note_to_skip(note);
        break;
    case 1: 
        m_fmskip=qrz_note_to_skip(note);
        m_fmenvclk=0;
        break;
    case 2: 
        m_ringskip=qrz_note_to_skip(note);
        m_ringenvclk=0;
        break;
    };
}

void qrz_ambference::run(s16 *buf, u32 len)
{
    // ring
    m_ringenvclk=qrz_tabread_env(m_ringenvbuf,len,m_ringenvclk,2,qrz_hillenv);
    m_ringclk=qrz_tabread(m_ringbuf,len,m_ringclk,m_ringskip,qrz_sinetab);
    qrz_mul(m_ringbuf,m_ringbuf,m_ringenvbuf,len);
    qrz_audio2cv(m_ringbuf,m_ringbuf,len);
    qrz_add(m_ringbuf,m_ringbuf,fx(1),len);
    
    // fm
    m_fmenvclk=qrz_tabread_env(m_fmenvbuf,len,m_fmenvclk,2,qrz_hillenv);
    m_fmclk=qrz_tabread(m_fmbuf,len,m_fmclk,m_fmskip,qrz_sinetab);
    qrz_mul(m_fmbuf,m_fmbuf,m_fmenvbuf,len);
    qrz_audio2cv(m_fmbuf,m_fmbuf,len);
    qrz_add(m_fmbuf,m_fmbuf,m_dutyskip,len);

    // duty
    m_dutyclk=qrz_tabread_mod(m_buf,len,m_dutyclk,m_fmbuf,qrz_sinetab);
    qrz_mul(m_buf,m_buf,m_ringbuf,len);
    qrz_add(buf,buf,m_buf,len);
}

//////////////////////////////////////////////////

qrz_melocore::qrz_melocore(u32 len)
{
    m_buf=new s16[len];
    m_bufphase=new s16[len];
    m_envbuf=new s16[len];
    m_pwmbuf=new s16[len];
    m_pwmenvbuf=new s16[len];
    m_pwmlfobuf=new s16[len];
    m_sbuf=new s16[len];
    m_senvbuf=new s16[len];
}

qrz_melocore::~qrz_melocore()
{
    delete[] m_buf;
    delete[] m_bufphase;
    delete[] m_envbuf;
    delete[] m_pwmbuf;
    delete[] m_pwmenvbuf;
    delete[] m_pwmlfobuf;
    delete[] m_sbuf;
    delete[] m_senvbuf;
}

void qrz_melocore::trigger(u32 note, u32 type)
{
    note=(note%40)+20;
    switch (type%3)
    {
    case 0:
        m_pwmskip=qrz_note_to_skip(qrz_scale_lu(note,qrz_gypsy,7));
        m_pwmenvclk=0;
        break;
    case 1:
        m_skip=qrz_note_to_skip(qrz_scale_lu(note,qrz_gypsy,7));
        m_skipphase=qrz_note_to_skip(qrz_scale_lu(note+3,qrz_gypsy,7));
        m_envclk=0;
        break;
    case 2:
        m_sskip=qrz_note_to_skip(qrz_scale_lu(note,qrz_gypsy,7));
        m_senvclk=0;
        break;
    }
}

void qrz_melocore::run(s16 *buf, u32 len)
{
    m_envclk=qrz_tabread_env(m_envbuf,len,m_envclk,16,qrz_decayenv);
    m_clk=qrz_tabread(m_buf,len,m_clk,m_skip,qrz_sawtab);
    m_clkphase=qrz_tabread(m_bufphase,len,m_clkphase,m_skipphase,qrz_sawtab);
    qrz_add(m_buf,m_buf,m_bufphase,len);
    qrz_mul(m_buf,m_buf,m_envbuf,len);
    qrz_add(buf,buf,m_buf,len);

    m_pwmlfoclk=qrz_tabread_lfo(m_pwmlfobuf,len,m_pwmlfoclk,2,qrz_sinetab);
    m_pwmenvclk=qrz_tabread_env(m_pwmenvbuf,len,m_pwmenvclk,16,qrz_decayenv);
    m_clk=qrz_tabread(m_pwmbuf,len,m_pwmclk,m_pwmskip,qrz_sawtab);
    qrz_thrsh2(m_pwmbuf,len,m_pwmbuf,m_pwmlfobuf);
    qrz_mul(m_pwmbuf,m_pwmbuf,m_pwmenvbuf,len);
    qrz_add(buf,buf,m_pwmbuf,len);

    m_senvclk=qrz_tabread_env(m_senvbuf,len,m_senvclk,4,qrz_hillenv);
    m_sclk=qrz_tabread(m_sbuf,len,m_sclk,m_sskip,qrz_sinetab);
    qrz_mul(m_sbuf,m_sbuf,m_senvbuf,len);
    qrz_add(buf,buf,m_sbuf,len);
}

//////////////////////////////////////////////////

qrz_pigeon::qrz_pigeon(u32 len)
{
    m_buf=new s16[len];
    m_hatbuf=new s16[len];
    m_hatenvbuf=new s16[len];
    m_kckbuf=new s16[len];
    m_kckenvbuf=new s16[len];
    m_ridbuf=new s16[len];
    m_ridenvbuf=new s16[len];
    m_snrbuf=new s16[len];
    m_snrenvbuf=new s16[len];
    m_kckclk=0;
}

qrz_pigeon::~qrz_pigeon()
{
    delete[] m_buf;
    delete[] m_hatbuf;
    delete[] m_hatenvbuf;
    delete[] m_kckbuf;
    delete[] m_kckenvbuf;
    delete[] m_ridbuf;
    delete[] m_ridenvbuf;
    delete[] m_snrbuf;
    delete[] m_snrenvbuf;

}

void qrz_pigeon::trigger(u32 note, u32 type)
{
    switch (type%4)
    {
    case 0:
        m_hatskip=200;//note;
        m_hatenvclk=0;
        break;
    case 1:
        m_kckskip=qrz_note_to_skip(qrz_scale_lu(note,qrz_gypsy,7));
        m_kckenvclk=0;
        break;
    case 2:
        m_ridskip=note;
        m_ridenvclk=0;
        break;
    case 3:
        m_snrskip=200;//255-note;
        m_snrenvclk=0;
        m_snrstate[0]=0;
        m_snrstate[1]=0;
        break;
    }
}

void qrz_pigeon::run(s16 *buf, u32 len)
{
    m_hatenvclk=qrz_tabread_env(m_hatenvbuf,len,m_hatenvclk,m_hatskip,qrz_decayenv);
    qrz_white_noise(m_hatbuf,m_hatclk,len,0);
    m_hatclk++;
    qrz_mul(m_hatbuf,m_hatbuf,m_hatenvbuf,len);
    qrz_add(buf,buf,m_hatbuf,len);

    m_kckenvclk=qrz_tabread_env(m_kckenvbuf,len,m_kckenvclk,40,qrz_decayenv);
    m_kckclk=qrz_tabread_mod(m_kckbuf,len,m_kckclk,m_kckenvbuf,qrz_sinetab);
    qrz_add(buf,buf,m_kckbuf,len);

    m_ridenvclk=qrz_tabread_env(m_ridenvbuf,len,m_ridenvclk,255,qrz_decayenv);
    qrz_white_noise(m_ridbuf,m_ridclk,len,0);
    m_ridclk++;
    qrz_mul(m_ridbuf,m_ridbuf,m_ridenvbuf,len);
    qrz_add(buf,buf,m_ridbuf,len);

    m_snrenvclk=qrz_tabread_env(m_snrenvbuf,len,m_snrenvclk,58,qrz_decayenv);
    qrz_white_noise(m_snrbuf,m_snrclk,len,0);
    m_snrclk++;
    qrz_lpf_modf(m_snrbuf,m_snrbuf,len,m_snrenvbuf,fx(1)*(m_snrskip/255),m_snrstate);
    qrz_add(buf,buf,m_snrbuf,len);
}


//////////////////////////////////////////////////

qrz_azid::qrz_azid(u32 len)
{
    m_wowbuf=new s16[len];
    m_wowenvbuf=new s16[len];
    m_basbuf=new s16[len];
    m_basdutybuf=new s16[len];
    m_basenvbuf=new s16[len];
    m_tecbuf=new s16[len];
    m_tecdutybuf=new s16[len];
    m_tecenvbuf=new s16[len];
    m_wowstate[0]=0;
    m_wowstate[1]=0;
    m_basskip=0;
    m_basenvclk=0;
}

qrz_azid::~qrz_azid()
{
    delete[] m_wowbuf;
    delete[] m_wowenvbuf;
    delete[] m_basbuf;
    delete[] m_basdutybuf;
    delete[] m_basenvbuf;
    delete[] m_tecbuf;
    delete[] m_tecdutybuf;
    delete[] m_tecenvbuf;
}

void qrz_azid::trigger(u32 note, u32 type)
{
    switch (type%3)
    {
    case 0:
        m_wowskip=qrz_note_to_skip(qrz_scale_lu(note%50,qrz_ionian,7));
        m_wowenvclk=0;
        break;
    case 1:
        m_basskip=qrz_note_to_skip(qrz_scale_lu(note,qrz_ionian,7));
        m_basenvclk=0;
        break;
    case 2:
        m_tecskip=2*qrz_note_to_skip(qrz_scale_lu(note,qrz_ionian,7));
        m_tecenvclk=0;
        break;
    }
}

void qrz_azid::run(s16 *buf, u32 len)
{
    m_wowenvclk=qrz_tabread_env(m_wowenvbuf,len,m_wowenvclk,64,qrz_decayenv);
    m_wowclk=qrz_tabread(m_wowbuf,len,m_wowclk,m_wowskip,qrz_sawtab);
    qrz_lpf_modf(m_wowbuf,m_wowbuf,len,m_wowenvbuf,fx(0.01),m_wowstate);
    qrz_add(buf,buf,m_wowbuf,len);

    m_basenvclk=qrz_tabread_env(m_basenvbuf,len,m_basenvclk,12,qrz_hillenv);
    m_basdutyclk=qrz_tabread(m_basdutybuf,len,m_basdutyclk,10,qrz_sinelfo);
    qrz_mul(m_basdutybuf,m_basdutybuf,m_basenvbuf,len);
    qrz_add(m_basdutybuf,m_basdutybuf,m_basskip,len);
    m_basclk=qrz_tabread_mod(m_basbuf,len,m_basclk,m_basdutybuf,qrz_sinetab);
    if (m_basenvbuf[0]!=0) qrz_add(buf,buf,m_basbuf,len);
    
    m_tecenvclk=qrz_tabread_env(m_tecenvbuf,len,m_tecenvclk,32,qrz_decayenv);
    m_tecdutyclk=qrz_tabread(m_tecdutybuf,len,m_tecdutyclk,fxmul(m_tecskip,3),qrz_sinelfo);
    qrz_mul(m_tecenvbuf,m_tecenvbuf,m_tecskip<<8,len);
    qrz_add(m_tecdutybuf,m_tecdutybuf,m_tecskip<<4,len);
    qrz_mul(m_tecdutybuf,m_tecdutybuf,m_tecenvbuf,len);
    m_tecclk=qrz_tabread_mod(m_tecbuf,len,m_tecclk,m_tecdutybuf,qrz_sinetab);
    if (m_tecenvbuf[0]!=0) qrz_add(buf,buf,m_tecbuf,len);
}

//////////////////////////////////////////////////

qrz_crunch::qrz_crunch(u32 len)
{
    m_buf=new s16[len];
    m_hatbuf=new s16[len];
    m_hatenvbuf=new s16[len];
    m_kckbuf=new s16[len];
    m_kckenvbuf=new s16[len];
    m_ridbuf=new s16[len];
    m_ridenvbuf=new s16[len];
    m_snrbuf=new s16[len];
    m_snrenvbuf=new s16[len];
}

qrz_crunch::~qrz_crunch()
{
    delete[] m_buf;
    delete[] m_hatbuf;
    delete[] m_hatenvbuf;
    delete[] m_kckbuf;
    delete[] m_kckenvbuf;
    delete[] m_ridbuf;
    delete[] m_ridenvbuf;
    delete[] m_snrbuf;
    delete[] m_snrenvbuf;
}

void qrz_crunch::trigger(u32 note, u32 type)
{
    switch (type%5)
    {
    case 0:
        m_hatskip=200;//note;
        m_hatenvclk=0;
        break;
    case 1:
        m_kckskip=qrz_note_to_skip(qrz_scale_lu(note,qrz_gypsy,7));
        m_kckenvclk=0;
        break;
    case 2:
        m_ridskip=note;
        m_ridenvclk=0;
        break;
    case 3:
        m_snrskip=200;//255-note;
        m_snrenvclk=0;
        m_snrstate[0]=0;
        m_snrstate[1]=0;
        break;
    case 4:
        m_rngskip=qrz_note_to_skip(qrz_scale_lu(note,qrz_gypsy,7));
        m_rngenvclk=0;
    }
}

void qrz_crunch::run(s16 *buf, u32 len)
{
    m_hatenvclk=qrz_tabread_env(m_hatenvbuf,len,m_hatenvclk,40,qrz_decayenv);
    qrz_white_noise(m_hatbuf,m_hatclk,len,0);
    m_hatclk++;
    qrz_mul(m_hatbuf,m_hatbuf,m_hatenvbuf,len);
    qrz_add(buf,buf,m_hatbuf,len);

    m_kckenvclk=qrz_tabread_env(m_kckenvbuf,len,m_kckenvclk,m_kckskip,qrz_decayenv);
    m_kckclk=qrz_tabread_mod(m_kckbuf,len,m_kckclk,m_kckenvbuf,qrz_sinetab);
    qrz_add(buf,buf,m_kckbuf,len);

    m_ridenvclk=qrz_tabread_env(m_ridenvbuf,len,m_ridenvclk,40,qrz_decayenv);
    qrz_white_noise(m_ridbuf,m_ridclk,len,0);
    m_ridclk++;
    qrz_mul(m_ridbuf,m_ridbuf,m_ridenvbuf,len);
    qrz_add(buf,buf,m_ridbuf,len);

    m_snrenvclk=qrz_tabread_env(m_snrenvbuf,len,m_snrenvclk,58,qrz_decayenv);
    qrz_white_noise(m_snrbuf,m_snrclk,len,0);
    m_snrclk++;
    qrz_lpf_modf(m_snrbuf,m_snrbuf,len,m_snrenvbuf,fx(1)*(m_snrskip/255),m_snrstate);
    qrz_add(buf,buf,m_snrbuf,len);

    m_rngenvclk=qrz_tabread_env(m_snrenvbuf,len,m_rngenvclk,8,qrz_decayenv);
    m_rngclk=qrz_tabread(m_snrbuf,len,m_rngclk,m_rngskip,qrz_sinetab);
    qrz_mul(m_snrbuf,m_snrbuf,m_snrenvbuf,len);

    qrz_mul(buf,buf,m_snrbuf,len);
}

//////////////////////////////////////////////////

qrz_ticklish::qrz_ticklish(u32 len)
{
    m_buf=new s16[len];
    m_bufphase=new s16[len];
    m_envbuf=new s16[len];

    m_buzbuf=new s16[len];
    m_buzenvbuf=new s16[len];

    m_phabuf=new s16[len];
    m_phbbuf=new s16[len];
}

qrz_ticklish::~qrz_ticklish()
{
    delete[] m_buf;
    delete[] m_bufphase;
    delete[] m_envbuf;
    delete[] m_buzbuf;
    delete[] m_buzenvbuf;
    delete[] m_phabuf;
    delete[] m_phbbuf;
}

void qrz_ticklish::trigger(u32 note, u32 type)
{
    note=(note%40)+30;
    switch (type%4)
    {
    case 0:
        m_buzskip=qrz_note_to_skip(qrz_scale_lu(note,qrz_gypsy,7));
        m_buzenvclk=0;
        break;
    case 1:
        m_skip=qrz_note_to_skip(qrz_scale_lu(note,qrz_gypsy,7));
        m_skipphase=qrz_note_to_skip(qrz_scale_lu(note+3,qrz_gypsy,7));
        m_envclk=0;
        break;
    case 2:
        m_bzzskip=qrz_note_to_skip(qrz_scale_lu(note,qrz_gypsy,7));
        m_bzzenvclk=0;
        break;
    case 3:
        m_phaskip=qrz_note_to_skip(qrz_scale_lu(note,qrz_gypsy,7));
        m_phbskip=m_skip+1;
        m_phaenvclk=0;
        break;
    }
}

void qrz_ticklish::run(s16 *buf, u32 len)
{
    m_envclk=qrz_tabread_env(m_envbuf,len,m_envclk,128,qrz_decayenv);
    m_clk=qrz_tabread(m_buf,len,m_clk,m_skip,qrz_sawtab);
    m_clkphase=qrz_tabread(m_bufphase,len,m_clkphase,m_skipphase,qrz_sawtab);
    qrz_add(m_buf,m_buf,m_bufphase,len);
    qrz_mul(m_buf,m_buf,m_envbuf,len);
    qrz_add(buf,buf,m_buf,len);

    m_buzenvclk=qrz_tabread_env(m_buzenvbuf,len,m_buzenvclk,128,qrz_decayenv);
    m_buzclk=qrz_tabread(m_buzbuf,len,m_buzclk,m_buzskip,qrz_sqrtab);
    qrz_mul(m_buzbuf,m_buzbuf,m_buzenvbuf,len);
    qrz_add(buf,buf,m_buzbuf,len);

    m_bzzenvclk=qrz_tabread_env(m_buzenvbuf,len,m_bzzenvclk,128,qrz_decayenv);
    m_bzzclk=qrz_tabread(m_buzbuf,len,m_bzzclk,m_bzzskip,qrz_sawtab);
    qrz_mul(m_buzbuf,m_buzbuf,m_buzenvbuf,len);
    qrz_add(buf,buf,m_buzbuf,len);

    m_phaenvclk=qrz_tabread_env(m_buzenvbuf,len,m_phaenvclk,32,qrz_hillenv);
    m_phaclk=qrz_tabread(m_phabuf,len,m_phaclk,m_phaskip,qrz_sawtab);
    m_phbclk=qrz_tabread(m_phbbuf,len,m_phbclk,m_phbskip,qrz_sawtab);
    qrz_add(m_buf,m_phabuf,m_phbbuf,len);
    qrz_mul(m_buf,m_buf,m_buzenvbuf,len);
    qrz_add(buf,buf,m_buf,len); 
}

//////////////////////////////////////////////////

qrz_glitchiference::qrz_glitchiference(u32 len)
{
    m_buf=new s16[len];
    m_dutybuf=new s16[len];
    m_readpos=(s16*)bgGetGfxPtr(0)+1000;
}
    
qrz_glitchiference::~qrz_glitchiference()
{
    delete[] m_buf;
    delete[] m_dutybuf;
}

void qrz_glitchiference::trigger(u32 note, u32 type)
{
    m_dutyskip=note*2;
    m_readpos=(s16*)((bgGetGfxPtr(0)-100000)+note*128);
}

void qrz_glitchiference::run(s16 *buf, u32 len)
{
    m_readclk=qrz_tabread(m_buf,len,m_readclk,m_dutyskip,m_readpos);
    qrz_add(m_buf,m_buf,fx(1),len);
    m_dutyclk=qrz_tabread_mod(m_dutybuf,len,m_dutyclk,m_buf,qrz_sinetab);
    qrz_add(buf,buf,m_dutybuf,len);
}
