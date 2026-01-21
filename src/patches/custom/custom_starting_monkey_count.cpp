#include "custom_starting_monkey_count.h"

#include "internal/assembly.h"
#include "internal/patch.h"
#include "internal/tickable.h"
#include "utils/ppcutil.h"

namespace custom_starting_monkey_count {

int monkey_count = 3;

TICKABLE_DEFINITION((
        .name = "custom-starting-monkey-count",
        .description = "Custom starting monkey count patch",
        .active_value = monkey_count,
        .lower_bound = 1,
        .upper_bound = 99,
        .init_main_loop = init_main_loop, ))

// In the function which sets the number of unlocked monkeys to 3 on boot, set it to our
// custom monkey count instead
void init_main_loop() {
    monkey_count = *active_tickable_ptr->active_value;// Get our monkey count
    patch::write_word(reinterpret_cast<void*>(0x803DD450), PPC_INSTR_LI(PPC_R0, monkey_count));
}

}// namespace custom_starting_monkey_count
