#include "custom_reflection_res_width.h"

#include "internal/assembly.h"
#include "internal/patch.h"
#include "internal/tickable.h"
#include "utils/ppcutil.h"

namespace custom_reflection_res_width {

int width = 640;

TICKABLE_DEFINITION((
        .name = "custom-reflection-resolution-width",
        .description = "Custom reflection resolution width",
        .active_value = width,
        .lower_bound = 1,
        .upper_bound = 640,
        .init_main_loop = init_main_loop, ))

// In the function which loads reflective stage objects, change the reflection resolution's width
// to our custom width
void init_main_loop() {
    width = *active_tickable_ptr->active_value;// Get our width
    patch::write_word(reinterpret_cast<void*>(0x8034aed8), PPC_INSTR_LI(PPC_R0, width));
}

}// namespace custom_reflection_res_width
