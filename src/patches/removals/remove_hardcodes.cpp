#include "remove_hardcodes.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "utils/ppcutil.h"

namespace remove_hardcodes {

TICKABLE_DEFINITION((
        .name = "remove-stage-slot-hardcodes",
        .description = "Remove stage slot hardcodes",
        .init_main_loop = init_main_loop,
        .init_main_game = init_main_game, ))

void init_main_loop() {
    // Nop a call to handle hardcoded stage object drawing for
    // SMB2 stages
    patch::write_nop(reinterpret_cast<void*>(0x802c96d8));
    // Revolution hardcode removal
    // Always return 'false' for a specific function that checks if the stage ID
    // is 348 when determining whether or not to handle level loading specially
    patch::write_word(reinterpret_cast<void*>(0x802ca9fc), PPC_INSTR_LI(PPC_R3, 0x0));
    // Nop a call to handle hardcoded stage lights leftover from
    // SMB1
    patch::write_nop(reinterpret_cast<void*>(0x802945d8));
    // Check for stage 0xffff instead of 190 for what stage's .gma/.tpl not to load
    patch::write_word(reinterpret_cast<void*>(0x802c7350), PPC_INSTR_CMPWI(PPC_R31, -1));
    // Check for stage 0xffffffff when enabling a lighting hardcode instead of 200
    patch::write_word(reinterpret_cast<void*>(0x80455ff8), 0xffffffff);
    // In the function which handles Bonus Wave's collision, nop
    // the part which decides what space has hardcoded collision
    patch::write_nop(reinterpret_cast<void*>(0x802c79b4));
    // In an array for hardcoded camera settings, check for stage ID
    // 0xffffffff instead of any used stage slots
    patch::write_word(reinterpret_cast<void*>(0x8044b208), 0xffffffff);
    patch::write_word(reinterpret_cast<void*>(0x8044b1e0), 0xffffffff);
    patch::write_word(reinterpret_cast<void*>(0x8044b1f4), 0xffffffff);
    // Labyrinth camera removal
    // Always compare the stage ID to 0xFFFF when these camera functions check
    // if the current stage ID is 0x15a when determining specific constants.
    patch::write_word(reinterpret_cast<void*>(0x802858D4), PPC_INSTR_CMPWI(PPC_R0, -1));
    patch::write_word(reinterpret_cast<void*>(0x802874BC), PPC_INSTR_CMPWI(PPC_R0, -1));
    patch::write_word(reinterpret_cast<void*>(0x8028751C), PPC_INSTR_CMPWI(PPC_R0, -1));
    patch::write_word(reinterpret_cast<void*>(0x802880EC), PPC_INSTR_CMPWI(PPC_R0, -1));
    patch::write_word(reinterpret_cast<void*>(0x802881D4), PPC_INSTR_CMPWI(PPC_R0, -1));
    patch::write_word(reinterpret_cast<void*>(0x802883B4), PPC_INSTR_CMPWI(PPC_R0, -1));
    patch::write_word(reinterpret_cast<void*>(0x802886B8), PPC_INSTR_CMPWI(PPC_R0, -1));
    patch::write_word(reinterpret_cast<void*>(0x8028BF44), PPC_INSTR_CMPWI(PPC_R0, -1));
    patch::write_word(reinterpret_cast<void*>(0x8028C1CC), PPC_INSTR_CMPWI(PPC_R0, -1));
    patch::write_word(reinterpret_cast<void*>(0x8028C650), PPC_INSTR_CMPWI(PPC_R0, -1));
    patch::write_word(reinterpret_cast<void*>(0x8028CA84), PPC_INSTR_CMPWI(PPC_R0, -1));
    patch::write_word(reinterpret_cast<void*>(0x80291338), PPC_INSTR_CMPWI(PPC_R0, -1));
    patch::write_word(reinterpret_cast<void*>(0x80291420), PPC_INSTR_CMPWI(PPC_R0, -1));
    patch::write_word(reinterpret_cast<void*>(0x80291664), PPC_INSTR_CMPWI(PPC_R0, -1));
    patch::write_word(reinterpret_cast<void*>(0x80291904), PPC_INSTR_CMPWI(PPC_R0, -1));
    patch::write_word(reinterpret_cast<void*>(0x80291930), PPC_INSTR_CMPWI(PPC_R0, -1));
    patch::write_word(reinterpret_cast<void*>(0x80291960), PPC_INSTR_CMPWI(PPC_R0, -1));
    patch::write_word(reinterpret_cast<void*>(0x8029198C), PPC_INSTR_CMPWI(PPC_R0, -1));
    patch::write_word(reinterpret_cast<void*>(0x80291AEC), PPC_INSTR_CMPWI(PPC_R0, -1));

    // Wormhole surface fix
    // Always return 'true' for a specific function that checks if the stage ID
    // belongs to a slot normally used for party games.
    patch::write_word(reinterpret_cast<void*>(0x802c8ce4), PPC_INSTR_LI(PPC_R0, 0x1));
}

void init_main_game() {
    // Nop a call to handle hardcoded stage object drawing for
    // SMB2 stages
    patch::write_nop(reinterpret_cast<void*>(0x809140f4));
    // In the array of Story Mode preview settings, normalize Entangled Path's
    // scale
    mkb::MAIN_GAME_STORY_STAGE_PREVIEW_PARAMS[73].scale = 1;
}

}// namespace remove_hardcodes
