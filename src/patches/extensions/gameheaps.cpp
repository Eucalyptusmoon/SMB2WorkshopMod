#include "gameheaps.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "mkb/mkb.h"

namespace gameheaps {

TICKABLE_DEFINITION((
        .name = "merged-heaps",
        .description = "Merged game heaps",
        .init_main_loop = init_main_loop, ))

static void create_merged_game_heaps(int heap_config_idx) {
    if (mkb::heap_configs[heap_config_idx].flags & 1) {
        mkb::OSSetArenaLo(reinterpret_cast<void*>(mkb::g_some_other_heap_lo));
        mkb::OSSetArenaHi(reinterpret_cast<void*>(mkb::g_some_other_heap_hi));
    } else {
        mkb::OSSetArenaLo(reinterpret_cast<void*>(mkb::g_some_dead_heap_mem_lo));
        mkb::OSSetArenaHi(reinterpret_cast<void*>(mkb::g_some_dead_heap_mem_hi));
    }

    mkb::main_heap = mkb::OSCreateHeap(mkb::OSGetArenaLo(), mkb::OSGetArenaHi());
    mkb::main_heap_size = mkb::OSCheckHeap(mkb::main_heap);
    mkb::OSSetArenaLo(mkb::OSGetArenaHi());
    mkb::OSSetCurrentHeap(mkb::main_heap);

    mkb::stage_heap = mkb::main_heap;
    mkb::stage_heap_size = mkb::main_heap_size;
    mkb::bg_heap = mkb::main_heap;
    mkb::bg_heap_size = mkb::main_heap_size;
    mkb::chara_heap = mkb::main_heap;
    mkb::chara_heap_size = mkb::main_heap_size;
    mkb::replay_heap = mkb::main_heap;
    mkb::replay_heap_size = mkb::main_heap_size;
}

static void destroy_merged_game_heaps() {
    if (mkb::main_heap != -1) {
        mkb::OSDestroyHeap(mkb::main_heap);
        mkb::main_heap = -1;
        mkb::stage_heap = -1;
        mkb::bg_heap = -1;
        mkb::chara_heap = -1;
        mkb::replay_heap = -1;
    }
}

void init_main_loop() {
    patch::hook_function(mkb::create_game_heaps, create_merged_game_heaps);
    patch::hook_function(mkb::destroy_game_heaps, destroy_merged_game_heaps);
}

}// namespace gameheaps
