#pragma once

#include "mkb/mkb.h"

namespace relutil {

/*
 * Returns one past the last address of relocation data in mainloop.rel which we are free to
 * overwrite.
 */
void* compute_mainloop_reldata_boundary(void* start);

/*
 * Adjusts a pointer to account for differences in REL load locations compared to vanilla.
 * Symbols listed in mkb2.us.lst are relocated by elf2rel and do not need this.
 */
void* relocate_addr(u32 vanilla_addr);

}// namespace relutil
