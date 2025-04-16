#include "remove_extras.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "utils/ppcutil.h"

namespace remove_extras {

TICKABLE_DEFINITION((
        .name = "remove-extras",
        .description = "Remove extras",
        .init_main_game = init_main_game, ))


void unlock_master() {
    if (mkb::main_game_mode == mkb::CHALLENGE_MODE) {
        if (mkb::curr_difficulty == mkb::DIFF_EXPERT) {
            if ((mkb::mode_info.ball_mode & mkb::BALLMODE_ON_FINAL_STAGE) && (mkb::mode_info.continues_used == 0)) {
                char is_master_unlocked = mkb::is_master_unlocked();
                if (is_master_unlocked == 0) {
                    mkb::unlock_master();
                }
            }
        }
    }
}
static patch::Tramp<decltype(&mkb::smd_game_goal_replay_tick)> s_smd_game_goal_replay_tick_tramp;

void init_main_game() {
    // Always return 'false' for if we're entering the extras
    // in the function which handles what to do once you enter a goal
    patch::write_word(reinterpret_cast<void*>(0x808f61a8), PPC_INSTR_LI(PPC_R4, 0x0));
    // The method used for unlocking Master in the vanilla game relies on playing Extras,
    // so use our own check
    patch::hook_function(s_smd_game_goal_replay_tick_tramp, mkb::smd_game_goal_replay_tick, []() {
        s_smd_game_goal_replay_tick_tramp.dest();
        unlock_master();
    });
}

}// namespace remove_extras
