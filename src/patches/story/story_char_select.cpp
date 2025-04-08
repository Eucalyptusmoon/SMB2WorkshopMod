#include "story_char_select.h"

#include "custom/menu_option_toggle.h"
#include "internal/assembly.h"
#include "internal/pad.h"
#include "internal/patch.h"
#include "internal/tickable.h"
#include "mkb/mkb.h"

namespace story_char_select {

TICKABLE_DEFINITION((
        .name = "story-mode-char-select",
        .description = "Story mode character select",
        .init_main_loop = init_main_loop,
        .init_main_game = init_main_game,
        .init_sel_ngc = init_sel_ngc,
        .tick = tick, ))

static mkb::undefined4* AIAI[] = {&mkb::CHAR_A, &mkb::CHAR_I, &mkb::CHAR_A, &mkb::CHAR_I, &mkb::CHAR_SPACE,
                                  &mkb::CHAR_SPACE};
static mkb::undefined4* MEEMEE[] = {&mkb::CHAR_M, &mkb::CHAR_E, &mkb::CHAR_E, &mkb::CHAR_M, &mkb::CHAR_E,
                                    &mkb::CHAR_E};
static mkb::undefined4* BABY[] = {&mkb::CHAR_B, &mkb::CHAR_A, &mkb::CHAR_B, &mkb::CHAR_Y, &mkb::CHAR_SPACE,
                                  &mkb::CHAR_SPACE};
static mkb::undefined4* GONGON[] = {&mkb::CHAR_G, &mkb::CHAR_O, &mkb::CHAR_N, &mkb::CHAR_G, &mkb::CHAR_O,
                                    &mkb::CHAR_N};
static mkb::undefined4* HIHI[] = {&mkb::CHAR_H, &mkb::CHAR_I, &mkb::CHAR_H, &mkb::CHAR_I, &mkb::CHAR_SPACE,
                                  &mkb::CHAR_SPACE};
static mkb::undefined4** monkey_name_lookup[] = {AIAI, MEEMEE, BABY, GONGON, HIHI};

// Overrides the return value of certain functions to force the chosen monkey to be
// preloaded in place of AiAi
void init_main_loop() {
    patch::write_branch_bl(reinterpret_cast<void*>(0x803daffc),
                           reinterpret_cast<void*>(main::get_monkey_id_hook));
}

// Sets the storymode filename to the name of the selected monkey, when no name is provided.
void set_nameentry_filename() {
    for (int i = 0; i < 6; i++) {
        mkb::story_file_name[i] = reinterpret_cast<char*>(monkey_name_lookup[mkb::active_monkey_id[0]][i]);
    }
    mkb::g_some_nameentry_length = 0x6;
}

// Overrides the return value of certain functions to force the chosen monkey to be
// preloaded in place of AiAi.
// Also calls the function to set the default filename to the name of the selected
// monkey, rather than deafulting to 'AIAI'.
void init_main_game() {
    patch::write_branch_bl(reinterpret_cast<void*>(0x808fcac4),
                           reinterpret_cast<void*>(main::get_monkey_id_hook));
    patch::write_branch_bl(reinterpret_cast<void*>(0x808ff120),
                           reinterpret_cast<void*>(main::get_monkey_id_hook));
    patch::write_branch_bl(reinterpret_cast<void*>(0x80908894),
                           reinterpret_cast<void*>(main::get_monkey_id_hook));

    patch::write_branch_bl(reinterpret_cast<void*>(0x80906368),
                           reinterpret_cast<void*>(set_nameentry_filename));
    patch::write_nop(reinterpret_cast<void*>(0x8090636c));
    patch::write_nop(reinterpret_cast<void*>(0x80906370));
    patch::write_nop(reinterpret_cast<void*>(0x80906374));
    patch::write_nop(reinterpret_cast<void*>(0x80906378));
}

// Assign the correct 'next screen' variables to redirect Story Mode to the
// character select screen. Also handle input to prevent Story Mode from not
// initializing if mode_cnt isn't set to 1.
void init_sel_ngc() {
    // Patches the 'next screen ID' for the 'STORY MODE' button in the Mode Select screen
    mkb::menu_main_game_select_entries[0].next_screen_id = mkb::MENUSCREEN_CHARACTER_SELECT_2;
    // Remove the 'Gameplay will begin' portion of the Story Mode menu description text
    if ((menu_option_toggle::main_game_bitflag & 0x1) == 0) {
        mkb::strcpy(mkb::MENU_STORY_DESCRIPTION_TEXT, "This mode is only for 1 player. The next world\nwill be available if you clear 10 stages.\n*You will receive Play Points.");
    }
}

void tick() {
    // Overwriting the next screen value for the default Practice Mode option causes
    // transitions to not work and a few other oddities, so this is a little more
    // messy than expected
    if (mkb::main_mode == mkb::MD_SEL) {
        // These all change values in menu_character_select_tick
        // If Story Mode is selected on the menu...
        if (mkb::g_focused_maingame_menu == 0) {
            // ...change the screen stack check for what next screen to throw us at to 0xff
            patch::write_word(reinterpret_cast<void*>(0x808fbec8), 0x2c0000ff);
            // Change the screen stack check for what next screen to throw us at to the Mode Select menu
            patch::write_word(reinterpret_cast<void*>(0x808fc8a0), 0x2c000007);
            // Change the next screen value to the one for entering Story Mode
            patch::write_word(reinterpret_cast<void*>(0x808fc8e0), 0x38a0000c);
        }
        else {
            // Original instructions
            // Change the screen stack check for what next screen to throw us at to the Mode Select menu
            patch::write_word(reinterpret_cast<void*>(0x808fbec8), 0x2c000007);
            // Change the screen stack check for what next screen to throw us at to the Bowling menu
            patch::write_word(reinterpret_cast<void*>(0x808fc8a0), 0x2c000024);
            // Change the next screen value to the one for entering Bowling
            patch::write_word(reinterpret_cast<void*>(0x808fc8e0), 0x38a00027);
        }
    }
}

}// namespace story_char_select