// Copyright (C) 2011 Dave Griffiths
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

#ifndef QRZ_TUNING
#define QRZ_TUNING

#define SKIPLUT_SIZE (12*12)
// returns 8 octaves worth of sample skips
u32 *qrz_get_skiplut(u32 tablelen, u32 samplerate);
u32 qrz_scale_lu(u32 note, u32 *scale, u32 scale_size);

#endif
