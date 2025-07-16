#include "smb1_camera_toggle.h"

#include "internal/pad.h"
#include "internal/patch.h"
#include "internal/tickable.h"
#include "mkb/mkb.h"
#include "utils/ppcutil.h"

namespace smb1_camera_toggle {

TICKABLE_DEFINITION((
        .name = "smb1-camera-toggle",
        .description = "SMB1 camera toggle",
        .init_main_loop = init_main_loop,
        .tick = tick, ))

static bool smb1_cam_toggled;

void remove_how_to() {
    // Intended behavior for when A is pressed on the ON/OFF menu option
    mkb::g_some_pausemenu_var = 0;
    mkb::destroy_sprite_with_unique_id(mkb::SPRITE_PAUSE_MENU);
    mkb::g_fade_track_volume(100, '\n');
    mkb::g_some_status_bitflag_maybe_pause_related |= 0x20;
}

void fix_on_off() {
    // The code to actually swap between ON/OFF is in the billiards .rel, so we manually recreate it elsewhere
    if (mkb::main_mode != mkb::MD_MINI) {
        mkb::g_some_status_bitflag_maybe_pause_related ^= 8;
    }
    // Original function call we branched (plays the sound effect when switching)
    mkb::call_SoundReqID_arg_1(0x169);
}

void init_main_loop() {
    // Remove default behavior of the How to play menu option in...
    // Challenge
    patch::write_branch_bl(reinterpret_cast<void*>(0x80273e5c), reinterpret_cast<void*>(remove_how_to));
    patch::write_nop(reinterpret_cast<void*>(0x80273df0));
    patch::write_nop(reinterpret_cast<void*>(0x80273e00));
    patch::write_nop(reinterpret_cast<void*>(0x80273e58));
    patch::write_nop(reinterpret_cast<void*>(0x80273e6c));
    // Practice
    patch::write_branch_bl(reinterpret_cast<void*>(0x80274148), reinterpret_cast<void*>(remove_how_to));
    patch::write_nop(reinterpret_cast<void*>(0x802740dc));
    patch::write_nop(reinterpret_cast<void*>(0x802740ec));
    patch::write_nop(reinterpret_cast<void*>(0x80274144));
    patch::write_nop(reinterpret_cast<void*>(0x80274158));
    // Story
    patch::write_branch_bl(reinterpret_cast<void*>(0x802747e0), reinterpret_cast<void*>(remove_how_to));
    patch::write_nop(reinterpret_cast<void*>(0x80274774));
    patch::write_nop(reinterpret_cast<void*>(0x80274784));
    patch::write_nop(reinterpret_cast<void*>(0x802747dc));
    patch::write_nop(reinterpret_cast<void*>(0x802747f0));
    // Rename the menu option
    mkb::challenge_play_pausemenu_entries[12] = "Camera";
    mkb::challenge_goal_pausemenu_entries[12] = "Camera";
    mkb::story_play_pausemenu_entries[18] = "Camera";
    mkb::story_goal_pausemenu_entries[18] = "Camera";
    mkb::practice_play_pausemenu_entries[18] = "Camera";
    mkb::practice_goal_pausemenu_entries[18] = "Camera";
    // Branch to our own code in the function which handles the ON/OFF pausemenu option
    patch::write_branch_bl(reinterpret_cast<void*>(0x80273878), reinterpret_cast<void*>(fix_on_off));
}

void tick() {
    // The write_word statement moves the camera's angle down by 2.8 degrees to match SMB1's angle
    // Everything else brings the camera position/pivot values in-line with SMB1's values
    // Camera mode 1 enables SMB1-like vertical camera tracking, camera mode 0x4c is SMB2's default
    if (mkb::main_mode == mkb::MD_GAME &&
        (mkb::sub_mode == mkb::SMD_GAME_PLAY_MAIN || mkb::sub_mode == mkb::SMD_GAME_READY_MAIN)) {
        if (smb1_cam_toggled) {
            if (mkb::cameras[0].mode == 0x4c) mkb::cameras[0].mode = 1;
            patch::write_word(reinterpret_cast<void*>(0x802886c8), PPC_INSTR_LI(PPC_R0, 0x400));
            mkb::g_camera_turn_rate_scale = 0.6875;
            mkb::camera_pivot_height = -0.5;
            mkb::camera_height = 1;
        }
        else {
            if (mkb::cameras[0].mode == 0x1) mkb::cameras[0].mode = 0x4c;
            patch::write_word(reinterpret_cast<void*>(0x802886c8), PPC_INSTR_LI(PPC_R0, 0x200));
            mkb::g_camera_turn_rate_scale = 0.75;
            mkb::camera_pivot_height = 0.18;
            mkb::camera_height = 0.8;
        }
    }
    // This is how ON/OFF is checked in vanilla
    if (mkb::g_some_status_bitflag_maybe_pause_related & 8) {
        smb1_cam_toggled = true;
    }
    else {
        smb1_cam_toggled = false;
    }
    // In functions which handle when to display the ON/OFF menu option and when it behaves,
    // alter them to check for our current pausemenu and only overwrite the text when not in
    // Billiards
    if (mkb::pausemenu_type == mkb::PMT_CHALLENGE) {
        if ((mkb::g_some_status_bitflag_maybe_pause_related & 4) != 0) {
            patch::write_word(reinterpret_cast<void*>(0x8032a914), 0x2c1b0009);
        }
        else {
            patch::write_word(reinterpret_cast<void*>(0x8032a914), 0x2c1b0001);
        }
        if (mkb::g_current_pause_menu_entry_count == 5) {
            // Special handler for CM retry
            patch::write_word(reinterpret_cast<void*>(0x8032a91c), 0x2c1c0003);
            patch::write_word(reinterpret_cast<void*>(0x80273678), 0x2c000003);
        }
        else {
            patch::write_word(reinterpret_cast<void*>(0x8032a91c), 0x2c1c0002);
            patch::write_word(reinterpret_cast<void*>(0x80273678), 0x2c000002);
        }
        patch::write_word(reinterpret_cast<void*>(0x80273664), 0x2c000001);
        mkb::strcpy(mkb::PAUSEMENU_ON_STRING, "  1");
        mkb::strcpy(mkb::PAUSEMENU_OFF_STRING, "2");
    }
    else if (mkb::pausemenu_type == mkb::PMT_STORY_PLAY) {
        if ((mkb::g_some_status_bitflag_maybe_pause_related & 4) != 0) {
            patch::write_word(reinterpret_cast<void*>(0x8032a914), 0x2c1b000e);
        }
        else {
            patch::write_word(reinterpret_cast<void*>(0x8032a914), 0x2c1b0006);
        }
        patch::write_word(reinterpret_cast<void*>(0x8032a91c), 0x2c1c0003);
        patch::write_word(reinterpret_cast<void*>(0x80273664), 0x2c000006);
        patch::write_word(reinterpret_cast<void*>(0x80273678), 0x2c000003);
        mkb::strcpy(mkb::PAUSEMENU_ON_STRING, "  1");
        mkb::strcpy(mkb::PAUSEMENU_OFF_STRING, "2");
    }
    else if (mkb::pausemenu_type == mkb::PMT_PRACTICE) {
        if ((mkb::g_some_status_bitflag_maybe_pause_related & 4) != 0) {
            patch::write_word(reinterpret_cast<void*>(0x8032a914), 0x2c1b000a);
        }
        else {
            patch::write_word(reinterpret_cast<void*>(0x8032a914), 0x2c1b0002);
        }
        patch::write_word(reinterpret_cast<void*>(0x8032a91c), 0x2c1c0003);
        patch::write_word(reinterpret_cast<void*>(0x80273664), 0x2c000002);
        patch::write_word(reinterpret_cast<void*>(0x80273678), 0x2c000003);
        mkb::strcpy(mkb::PAUSEMENU_ON_STRING, "  1");
        mkb::strcpy(mkb::PAUSEMENU_OFF_STRING, "2");
    }
    else {
        // Original instructions (Billiards)
        patch::write_word(reinterpret_cast<void*>(0x8032a914), 0x2c1b0004);
        patch::write_word(reinterpret_cast<void*>(0x8032a91c), 0x2c1c0001);
        patch::write_word(reinterpret_cast<void*>(0x80273664), 0x2c000004);
        patch::write_word(reinterpret_cast<void*>(0x80273678), 0x2c000001);
        mkb::strcpy(mkb::PAUSEMENU_ON_STRING, "ON");
        mkb::strcpy(mkb::PAUSEMENU_OFF_STRING, "OFF");
    }
}
}// namespace smb1_camera_toggle
