#include "unlock_master.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "mkb/mkb.h"

namespace unlock_master {

TICKABLE_DEFINITION((
        .name = "master-unlocked-by-default",
        .description = "Master unlocked by default",
        .init_main_loop = init_main_loop, ))

void init_main_loop() {
    mkb::unlock_info_ptr->master_unlocked = 1;
}

}// namespace unlock_master
