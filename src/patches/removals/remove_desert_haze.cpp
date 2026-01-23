#include "remove_desert_haze.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "utils/ppcutil.h"

namespace remove_desert_haze {

TICKABLE_DEFINITION((
        .name = "remove-desert-haze",
        .description = "Desert haze removal",
        .init_main_loop = init_main_loop, ))

// TODO: Probably not the best way to implement this, will need to look into a
// proper fix soon. In a function that sets a parameter that enables heat
// haze for the specific desert theme ID, the theme ID is compared to 0xffff
// instead of 0x7.
void init_main_loop() {
    patch::write_word(reinterpret_cast<void*>(0x802e4ed8), PPC_INSTR_CMPWI(PPC_R0, -1));
}

}// namespace remove_desert_haze
