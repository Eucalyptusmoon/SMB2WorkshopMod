#include "fix_stobj_reflection.h"

#include "internal/assembly.h"
#include "internal/patch.h"
#include "internal/relutil.h"
#include "internal/tickable.h"
#include "utils/ppcutil.h"

namespace main {
extern "C" {
void* reflection_draw_state_addr;
void* reflection_draw_stage_skip_addr;
void* reflection_view_stage_skip_addr;
}
}// namespace main

namespace fix_stobj_reflection {

TICKABLE_DEFINITION((
        .name = "fix-stage-object-reflection",
        .description = "Stobj reflection flag support",
        .init_main_loop = init_main_loop,
        .init_main_game = init_main_game, ))

// Disables the check that prevents stage objects from being reflected,
// then runs an additional check against the stage object's model flags to
// determine if the proper flag was set. TODO: Maybe make this more elegant?
// 0x38000000 = li r0, 0
void init_main_loop() {
    main::reflection_draw_state_addr = relutil::relocate_addr(0x8054DCE0);
    main::reflection_draw_stage_skip_addr = relutil::relocate_addr(0x802C9540);
    patch::write_word(relutil::relocate_addr(0x802ca480), PPC_INSTR_LI(PPC_R0, 0x0));
    patch::write_branch_bl(relutil::relocate_addr(0x802c9434),
                           reinterpret_cast<void*>(main::reflection_draw_stage_hook));
}

// Checks the stage object's model flag to determine if the proper flag is set
// during the 'view stage' sequence.
void init_main_game() {
    main::reflection_view_stage_skip_addr = relutil::relocate_addr(0x80913F5C);
    patch::write_branch_bl(relutil::relocate_addr(0x80913F34),
                           reinterpret_cast<void*>(main::reflection_view_stage_hook));
}

}// namespace fix_stobj_reflection
