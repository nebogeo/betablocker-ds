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

#include "pointer.h"
#include <stdio.h>

pointer::pointer(sprites *s):
    m_next_pointer(0),
    m_time(0)
{
    m_pointers=new u8[NUM_POINTERS];    
    for (u32 i=0; i<NUM_POINTERS; i++)
    {
        m_pointers[i]=0;
    }

    m_sprites=new sprite**[NUM_POINTERS];
    for (u32 i=0; i<NUM_POINTERS; i++)
    {
        m_sprites[i]=new sprite*[SPR_PER_POINTER];
        for (u32 n=0; n<SPR_PER_POINTER; n++)
        {
            m_sprites[i][n]=new sprite(10,20,31);
            s->add(m_sprites[i][n]);
            m_sprites[i][n]->hide(true);
        }
    }
}

pointer::~pointer()
{
}

void pointer::add(u8 addr)
{
    for (u32 n=0; n<SPR_PER_POINTER; n++)
    {
        m_sprites[m_next_pointer][n]->hide(false);
    }
    m_pointers[m_next_pointer]=addr;
    m_next_pointer=(m_next_pointer+1)%NUM_POINTERS;
}

void pointer::update(u32 x, u32 y, machine *m)
{
    m_time=(m_time+1)%256;
    for (u32 i=0; i<NUM_POINTERS; i++)
    {
        u8 from=m_pointers[i];
        u8 to=m->peek(from)+m->find_previous_org(from);
        /*iprintf("%d from %d to %d %d prev %d\n", i, from, to, 
                m->peek(from),
                m->find_previous_org(from));*/

        u32 fx=(from%16)*16;
        u32 fy=(from/16)*16;
        u32 tx=(to%16)*16;
        u32 ty=(to/16)*16;

        for (u32 n=0; n<SPR_PER_POINTER; n++)
        {
            float t=n/(float)SPR_PER_POINTER;
            t+=((m_time%10)/(float)10)/(float)SPR_PER_POINTER;
            u32 xx=tx*t+fx*(1-t);          
            u32 yy=ty*t+fy*(1-t);

            m_sprites[i][n]->m_x=xx-x-8;
            m_sprites[i][n]->m_y=yy-y-8;
        }
    }
}

