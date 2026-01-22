#include "custom_banana_count_per_1up.h"

#include "internal/assembly.h"
#include "internal/patch.h"
#include "internal/tickable.h"
#include "utils/ppcutil.h"

namespace custom_banana_count_per_1up {

int banana_count = 100;

TICKABLE_DEFINITION((
        .name = "custom-banana-count-per-1up",
        .description = "Custom bananas per 1UP patch",
        .active_value = banana_count,
        .lower_bound = 1,
        .upper_bound = 999,
        .init_main_loop = init_main_loop, ))

// In add_bananas, change the amount of bananas to add a life + subtract to our custom count
void init_main_loop() {
    banana_count = *active_tickable_ptr->active_value;// Get our banana count
    patch::write_word(reinterpret_cast<void*>(0x802B8240), PPC_INSTR_CMPWI(PPC_R0, banana_count));
    patch::write_word(reinterpret_cast<void*>(0x802B8258), PPC_INSTR_SUBI(PPC_R0, PPC_R3, banana_count));
}

}// namespace custom_banana_count_per_1up
