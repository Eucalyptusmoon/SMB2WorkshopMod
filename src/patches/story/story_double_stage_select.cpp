#include "story_double_stage_select.h"
#include "../internal/pad.h"
#include "internal/patch.h"
#include "internal/tickable.h"
#include "mkb/mkb.h"

namespace story_double_stage_select {

TICKABLE_DEFINITION((
        .name = "story-double-stage-select",
        .description = "Story mode double stage select",
        .init_main_loop = init_main_loop,
        .tick = tick))

static patch::Tramp<decltype(&mkb::check_pause_menu_input)> s_check_pause_menu_input_tramp;

// While we can easily edit a few values in the function which handles pausemenu selections to
// get most of the desired behavior, some of it has to be recreated manually

void pausemenu_handler(mkb::Sprite* pause_sprite) {
    if (mkb::main_game_mode == mkb::STORY_MODE) {
        // Stage select
        if (((mkb::g_current_focused_pause_menu_entry == 5) || (mkb::g_current_focused_pause_menu_entry == 6)) && pad::button_pressed(mkb::PAD_BUTTON_A) && mkb::g_some_status_bitflag_maybe_pause_related != 00000000) {
            mkb::g_related_to_pause_menu_input = 0x3c;
            if (pause_sprite != (mkb::Sprite*) 0x0) {
                pause_sprite->para1 = 2;
            }
            // Fade the music out only if the music fix isn't enabled
            if (!tickable::get_tickable_manager().get_tickable_status("story-mode-music-fix")) {
                mkb::g_fade_track_volume(0x3b, '\x02');
            }
            mkb::g_screenfade_color = 0xffffff;
        }
        // Exit game
        else if (mkb::g_current_focused_pause_menu_entry == 4 && pad::button_pressed(mkb::PAD_BUTTON_A) && mkb::g_some_status_bitflag_maybe_pause_related != 00000000) {
            mkb::g_related_to_pause_menu_input = 0x3c;
            if (pause_sprite != (mkb::Sprite*) 0x0) {
                pause_sprite->para1 = 3;
            }
            mkb::g_fade_track_volume(0x3c, '\x02');
            mkb::g_load_stage_for_menu_bg(-1, 1);
        }
    }
}

void init_main_loop() {
    const int total_size = 42;// Adjust size based on gaps and strings
    const int gap_size = 5;   // Gap of nullptrs between each string (game expects these to be localized strings)

    // Array of pointers to strings
    static char* menu_options_1[total_size] = {nullptr};
    static char* menu_options_2[total_size] = {nullptr};

    // Strings to be stored in the array
    char* options_1[7];
    char* options_2[7];

    if (!tickable::get_tickable_manager().get_tickable_status("smb1-camera-toggle")) {
        options_1[0] = "Continue game";
        options_1[1] = "Retry";
        options_1[2] = "View stage";
        options_1[3] = "How to play";
        options_1[4] = "Exit game";
        options_1[5] = "Stage select";
        options_1[6] = "Stage select";

        options_2[0] = "Continue game";
        options_2[1] = "Retry";
        options_2[2] = "Save replay";
        options_2[3] = "How to play";
        options_2[4] = "Exit game";
        options_2[5] = "Stage select";
        options_2[6] = "Stage select";
    }
    else {
        options_1[0] = "Continue game";
        options_1[1] = "Retry";
        options_1[2] = "View stage";
        options_1[3] = "Camera";
        options_1[4] = "Exit game";
        options_1[5] = "Stage select";
        options_1[6] = "Stage select";

        options_2[0] = "Continue game";
        options_2[1] = "Retry";
        options_2[2] = "Save replay";
        options_2[3] = "Camera";
        options_2[4] = "Exit game";
        options_2[5] = "Stage select";
        options_2[6] = "Stage select";
    }


    // Fill the array with strings and gaps of nullptrs
    int current_index = 0;
    for (int i = 0; i < 7; ++i) {
        menu_options_1[current_index] = options_1[i];
        menu_options_2[current_index] = options_2[i];
        current_index += gap_size + 1;// Move index by 1 (string) + gap size
    }

    mkb::pausemenu_entry_pointers[6] = menu_options_1;
    mkb::pausemenu_entry_pointers[14] = menu_options_2;
    // Expand entry count
    mkb::pausemenu_entry_counts[6] = 7;
    // Edits behaviors in the function which handles pausemenu selections
    patch::write_nop(reinterpret_cast<void*>(0x802745a4));
    patch::write_word(reinterpret_cast<void*>(0x8027459c), 0x2c000004);// cmpwi r0, 4
    // Hook into the function which checks input in the pausemenu for our recreated behaviors
    patch::hook_function(s_check_pause_menu_input_tramp, mkb::check_pause_menu_input, [](mkb::Sprite* pause_sprite) {
        s_check_pause_menu_input_tramp.dest(pause_sprite);
        pausemenu_handler(pause_sprite);
    });
}

void tick() {
    if (mkb::main_game_mode == mkb::STORY_MODE && mkb::sub_mode != mkb::SMD_GAME_SCENARIO_MAIN) {
        // Rids and rearranges some vanilla behaviors for pausemenu selections when in Story Mode
        patch::write_word(reinterpret_cast<void*>(0x8027513c), 0x3803fffd);
        patch::write_nop(reinterpret_cast<void*>(0x80273ad0));
        // In the function which handles displaying the pausemenu sprite, edit some values so seven
        // options at once display properly
        patch::write_word(reinterpret_cast<void*>(0x8032a5a0), 0x2c000007);
        patch::write_word(reinterpret_cast<void*>(0x8032a760), 0x2c000007);
        patch::write_word(reinterpret_cast<void*>(0x8032a768), 0x1c7c0019);
    }
    else {
        // Original instructions
        patch::write_word(reinterpret_cast<void*>(0x8027513c), 0x3803ffff);
        patch::write_word(reinterpret_cast<void*>(0x80273ad0), 0x3803ffff);
        patch::write_word(reinterpret_cast<void*>(0x8032a5a0), 0x2c000006);
        patch::write_word(reinterpret_cast<void*>(0x8032a760), 0x2c000006);
        patch::write_word(reinterpret_cast<void*>(0x8032a768), 0x1c7c001e);
    }
}
}// namespace story_double_stage_select