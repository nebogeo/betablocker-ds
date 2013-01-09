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

#include "machine.h"
#include "sprites.h"
#include "pointer.h"

#ifndef BB_CURSOR
#define BB_CURSOR

class cursor
{
public:

    cursor(machine  *m, sprites *s);
    ~cursor();

    bool touch(u32 x, u32 y, pointer *p);
    bool drag(u32 x, u32 y, u32 lx, u32 ly);
    void set_pos(u32 lx, u32 ly, u32 x, u32 y);
    void update(u32 src_x, u32 scr_y);
    bool menu_shown() { return m_menu_shown; }
    u32 get_x() { return m_x; }
    u32 get_y() { return m_y; }

    u32 get_cursor_addr() { return m_logical_x+m_logical_y*16; }

private:

    void inc();
    void dec();

    u32 m_x,m_y;
    u32 m_logical_x, m_logical_y;

    sprite *m_cursor;
    sprite *m_pointer;
    sprite *m_number;
    sprite *m_add_thread;
    sprite *m_del_thread;
    
    sprite **m_instr;
  
    machine *m_machine;
    bool m_menu_shown;

    enum mode{NONE,NUMBER,ADDR};

    mode m_mode;
    u32 m_number_addr;
    u32 m_number_y;

    float m_menu_expand;
    s8 m_menu_opening;
};

#endif
