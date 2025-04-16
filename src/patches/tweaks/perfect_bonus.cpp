#include "perfect_bonus.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "mkb/mkb.h"


namespace perfect_bonus {

TICKABLE_DEFINITION((
        .name = "perfect-bonus-completion",
        .description = "Perfect bonus completion",
        .init_main_loop = init_main_loop,
        .init_main_game = init_main_game))

static patch::Tramp<decltype(&mkb::smd_game_bonus_clear_init)> s_smd_game_bonus_clear_init_tramp;

// We want to have the score you'd get for a blue goal to be added so
// there isn't a tradeoff between not getting a banana and going for
// the goal instead
void add_goal_score() {
    mkb::mode_info.g_some_timer_frame_remaining_count = mkb::mode_info.stage_time_frames_remaining;
    mkb::increment_score(2, 0);
}

// If the stage is a bonus stage (ball mode 0x40) and no bananas remain,
// end the stage with a perfect (|= 0x228)
void init_main_loop() {
    static patch::Tramp<decltype(&mkb::event_info_tick)> event_info_tick_tramp;

    patch::hook_function(
        event_info_tick_tramp,
        mkb::event_info_tick, []() {
            event_info_tick_tramp.dest();
            if (mkb::mode_info.ball_mode == mkb::BALLMODE_ON_BONUS_STAGE &&
                mkb::mode_info.bananas_remaining == 0) {
                mkb::mode_info.ball_mode |= 0x228;
            }
        });
}

void init_main_game() {
    patch::hook_function(s_smd_game_bonus_clear_init_tramp, mkb::smd_game_bonus_clear_init, []() {
        add_goal_score();
        s_smd_game_bonus_clear_init_tramp.dest();
    });
}

}// namespace perfect_bonus