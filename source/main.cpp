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

#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "machine.h"
#include "thread.h"
#include "sound.h"

#include "soundbank.h"
#include "soundbank_bin.h"

#include "list.h"
#include "sprites.h"
#include "machine_view.h"
#include "thread_view.h"
#include "cursor.h"

//#define BBDEBUG

void sound_check(machine &m)
{
    m.poke(0,ORG);
    m.poke(1,PSHL);
    m.poke(2,0);
    m.poke(3,DUP);
    m.poke(4,VOX);
    m.poke(5,NOTE);
    m.poke(6,PIP);
    m.poke(7,2);
    m.poke(8,JMP);
    m.poke(9,0);
    m.add_thread(0);
}

int main(void) 
{
	mmInitDefaultMem((mm_addr)soundbank_bin);

	//set video mode and map vram to the background
	videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE | 
                 DISPLAY_SPR_1D_LAYOUT | 
                 DISPLAY_SPR_ACTIVE);
    
	vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
    vramSetBankB(VRAM_B_MAIN_SPRITE);

	//set video mode and map vram to the background
	videoSetModeSub(MODE_1_2D | DISPLAY_BG1_ACTIVE | 
                 DISPLAY_SPR_1D_LAYOUT | 
                 DISPLAY_SPR_ACTIVE);

	vramSetBankD(VRAM_D_MAIN_BG_0x06000000);
    vramSetBankE(VRAM_E_MAIN_SPRITE);

    lcdMainOnBottom();

#ifdef BBDEBUG
    consoleDemoInit();
#endif

    sound s;
    s.load_bank(0);

    sprites spr;
	machine m(&s);
    machine_view mv(&spr);
#ifndef BBDEBUG
    thread_view tv;
#endif
    cursor c(&m,&spr);
    pointer p(&spr);
    iprintf("%d\n",list::unit_test());

    int frame=0;
    int x=0;
    int y=0;
    int tx=0;
    int ty=0;
    int dx=0;
    int dy=0;
    int touchedlast=0;

    int tempo=2;
    int bank=0;
    u8 flip=0;
    u8 flip_count=1;
	irqEnable( IRQ_VCOUNT );

	while(1)
	{
		// wait until line 0
		swiIntrWait( 0, IRQ_VCOUNT);

        u32 t=tempo;
        //t=t/(flip+1);
        //iprintf("%d\n",t);
        
        if (tempo==0 || frame%t==0) 
        {
            m.run();
            /*    flip_count--;
            if (flip_count==0)
            {
                flip_count=4;
                if (flip) flip=0;
                else flip=1;
                }*/
        }
		scanKeys();
        frame++;
        touchPosition touch;
        touchRead(&touch);

        if (keysDown() & KEY_UP) m.slow_down_closest(c.get_cursor_addr());
        if (keysDown() & KEY_DOWN) m.speed_up_closest(c.get_cursor_addr());
        if (tempo<0) tempo=0;

        if (keysDown() & KEY_L) sound_check(m);

        if (keysDown() & KEY_LEFT) { bank--; s.load_bank(bank); }
        if (keysDown() & KEY_RIGHT) { bank++; s.load_bank(bank); }
 
        if(keysDown() & KEY_TOUCH)
        {
            tx=touch.px;
            ty=touch.py;
            if (!c.touch(tx,ty,&p))
            {
                touchedlast=1;
            }
        }
        else if(keysHeld() & KEY_TOUCH)
		{				
            if (!c.menu_shown() &&
                !c.drag(touch.px+x,touch.py+y,(touch.px+x)/16,(touch.py+y)/16))
            {
                if (touchedlast>0) touchedlast++;
                x+=tx-touch.px;
                y+=ty-touch.py;
                tx=touch.px;
                ty=touch.py;
            }
        }

        if (keysUp() & KEY_TOUCH)
        {
            if (touchedlast>0 && touchedlast<5)
            {
                int lx=(tx+x)/16;
                int ly=(ty+y)/16;
                c.set_pos(lx,ly,lx*16,ly*16);
            }
            touchedlast=0;
        }       

        if (keysUp() & KEY_A)
        {
            for (u32 i=0; i<HEAP_SIZE; i++)
            {
                m.poke(i,rand());
            }
        }
        
        c.update(x,y);
        if (c.menu_shown()) 
        {
            dx=c.get_x()-128;
            dy=c.get_y()-82;
            if (dx!=x) x+=(dx-x)*0.1;
            if (dy!=y) y+=(dy-y)*0.1; 
        }

        p.update(x,y,&m);
        REG_BG0HOFS = x;
        REG_BG0VOFS = y;
        spr.update();
        mv.update(x,y,&m);
        s.update();
#ifndef BBDEBUG
        tv.update(&m);
#endif
        BG_PALETTE_SUB[0]=13<<10;
		swiWaitForVBlank();
        BG_PALETTE_SUB[0]=31;

	}
		
	return 0;
}

///////////////////////////////////////////////////////////

int Pos2Index(int x, int y)
{
    return y*8+x;
}

int SafeIndex(u8 *data, int i)
{
    if (i>0 && i<64) return data[i];
    else return 0;
}

u8 *CompTiles(u8* a, u8 *b, int xoff)
{
    u8 *tmp = new u8[64];
    for(int x=0; x<8; x++)
    {
        for(int y=0; y<8; y++)
        {
            int i=Pos2Index(x,y);
            int o=Pos2Index(x-xoff,y);
            if (SafeIndex(a,o)==0) tmp[i]=b[i];
            else tmp[i]=SafeIndex(a,o);
        }
    }
    return tmp;
}
