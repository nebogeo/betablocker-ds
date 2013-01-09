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
#include "thread.h"
#include "machine.h"

thread::thread() :
m_active(false),
m_start(0),
m_pc(0),
m_stack_pos(-1),
m_sound(NULL),
m_instrument(0),
m_pitch(0),
m_speed(1),
m_played_sound(-1)
{
    m_stack=new u8[STACK_SIZE];
	for (int n=0; n<STACK_SIZE; n++)
	{
		m_stack[n]=0;
	}
}

thread::~thread()
{
}
	
u8 thread::peek(machine *m, u8 addr) const
{
	return m->peek(m_start+addr);
}

void thread::poke(machine *m, u8 addr, u8 data)
{
	m->poke(m_start+addr,data);
}

void thread::run(machine *m, u32 clock)
{
	if (!m_active) return;
    if (clock%m_speed!=0) return;

    m_played_sound=-1;

	u8 instr=peek(m,m_pc);
	//iprintf("%d ",m_PC);
	//PrintInstr(instr);
	//iprintf("\n");
	m->set_instr(m_pc+m_start,true);
	m_pc++;

 	switch(instr)
	{
    case NOP: break;
    case ORG: m_start=m_start+m_pc-1; m_pc=1; break;
    case EQU: push(pop()==pop()); break;
    case JMP: m_pc=peek(m,m_pc++); break;
    case JMPZ: if (pop()==0) m_pc=peek(m,m_pc); else m_pc++; break;
    case PSHL: push(peek(m,m_pc++)); break;
    case PSH: push(peek(m,peek(m,m_pc++))); break;
    case PSHI: push(peek(m,peek(m,peek(m,m_pc++)))); break;
    case POP: poke(m,peek(m,m_pc++),pop()); break;
    case POPI: poke(m,peek(m,peek(m,m_pc++)),pop()); break;
    case ADD: push(pop()+pop()); break;
    case SUB: push(pop()-pop()); break;
    case INC: push(pop()+1); break;
    case DEC: push(pop()-1); break;
    case AND: push(pop()&pop()); break;
    case OR: push(pop()|pop()); break;
    case XOR: push(pop()^pop()); break;
    case NOT: push(~pop()); break;
    case ROR: push(pop()>>peek(m,m_pc++)); break;
    case ROL: push(pop()<<peek(m,m_pc++)); break;
    case PIP: 
    {
        u8 d=peek(m,m_pc++); 
        poke(m,d,peek(m,d)+1); 
    } break;
    case PDP: 
    {
        u8 d=peek(m,m_pc++); 
        poke(m,d,peek(m,d)-1); 
    } break;
    case DUP: push(top()); break;
    case NOTE: 
    {
        m_pitch=pop();
        m_played_sound=m_pitch;
#ifndef POLYSTYRENE
        m_sound->play(m_instrument,m_pitch); 
#else
        m->out(m_instrument,m_pitch);
#endif
    } break;
    case VOX: 
    {
        m_instrument=pop();
        m_played_sound=m_pitch;
#ifndef POLYSTYRENE
        m_sound->play(m_instrument,m_pitch); 
#else
//        m->out(m_instrument,m_pitch);
#endif
    } break;
    case STOP: /*m_active=false;*/ break;
    default : break;
	};
}
	
void thread::push(u8 data)
{
	if (m_stack_pos<STACK_SIZE-1)
	{
		m_stack[++m_stack_pos]=data;
	}
}

u8 thread::pop()
{
	if (m_stack_pos>=0)
	{
		u8 ret=m_stack[m_stack_pos];
		m_stack_pos--;
		return ret;
	}
	return 0;
}

u8 thread::top() const
{
	if (m_stack_pos>=0)
	{
		return m_stack[m_stack_pos];
	}
	return 0;
}

