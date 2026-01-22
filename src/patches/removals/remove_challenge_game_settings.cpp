#include "remove_challenge_game_settings.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "mkb/mkb.h"
#include "utils/ppcutil.h"

namespace remove_challenge_game_settings {

TICKABLE_DEFINITION((
        .name = "remove-challenge-game-settings-screen",
        .description = "Challenge Mode Game Settings removal",
        .init_sel_ngc = init_sel_ngc, ))

// In the function which handles what screen to take us to after selecting a difficulty,
// replace the Game Settings menu call with one to start Challenge Mode
void init_sel_ngc() {
    patch::write_word(reinterpret_cast<void*>(0x808FFB40), PPC_INSTR_LI(PPC_R6, mkb::MENUSCREEN_CHALLENGE_DIFFICULTY));
}

}// namespace remove_challenge_game_settings
