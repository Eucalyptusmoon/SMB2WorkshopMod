#include "goal_type_sprites.h"

#include "internal/heap.h"
#include "internal/log.h"
#include "internal/patch.h"
#include "internal/tickable.h"
#include "mkb/mkb.h"

namespace goal_type_sprites {

TICKABLE_DEFINITION((
        .name = "goal-type-sprites",
        .description = "Goal type sprites",
        .init_main_loop = init_main_loop, ))


void create_goal_type_sprites() {

    mkb::StagedefColiHeader* itemgroup;
    // Literal strings with Shift-JIS bytes
    char* open_star = "\x81\x9b";
    char* full_star = "\x81\x9c";
    bool has_blue_goal = false;
    bool has_green_goal = false;
    bool has_red_goal = false;

    itemgroup = mkb::stagedef->coli_header_list;

    for (int itemgroup_idx = 0; itemgroup_idx < (int) mkb::stagedef->coli_header_count; itemgroup_idx++, itemgroup++) {
        if (itemgroup->goal_count > 0) {
            mkb::StagedefGoal* goal = itemgroup->goal_list;
            for (int goal_idx = 0; goal_idx < (int) itemgroup->goal_count; goal_idx++, goal++) {
                switch (goal->type) {
                    case mkb::Blue:
                        has_blue_goal = true;
                        break;
                    case mkb::Green:
                        has_green_goal = true;
                        break;
                    case mkb::Red:
                        has_red_goal = true;
                        break;
                }
            }
        }
    }

    mkb::Sprite* blue_sprite = mkb::create_sprite();
    if (blue_sprite != nullptr) {
        blue_sprite->pos.x = 16.0;
        blue_sprite->pos.y = 49.0;
        blue_sprite->font = mkb::FONT_ASC_24x24;
        blue_sprite->fpara1 = 1.0;
        blue_sprite->fpara2 = 0.0;
        blue_sprite->alpha = 1.0;
        blue_sprite->mult_color.red = 0x00;
        blue_sprite->mult_color.green = 0x00;
        if (has_blue_goal) {
            blue_sprite->mult_color.blue = 0xff;
        }
        else {
            blue_sprite->mult_color.blue = 0x40;
        }
        blue_sprite->width = 0.8;
        blue_sprite->height = 0.8;
        blue_sprite->field21_0x20 = 1.0;
        blue_sprite->field22_0x24 = 1.0;
        blue_sprite->g_flags1 |= 0xa1000000;
        blue_sprite->widescreen_translation_x = 0x60;
        if (has_blue_goal) {
            mkb::strcpy(blue_sprite->text, full_star);
        }
        else {
            mkb::strcpy(blue_sprite->text, open_star);
        }
    }

    mkb::Sprite* green_sprite = mkb::create_sprite();
    if (green_sprite != nullptr) {
        green_sprite->pos.x = 36.0;
        green_sprite->pos.y = 49.0;
        green_sprite->font = mkb::FONT_ASC_24x24;
        green_sprite->fpara1 = 1.0;
        green_sprite->fpara2 = 0.0;
        green_sprite->alpha = 1.0;
        green_sprite->mult_color.red = 0x00;
        if (has_green_goal) {
            green_sprite->mult_color.green = 0xff;
        }
        else {
            green_sprite->mult_color.green = 0x40;
        }
        green_sprite->mult_color.blue = 0x00;
        green_sprite->width = 0.8;
        green_sprite->height = 0.8;
        green_sprite->field21_0x20 = 1.0;
        green_sprite->field22_0x24 = 1.0;
        green_sprite->g_flags1 |= 0xa1000000;
        green_sprite->widescreen_translation_x = 0x60;
        if (has_green_goal) {
            mkb::strcpy(green_sprite->text, full_star);
        }
        else {
            mkb::strcpy(green_sprite->text, open_star);
        }
    }

    mkb::Sprite* red_sprite = mkb::create_sprite();
    if (red_sprite != nullptr) {
        red_sprite->pos.x = 56.0;
        red_sprite->pos.y = 49.0;
        red_sprite->font = mkb::FONT_ASC_24x24;
        red_sprite->fpara1 = 1.0;
        red_sprite->fpara2 = 0.0;
        red_sprite->alpha = 1.0;
        if (has_red_goal) {
            red_sprite->mult_color.red = 0xff;
        }
        else {
            red_sprite->mult_color.red = 0x40;
        }
        red_sprite->mult_color.green = 0x00;
        red_sprite->mult_color.blue = 0x00;
        red_sprite->width = 0.8;
        red_sprite->height = 0.8;
        red_sprite->field21_0x20 = 1.0;
        red_sprite->field22_0x24 = 1.0;
        red_sprite->g_flags1 |= 0xa1000000;
        red_sprite->widescreen_translation_x = 0x60;
        if (has_red_goal) {
            mkb::strcpy(red_sprite->text, full_star);
        }
        else {
            mkb::strcpy(red_sprite->text, open_star);
        }
    }
}

void init_main_loop() {

    static patch::Tramp<decltype(&mkb::create_score_sprites)> s_score_sprites_tramp;
    patch::hook_function(
        s_score_sprites_tramp,
        mkb::create_score_sprites, [](float x, float y) {
            s_score_sprites_tramp.dest(x, y);
            create_goal_type_sprites();
        });
}

}// namespace goal_type_sprites
