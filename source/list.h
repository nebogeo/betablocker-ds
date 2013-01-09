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

#include <stdlib.h>

#ifndef BB_LIST
#define BB_LIST

class list
{
public:
    list();
    ~list();

    class node
    {
    public:
        node() : m_next(NULL) {}
        node *m_next;
    };

    void clear();
    unsigned int size();
    node *last();
    void add_to_front(node* n);
    void add_to_end(node* n);
    void remove(node* n);

    static int unit_test();

    node* m_head;
};

#endif
