#include "remove_menu_icon_brown_color.h"

#include "internal/patch.h"
#include "internal/relutil.h"
#include "internal/tickable.h"

namespace remove_menu_icon_brown_color {

TICKABLE_DEFINITION((
        .name = "remove-menu-icon-brown-color",
        .description = "Remove menu icon brown color",
        .init_sel_ngc = init_sel_ngc, ))

void init_sel_ngc() {
    patch::write_nop(relutil::relocate_addr(0x8090AC4C));
}

}// namespace remove_menu_icon_brown_color
