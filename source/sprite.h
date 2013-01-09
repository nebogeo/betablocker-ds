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
#include "list.h"

#ifndef BB_SPRITE
#define BB_SPRITE

class sprite : public list::node
{
public:

    sprite(s32 x, s32 y, u32 id);
    ~sprite();

    void update(u32 id, u16 *gfx_base);
    void parent(sprite *s) { m_parent=s; }
    bool intersect(s32 x, s32 y);
    void hide(bool h);
    bool hidden();

    s32 m_x,m_y;
    s32 m_dx,m_dy;
    int m_scale;
    int m_angle;
    bool m_double_size;
    u32 m_matrix;
    u32 m_priority;
    
private:
    
	u32 m_gfx_id; 		// graphics location
    sprite *m_parent;
    bool m_hidden;
};

#endif
