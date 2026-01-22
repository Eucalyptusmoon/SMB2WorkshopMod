#include "enable_autosave.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "mkb/mkb.h"

namespace enable_autosave {

TICKABLE_DEFINITION((
        .name = "autosave-on-by-default",
        .description = "Autosave on by default",
        .init_main_loop = init_main_loop, ))

void init_main_loop() {
    mkb::autosave = true;
}

}// namespace enable_autosave
