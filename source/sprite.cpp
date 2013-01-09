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
#include "sprite.h"

sprite::sprite(s32 x, s32 y, u32 id) :
    m_x(x),
    m_y(y),
    m_dx(0),
    m_dy(0),
    m_scale(intToFixed(1,8)),
    m_angle(0),
    m_double_size(false),
    m_matrix(0),
    m_priority(0),
    m_gfx_id(id), 
    m_parent(NULL),
    m_hidden(false)
{
}

sprite::~sprite()
{
}

void sprite::hide(bool h)
{
    m_hidden=h;
}

bool sprite::hidden()
{
    return m_hidden;
}

bool sprite::intersect(s32 x, s32 y)
{
    if (hidden()) return false;

    s32 px=m_x;
    s32 py=m_y;

    if (m_parent!=NULL)
    {
        px+=m_parent->m_x;
        py+=m_parent->m_y;
    }

    return (x<px+32 && x>px &&
            y<py+32 && y>py);
}

void sprite::update(u32 id, u16 *gfx_base)
{
    m_x+=m_dx;
    m_y+=m_dy;

    s32 x=m_x;
    s32 y=m_y;

    if (m_parent!=NULL)
    {
        x+=m_parent->m_x;
        y+=m_parent->m_y;
    }

    bool clip=(x<0-16 || y<0-16 || x>256-16 || y>192-16);

    if (m_double_size)
    {
        x-=16; y-=16;
    }

    oamSet(&oamMain, //main graphics engine context
           id,       //oam index (0 to 127)  
           x, y,     //x and y pixle location of the sprite
           m_priority,  //priority, lower renders last (on top)
           0,			//this is the palette index if multiple palettes or the alpha value if bmp sprite	
           SpriteSize_32x32,     
           SpriteColorFormat_256Color, 
           &gfx_base[m_gfx_id*32*8*2],  //pointer to the loaded graphics
           m_matrix,                  //sprite rotation/scale matrix index 
           m_double_size,               //double the size when rotating?
           m_hidden | clip,			//hide the sprite?
           false, false, //vflip, hflip
           false	//apply mosaic
        );       

    oamRotateScale(&oamMain,m_matrix,m_angle,m_scale,m_scale);
}
