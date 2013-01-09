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
#include "sprite.h"

#ifndef BB_SPRITES
#define BB_SPRITES

class sprites
{
public:

    sprites();
    ~sprites();

    void load(u8 *data, u8 sprite_size, u8 sprites_w, u8 sprites_h);
    void add(sprite *s);
    void update();
    
private:

    //SpriteEntry m_oam_copy[128];
    list m_sprites;
    int m_next_oam;
    u16* m_gfx_base;

};

#endif
