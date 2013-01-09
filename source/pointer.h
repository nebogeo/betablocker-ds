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

#include "sprites.h"
#include "machine.h"

#ifndef BB_POINTER
#define BB_POINTER

#define NUM_POINTERS 5
#define SPR_PER_POINTER 10

class pointer
{
public:
    pointer(sprites *s);
    ~pointer();

    void add(u8 addr);
    void update(u32 x, u32 y, machine *m);

private:
    u8 *m_pointers;
    u32 m_next_pointer;
    sprite ***m_sprites;
    u32 m_time;
};

#endif
