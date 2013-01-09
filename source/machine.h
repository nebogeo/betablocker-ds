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

#ifndef POLYSTYRENE
#include <nds.h>
#include "sound.h"
#else
#include "types.h"

#endif

#include "thread.h"

#ifndef BB_MACHINE
#define BB_MACHINE

static const u8 MAX_THREADS=8;
static const u32 HEAP_SIZE=256;

class machine
{
public:
	machine(sound* s);
	~machine();

	// global
	u8 peek(u32 addr) const;
	void poke(u32 addr, u8 data);
	bool is_instr(u32 addr) const;
	void set_instr(u32 addr,bool s);
	void print_instr(u8 instr) const;

	void run();
	void dump() const;

    u32 find_previous_org(u32 addr) const;
	
    void add_thread(u32 addr);
    void del_thread(u32 addr);
	thread *get_thread(int t) { return &m_threads[t]; } 
	const thread *get_thread_const(int t) const { return &m_threads[t]; } 
    s32 find_closest_thr(u32 addr);

    u32 get_clock() { return m_clock; }
    void speed_up_closest(u32 addr);
    void slow_down_closest(u32 addr);

#ifdef POLYSTYRENE
    void out(u8 inst, u8 pitch)
    {
        if (m_outpos<1024) m_output[m_outpos++]=pitch;
    }
    u8 m_output[1024];
    u8 m_outpos;
#endif
	
private:
	u32 get_free_thread();

	thread *m_threads;
	u8 *m_heap;
	bool *m_instruction;
    u32 m_recycle_thread;
    u32 m_clock;
};

#endif
