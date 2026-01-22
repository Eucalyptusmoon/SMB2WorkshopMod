#include "smb1_score_count.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "mkb/mkb.h"

namespace smb1_score_count {

TICKABLE_DEFINITION((
        .name = "smb1-score-count",
        .description = "SMB1 score count",
        .init_main_loop = init_main_loop, ))

int frame_counter = 0;
int lerp_duration;

void sprite_tick(u8* status, mkb::Sprite* sprite) {
    float score = (float) mkb::balls[mkb::curr_player_idx].score;
    float old_score = sprite->fpara1;
    float delta = score - old_score;
    float displayed;
    int final_score;

    if (mkb::sub_mode != mkb::SMD_GAME_GOAL_REPLAY_MAIN) {
        lerp_duration = 30;
    }
    else {
        lerp_duration = 120;
    }

    if (score > old_score) {
        if (frame_counter < lerp_duration) {
            displayed = old_score + frame_counter * (delta / lerp_duration);
            frame_counter++;
            if (mkb::sub_mode == mkb::SMD_GAME_GOAL_REPLAY_MAIN && (frame_counter % 4 == 0)) {
                mkb::call_SoundReqID_arg_2(46);
            }
        }
        else {
            sprite->fpara1 = score;
            frame_counter = 0;
        }
    }
    else {
        displayed = score;
    }

    final_score = mkb::__cvt_fp2unsigned((double) displayed);
    mkb::sprintf(sprite->text, "%d", final_score);
    return;
}

void init_main_loop() {
    patch::hook_function(mkb::sprite_score_tick, sprite_tick);
}

}// namespace smb1_score_count
