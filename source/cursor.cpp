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
#include <math.h>
#include "cursor.h"
#include "sprites_pcx.h"
#include "pointer.h"

cursor::cursor(machine *m, sprites *s):
    m_machine(m),
    m_menu_shown(false),
    m_mode(NONE),
    m_number_addr(0),
    m_number_y(0),
    m_menu_expand(0),
    m_menu_opening(0)
{
    s->load((u8*)sprites_pcx,4,8,4);	
    m_cursor=new sprite(0,0,27);
    m_number=new sprite(12,-30,25);
    m_pointer=new sprite(-12,-30,26);
    m_add_thread=new sprite(-12,30,28);
    m_del_thread=new sprite(12,30,29);

    m_number->parent(m_cursor);
    m_pointer->parent(m_cursor);
    m_add_thread->parent(m_cursor);
    m_del_thread->parent(m_cursor);

    s->add(m_cursor);
    s->add(m_number);
    s->add(m_pointer);
    s->add(m_add_thread);
    s->add(m_del_thread);

    m_instr = new sprite*[25];

    for (int i=1; i<25; i++)
    {
        //float a=M_PI+i*((M_PI*2)/25);
        //     m_instr[i]=new sprite(70*sin(-a),70*cos(-a),i);
        int ii=i-1;
        if (ii>11) ii++;
        m_instr[i]=new sprite((ii%5)*32-(64),(ii/5)*32-(64),i);
        m_instr[i]->parent(m_cursor);
        m_instr[i]->m_scale=1<<5;
        s->add(m_instr[i]);
        m_instr[i]->hide(true);
    }

}

cursor::~cursor()
{
}

bool cursor::drag(u32 x, u32 y, u32 lx, u32 ly)
{
    if (m_mode==NUMBER)
    {
        int inc=(y-m_number_y);
        m_machine->poke(m_number_addr,inc);
        m_machine->set_instr(m_number_addr,false);
        return true;
    }

    if (m_mode==ADDR)
    {
        m_machine->poke(m_number_addr,(lx+ly*16)-
                        m_machine->find_previous_org(m_number_addr));
        m_machine->set_instr(m_number_addr,false);
        return true;
    }
    return false;
}

void cursor::inc()
{
    m_logical_x++;
    if (m_logical_x>=16) 
    {
        m_logical_x=0;
        m_logical_y++;
        m_x=0;
        if (m_logical_y>=16)
        {
            m_logical_y=0;
            m_y=0;
        }
        else
        {
            m_y+=16;
        }
    }
    else
    {
        m_x+=16;
    }
}

void cursor::dec()
{
    if (m_logical_x==0)
    {
        m_logical_x=15;
        m_x=15*16;
        if (m_logical_y==0)
        {
            m_logical_y=15;
            m_y=15*16;
        }
        else
        {
            m_logical_y--;
            m_y-=16;
        }
    }
    else
    {
        m_logical_x--;
        m_x-=16;
    }
}

bool cursor::touch(u32 x, u32 y, pointer *p)
{
    if (m_mode==NUMBER)
    {
        m_mode=NONE;
        m_number_addr=0;
        m_number_y=0;
    }
    else if (m_number->intersect(x,y)) 
    {
        m_mode=NUMBER;
        m_number_addr=m_logical_x+m_logical_y*16;
        m_number_y=y;
        return true;
    }

    if (m_mode==ADDR)
    {
        m_mode=NONE;
        m_number_addr=0;
    }
    else if (m_pointer->intersect(x,y))
    {
        m_mode=ADDR;
        m_number_addr=m_logical_x+m_logical_y*16;
        p->add(m_number_addr);
        return true;
    }

    if (m_add_thread->intersect(x,y)) 
    {
        m_machine->add_thread(m_logical_x+m_logical_y*16);
        return true;
    }

    if (m_del_thread->intersect(x,y)) 
    {
        m_machine->del_thread(m_logical_x+m_logical_y*16);
        return true;
    }

    if (m_cursor->intersect(x,y)) 
    {
        if (m_menu_shown)
        {
            m_number->hide(false);
            m_pointer->hide(false);
            m_add_thread->hide(false);
            m_del_thread->hide(false);
            m_menu_shown=false;      
            m_menu_opening=-1;
        }
        else
        {
            for (int i=0; i<25; i++) m_instr[i]->hide(false);
            m_number->hide(true);
            m_pointer->hide(true);
            m_add_thread->hide(true);
            m_del_thread->hide(true);
            m_menu_shown=true;
            m_menu_opening=1;
        }
        return true;
    }

    if (m_menu_shown)
    {
        for (int i=0; i<25; i++)
        {
            if (m_instr[i]->intersect(x,y))
            {
                m_machine->print_instr(i); iprintf("\n");
                m_machine->poke(m_logical_x+m_logical_y*16,i);
                m_machine->set_instr(m_logical_x+m_logical_y*16,true);
                inc();
                return true;
            }
        }
        
        if (x>128)
        {
            m_machine->poke(m_logical_x+m_logical_y*16,0);
            m_machine->set_instr(m_logical_x+m_logical_y*16,false);                
            inc();
        }
        else
        {
            dec();
        }
    }

    if (m_menu_shown) return true;
    return false;
}

void cursor::set_pos(u32 lx,u32 ly,u32 x,u32 y)
{
    m_logical_x=lx;
    m_logical_y=ly;
    m_x=x;
    m_y=y;
}

void cursor::update(u32 scr_x, u32 scr_y)
{
    m_cursor->m_x = m_x-scr_x-8;
    m_cursor->m_y = m_y-scr_y-8;

    if (m_menu_opening>0 && m_menu_expand<1.0)
    {
        m_menu_expand+=0.05;
    }

    if (m_menu_opening<0 && m_menu_expand>0.0)
    {
        m_menu_expand-=0.05;
    }

    if (m_menu_opening<0 && m_menu_expand<0.01)
    {
        for (int i=0; i<25; i++) m_instr[i]->hide(true);
        m_menu_opening=0;
    }

    for (int i=1; i<25; i++)
    {
        //float a=M_PI+i*((M_PI*2)/25);
        //     m_instr[i]=new sprite(70*sin(-a),70*cos(-a),i);
        int ii=i-1;
        if (ii>11) ii++;
        m_instr[i]->m_x=((ii%5)*32-(64))*m_menu_expand;
        m_instr[i]->m_y=((ii/5)*32-(64))*m_menu_expand;
    }


    /* m_pointer->m_x=m_cursor->m_x;
    m_pointer->m_y=m_cursor->m_y+30;
    m_number->m_x=m_cursor->m_x;
    m_number->m_y=m_cursor->m_y-30;*/
}
