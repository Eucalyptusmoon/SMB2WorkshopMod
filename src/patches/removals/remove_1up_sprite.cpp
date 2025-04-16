#include "remove_1up_sprite.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "mkb/mkb.h"

namespace remove_1up_sprite {

TICKABLE_DEFINITION((
        .name = "remove-1up-sprite",
        .description = "1UP sprite removal",
        .init_main_loop = init_main_loop, ))

// Null the 1UP sprite's text string
// Some functions associated with the sprite play sound effects, so this is
// The best removal method
void init_main_loop() {
    mkb::strcpy(mkb::SPRITE_1UP_TEXT_STRING, "");
}

}// namespace remove_1up_sprite
