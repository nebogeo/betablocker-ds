// Copyright (C) 2011 Dave Griffiths
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

#include "machine.h"
#include "thread_view.h"
#include "tiles_pcx.h"
#include "numbers_pcx.h"

thread_view::thread_view() :
    m_bg(bgInitSub(0, BgType_Text8bpp, BgSize_T_256x256, 0,1)),
    m_tile_mem((u8*)bgGetGfxPtr(m_bg)),
    m_map_mem((u16*)bgGetMapPtr(m_bg))
{
    // load the tile images
	loadPCX((u8*)tiles_pcx, &m_tiles_image);
	imageTileData(&m_tiles_image);
    loadPCX((u8*)numbers_pcx, &m_numbers_image);
    imageTileData(&m_numbers_image);

    // setup the palette
	for(int i=0; i<256; i++)
    {
		BG_PALETTE_SUB[i] = m_numbers_image.palette[i];
    }

    BG_PALETTE_SUB[0] = RGB15(0,0,0);

    // load the tiles into tile mem
	load_tiles();
}

thread_view::~thread_view()
{
}

void thread_view::load_tiles()
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

void thread_view::set_number(u8 x, u8 y, u8 v)
{
    u8 w=16;
    u32 num_ofs=25*4;
    u32 toprow=(y*w*2+x)*2;
    u32 botrow=toprow+w*2;
    // splice together the tiles for the numbers
    u8 l=v&0x0f;
    u8 u=v>>4;
    u16 ltile=num_ofs+1+l*4;
    u16 utile=num_ofs+u*4;
    m_map_mem[toprow]=utile;
    m_map_mem[toprow+1]=ltile;
    m_map_mem[botrow]=utile+2;
    m_map_mem[botrow+1]=ltile+2;
}

void thread_view::clear_number(u8 x, u8 y)
{
    u8 w=16;
    u32 toprow=(y*w*2+x)*2;
    u32 botrow=toprow+w*2;
    m_map_mem[toprow]=0;
    m_map_mem[toprow+1]=0;
    m_map_mem[botrow]=0;
    m_map_mem[botrow+1]=0;
}

void thread_view::update(const machine *m)
{
    const thread* thr=m->get_thread_const(0);
    set_number(0,10,thr->get_sound()->bank());

    u32 pos=0;
    for (int t=0; t<MAX_THREADS; t++)
    {
        const thread* thr=m->get_thread_const(t);
        clear_number(0,t);
        clear_number(1,t);

        if (thr->is_active())
        {
            set_number(0,pos,thr->get_pc());

            //s32 sound=thr->played_sound();
            //if (sound>-1) set_number(0,pos,sound);
 
            const u8* s=thr->get_stack();
            for (int i=0; i<STACK_SIZE; i++)
            {
                if (i<thr->get_stack_pos())
                {
                    set_number(i+2,pos,s[i]);
                }
                else
                {
                    clear_number(i+2,pos);
                }
            }
            pos++;
        }
    }
}

