#include "custom_story_entries.h"

#include "internal/assembly.h"
#include "internal/patch.h"
#include "internal/tickable.h"
#include "utils/ppcutil.h"

namespace custom_story_entries {

TICKABLE_DEFINITION((
        .name = "custom-story-mode-entries",
        .description = "Custom Story Mode entries",
        .init_main_loop = init_main_loop, ))

int get_new_story_timers(int world, int world_stage) {
    return (int) main::new_story_entries[world][world_stage].time_limit * 60;
}

void init_main_loop() {
    mkb::SmStageInfo* const worlds[main::WORLD_COUNT] = {
        mkb::sm_stage_infos_world1,
        mkb::sm_stage_infos_world2,
        mkb::sm_stage_infos_world3,
        mkb::sm_stage_infos_world4,
        mkb::sm_stage_infos_world5,
        mkb::sm_stage_infos_world6,
        mkb::sm_stage_infos_world7,
        mkb::sm_stage_infos_world8,
        mkb::sm_stage_infos_world9,
        mkb::sm_stage_infos_world10,
    };

    for (int w = 0; w < main::WORLD_COUNT; ++w) {
        for (int s = 0; s < main::STAGES_PER_WORLD; ++s) {
            const main::NewStoryStageEntry& src = main::new_story_entries[w][s];

            worlds[w][s].stage_id = src.stage_id;
            worlds[w][s].difficulty = src.difficulty;
        }
    }

    patch::write_branch(
        reinterpret_cast<void*>(mkb::get_storymode_stage_time_limit),
        reinterpret_cast<void*>(get_new_story_timers));
}
}// namespace custom_story_entries
