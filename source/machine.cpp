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
#include "machine.h"

machine::machine(sound *s):
    m_recycle_thread(0),
    m_clock(0)
{
    m_heap = new u8[HEAP_SIZE];
    m_instruction = new bool[HEAP_SIZE];
    m_threads = new thread[MAX_THREADS];

	for (u32 n=0; n<HEAP_SIZE; n++)
	{
		m_heap[n]=0;
	}

	for (u32 n=0; n<HEAP_SIZE; n++)
	{
		m_instruction[n]=false;
	}

	for (u8 n=0; n<MAX_THREADS; n++)
	{
		m_threads[n].set_sound(s);
	}
}

machine::~machine()
{
}

u8 machine::peek(u32 addr) const
{
	return m_heap[addr%HEAP_SIZE];
}

void machine::poke(u32 addr, u8 data)
{	
	m_heap[addr%HEAP_SIZE]=data;
}

bool machine::is_instr(u32 addr) const
{
	return m_instruction[addr%HEAP_SIZE];
}

void machine::set_instr(u32 addr, bool s)
{
	m_instruction[addr%HEAP_SIZE]=s;
}

void machine::run()
{
	for (int n=0; n<MAX_THREADS; n++)
	{
		m_threads[n].run(this,m_clock);
	}

    m_clock++;
}

void machine::dump() const
{
	for (u32 n=0; n<HEAP_SIZE; n++)
	{
		iprintf("%d ",m_heap[n]);
	}
	iprintf("\n");
}


void machine::print_instr(u8 instr) const
{
	switch(instr)
	{
		case NOP:  iprintf("nop"); break;
		case ORG:  iprintf("org"); break;
		case EQU:  iprintf("equ"); break;
		case JMP:  iprintf("jmp"); break;
		case JMPZ: iprintf("jmpz"); break;
		case PSHL: iprintf("pshl"); break;
		case PSH:  iprintf("psh"); break;
		case PSHI: iprintf("pshi"); break;
		case POP:  iprintf("pop"); break;
		case POPI: iprintf("popi"); break;
		case ADD:  iprintf("add"); break;
	    case SUB:  iprintf("sub"); break;
		case INC:  iprintf("inc"); break;
		case DEC:  iprintf("dec"); break;
		case AND:  iprintf("and"); break;
		case OR:   iprintf("or"); break;
		case XOR:  iprintf("xor"); break;
		case NOT:  iprintf("not"); break;
		case ROR:  iprintf("ror"); break;
		case ROL:  iprintf("rol"); break;
		case PIP:  iprintf("pip"); break;
		case PDP:  iprintf("pdp"); break;
		case DUP: iprintf("dup"); break;
		case NOTE: iprintf("note"); break;
		case VOX:  iprintf("vox"); break;
		case STOP: iprintf("stop"); break;
		default: iprintf("%d", instr); break;
	};
}

u32 machine::find_previous_org(u32 addr) const
{
    // find previous org to be relative to
    u32 org=0;
    bool found=false;
    u32 i=0;
    while(i<256 && !found) 
    {
        if (peek(addr-i)==ORG)
        {
            org=addr-i;
            found=true;
        }
        i++;
    }
    return org;
}

u32 machine::get_free_thread()
{
    // first look for a free one
    for (u32 i=0; i<MAX_THREADS; i++)
    {
        if (!get_thread(i)->is_active())
        {
            return i;
        }
    }
    
    // otherwise grab a used one
    u32 ret=m_recycle_thread;
    m_recycle_thread++;
    m_recycle_thread=m_recycle_thread%MAX_THREADS;
    return ret;
}

void machine::add_thread(u32 addr)
{
    u32 t=get_free_thread();
	m_threads[t].set_active(true);
    m_threads[t].set_start(addr);
    m_threads[t].set_pc(0);
}

s32 machine::find_closest_thr(u32 addr)
{
    // look for the closest thread in the machine from the address
    u32 closest=-1;
    s32 chosen=-1;

    for (u32 i=0; i<MAX_THREADS; i++)
    {
        thread *thr=&m_threads[i];
        if (thr->is_active())
        {
            u32 dist=abs(thr->get_pc()-addr);
            if (dist<closest)
            {
                closest=dist;
                chosen=i;
            }
        }
    }
    return chosen;
}

void machine::del_thread(u32 addr)
{
    s32 chosen=find_closest_thr(addr);
    if (chosen!=-1)
    {
        // turn it off
        m_threads[chosen].set_active(false);
    }
}

void machine::speed_up_closest(u32 addr)
{
    s32 chosen=find_closest_thr(addr);
    if (chosen!=-1)
    {
        // speed up
        m_threads[chosen].set_speed(
            m_threads[chosen].get_speed()+1);
    }
}

void machine::slow_down_closest(u32 addr)
{
    s32 chosen=find_closest_thr(addr);
    if (chosen!=-1)
    {
        // speed up
        m_threads[chosen].set_speed(
            m_threads[chosen].get_speed()-1);
    }
}
