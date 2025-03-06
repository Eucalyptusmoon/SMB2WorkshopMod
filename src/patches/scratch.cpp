#include "scratch.h"
#include "internal/tickable.h"

namespace scratch {

TICKABLE_DEFINITION((
        .name = "scratch",
        .description = "Scratch",
        .enabled = false,
        .init_main_loop = init_main_loop,
        .init_main_game = init_main_game,
        .init_sel_ngc = init_sel_ngc,
        .init_exoption = init_exoption,
        .tick = tick))

void init_main_loop() {}
void init_main_game() {}
void init_sel_ngc() {}
void init_exoption() {}
void tick() {}

}// namespace scratch
