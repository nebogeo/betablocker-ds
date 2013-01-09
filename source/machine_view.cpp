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
#include <stdio.h>
#include "machine.h"
#include "machine_view.h"
#include "tiles_pcx.h"
#include "numbers_pcx.h"

#define NUM_TRIGGERS 10

machine_view::machine_view(sprites *spr) :
    m_bg(bgInit(0, BgType_Text8bpp, BgSize_T_512x512, 0,1)),
    m_tile_mem((u8*)bgGetGfxPtr(m_bg)),
    m_map_mem((u16*)bgGetMapPtr(m_bg)),
    m_next_trigger(0)
{
    bgSetPriority(0,2);

    iprintf("bg=%d\n",m_bg);

    // load the tile images
	loadPCX((u8*)tiles_pcx, &m_tiles_image);
	imageTileData(&m_tiles_image);
    loadPCX((u8*)numbers_pcx, &m_numbers_image);
    imageTileData(&m_numbers_image);

    // setup the palette
	for(int i=0; i<256; i++)
    {
		BG_PALETTE[i] = m_numbers_image.palette[i];
    }

    BG_PALETTE[0] = RGB15(0,0,0);

    // load the tiles into tile mem
	load_tiles();

    m_threads = new sprite*[MAX_THREADS];
    for (int i=0; i<MAX_THREADS; i++)
    {
        m_threads[i]=new sprite(0,0,30);
        spr->add(m_threads[i]);
    }

    m_triggers = new sprite*[NUM_TRIGGERS];
    for (int i=0; i<NUM_TRIGGERS; i++)
    {
        m_triggers[i]=new sprite(0,0,0);
        m_triggers[i]->m_double_size=true;
        m_triggers[i]->m_matrix=1+i;
        m_triggers[i]->m_priority=1;
        m_triggers[i]->hide(true);
        spr->add(m_triggers[i]);
    }

}

machine_view::~machine_view()
{
}

void machine_view::load_tiles()
{
    // tile layout is:
    //  0  1  2  3  4  5  6  7 
    //  8  9 10 11 12 13 14 15 
    // 16 17 18 19 20 21 22 23 
    // 24 25
    //  0  1  2  3  4  5  6  7  
    //  8  9 10 11 12 13 14 15

    // each bb tile comprises 4 8X8 nds tiles

	//copy the tiles into tile memory
	int pos=0;

    // first the instruction tiles
    for (int i=0; i<25; i++)
    {
        u8 rowi=i/8;
        u16* toprow=m_tiles_image.image.data16+(i+(rowi*8))*64;
        u16* botrow=toprow+(64*8);

        swiCopy(toprow, m_tile_mem+64*pos++, 32);
        swiCopy(toprow+32, m_tile_mem+64*pos++, 32);
        swiCopy(botrow, m_tile_mem+64*pos++, 32);
        swiCopy(botrow+32, m_tile_mem+64*pos++, 32);
    }

    // then the number tiles
    for (int i=0; i<32; i++)
    {
        u8 rowi=i/8;
        u16* toprow=m_numbers_image.image.data16+(i+((rowi%2)*8))*64;
        u16* botrow=toprow+(64*8);

        swiCopy(toprow, m_tile_mem+64*pos++, 32);
        swiCopy(toprow+32, m_tile_mem+64*pos++, 32);
        swiCopy(botrow, m_tile_mem+64*pos++, 32);
        swiCopy(botrow+32, m_tile_mem+64*pos++, 32);
    }
}

void machine_view::update(u32 x, u32 y, const machine *m)
{
    for (int i=0; i<MAX_THREADS; i++)
    {
        const thread *thr=m->get_thread_const(i);
        m_threads[i]->hide(!thr->is_active());
        int pc=thr->get_pc()%256;
        m_threads[i]->m_x=(pc%16)*16-x-8;
        m_threads[i]->m_y=(pc/16)*16-y-8;

        if (thr->played_sound()!=-1)
        {
            int pc=(thr->get_pc()-1)%256;
            m_triggers[m_next_trigger]->m_x=(pc%16)*16-x-8;
            m_triggers[m_next_trigger]->m_y=(pc/16)*16-y-8;
            m_triggers[m_next_trigger]->m_scale=intToFixed(2,8);
            m_triggers[m_next_trigger]->hide(false);
            m_next_trigger=(m_next_trigger+1)%NUM_TRIGGERS;
        }
    }

    for (int i=0; i<NUM_TRIGGERS; i++)
    {
        if (m_triggers[i]->m_scale>0)
        {
            m_triggers[i]->m_scale*=0.9;
            if (m_triggers[i]->m_scale==0)
            {
                m_triggers[i]->hide(true);
            }
        }
    }

    u8 w=16;
    u8 h=16;
    u32 addr=0;
    u32 num_ofs=25*4;
    for (int y=0; y<h; y++)
    {
        for (int x=0; x<w; x++)
        {
            u32 toprow=(y*w*2+x)*2;
            u32 botrow=toprow+w*2;
            
            if (m->is_instr(addr) && m->peek(addr)<25)
            {
                // instruction tiles are easy
                u32 tile=m->peek(addr)*4;
                m_map_mem[toprow]=tile;
                m_map_mem[toprow+1]=tile+1;
                m_map_mem[botrow]=tile+2;
                m_map_mem[botrow+1]=tile+3;
            }
            else
            {
                // splice together the tiles for the numbers
                u8 v=m->peek(addr);
                u8 l=v&0x0f;
                u8 u=v>>4;
                u32 ltile=num_ofs+1+l*4;
                u32 utile=num_ofs+u*4;
                m_map_mem[toprow]=utile;
                m_map_mem[toprow+1]=ltile;
                m_map_mem[botrow]=utile+2;
                m_map_mem[botrow+1]=ltile+2;
            }
            addr++;
        }
    }
}

