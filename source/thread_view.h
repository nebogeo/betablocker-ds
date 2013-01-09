#include <nds.h>
#include "machine.h"

class thread_view
{
public:
    thread_view();
    ~thread_view();
    
    void update(const machine *m);

private:
    void load_tiles();
    void set_number(u8 x, u8 y, u8 v);
    void clear_number(u8 x, u8 y);

    int m_bg;
	u8* m_tile_mem;
	u16* m_map_mem;

    sImage m_tiles_image;
    sImage m_numbers_image;
};
