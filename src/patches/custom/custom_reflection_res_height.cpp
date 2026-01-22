#include "custom_reflection_res_height.h"

#include "internal/assembly.h"
#include "internal/patch.h"
#include "internal/tickable.h"
#include "utils/ppcutil.h"

namespace custom_reflection_res_height {

int height = 448;

TICKABLE_DEFINITION((
        .name = "custom-reflection-resolution-height",
        .description = "Custom reflection resolution height",
        .active_value = height,
        .lower_bound = 1,
        .upper_bound = 448,
        .init_main_loop = init_main_loop, ))

// In the function which loads reflective stage objects, change the reflection resolution's height
// to our custom height
void init_main_loop() {
    height = *active_tickable_ptr->active_value;// Get our height
    patch::write_word(reinterpret_cast<void*>(0x8034aee0), PPC_INSTR_LI(PPC_R0, height));
}

}// namespace custom_reflection_res_height
