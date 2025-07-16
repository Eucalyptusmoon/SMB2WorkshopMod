#include "remove_playpoints.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "mkb/mkb.h"

// Removes the playpoint notification screens when exiting from story mode or challenge mode, or after a 'game over'.
namespace remove_playpoints {

TICKABLE_DEFINITION((
        .name = "remove-playpoints",
        .description = "Playpoint removal patch",
        .init_main_game = init_main_game,
        .init_sel_ngc = init_sel_ngc,
        .tick = tick, ))

void init_main_game() {
    // Removes playpoint screen when exiting challenge/story mode.
    patch::write_nop(reinterpret_cast<void*>(0x808f9ecc));
    patch::write_nop(reinterpret_cast<void*>(0x808f9eec));

    // Removes playpoint screen after the 'game over' sequence.
    patch::write_nop(reinterpret_cast<void*>(0x808f801c));
    patch::write_nop(reinterpret_cast<void*>(0x808f803c));

    // Removes playpoint screen when saving game data in story mode.
    patch::write_nop(reinterpret_cast<void*>(0x80274c94));

    // Removes playpoint message on continue screen.
    patch::write_nop(reinterpret_cast<void*>(0x808f653c));
    patch::write_word(reinterpret_cast<void*>(0x803dcae0), 0x38600001);
}

void init_sel_ngc() {
    // Disables gift menu
    patch::write_nop(reinterpret_cast<void*>(0x808f9890));
}

void tick() {
    mkb::unlock_info.party_games = 0x0001b600;
    // We have to do this hacky stuff to get some Continue screen sprites to work properly
    if (mkb::sub_mode == mkb::SMD_GAME_CONTINUE_INIT) {
        mkb::g_playpoint_msg_counter = 0x1;
    }
    else if (mkb::sub_mode == mkb::SMD_GAME_RESTART_INIT || mkb::sub_mode == mkb::SMD_GAME_NAMEENTRY_READY_INIT || mkb::sub_mode == mkb::SMD_GAME_ROLL_INIT) {
        mkb::g_playpoint_msg_counter = 0x0;
    }
    else {
        return;
    }
}
}// namespace remove_playpoints