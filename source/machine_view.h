#include <nds.h>
#include "sprites.h"
#include "sprite.h"
#include "machine.h"

class machine_view
{
public:
    machine_view(sprites *spr);
    ~machine_view();

    void update(u32 x, u32 y, const machine *m);

private:
    void load_tiles();

    int m_bg;
	u8* m_tile_mem;
	u16* m_map_mem;

    sImage m_tiles_image;
    sImage m_numbers_image;
    
    sprite **m_threads;
    sprite **m_triggers;
    u32 m_next_trigger;

};
