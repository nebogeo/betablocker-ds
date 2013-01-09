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
#include "sprites.h"

sprites::sprites() :
    m_next_oam(0),
    m_gfx_base(NULL)
{
    oamInit(&oamMain, SpriteMapping_1D_32, false);
}

sprites::~sprites()
{
}

// only square sprites, all the same size
void sprites::load(u8 *data, u8 sprite_w_in_tiles, u8 sprites_w, u8 sprites_h)
{
	sImage image;
	//load our ball pcx file into an image
	loadPCX(data, &image);
	//tile it so it is usefull as sprite data
	imageTileData(&image);

	// Sprite initialisation
	for(int i=0; i<256; i++)
    {
		SPRITE_PALETTE[i] = image.palette[i];
    }
   
    u32 pos=0;
    u32 tiles_in_sprite=sprite_w_in_tiles*sprite_w_in_tiles;
    u32 image_stride_in_tiles=sprite_w_in_tiles*sprites_w;
    m_gfx_base = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);

    for (int y=0; y<sprites_h; y++)
    {
        for (int x=0; x<sprites_w; x++)
        {
            //iprintf("sprite %d %d\n",x,y);
            u32 row_tile_start = x*sprite_w_in_tiles+y*tiles_in_sprite*sprites_w;
            //iprintf("start:%d\n", pos);
            
            for (int tile_row=0; tile_row<sprite_w_in_tiles; tile_row++)
            {
                u32 row_tile=row_tile_start+tile_row*image_stride_in_tiles;
                
                u32 p=row_tile*8*4;
                for(int i=0; i<sprite_w_in_tiles*8*4; i++)
                {
                    m_gfx_base[pos++] = image.image.data16[p+i];
                }
            }
        }
    }
}

void sprites::add(sprite *s)
{
    m_sprites.add_to_end(s);
}

void sprites::update() 
{
    sprite* s=static_cast<sprite*>(m_sprites.m_head);

    u32 id=0;
    while(s!=NULL)
    {
        s->update(id,m_gfx_base);
        s=static_cast<sprite*>(s->m_next);
        id++;
    }

	//memcpy(OAM, m_oam_copy, 128 * sizeof(SpriteEntry));

    oamUpdate(&oamMain);
}

