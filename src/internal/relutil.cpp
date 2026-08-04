#include "relutil.h"

#include "mkb/mkb.h"
#include "utils/mathutils.h"

namespace relutil {

enum class ModuleId : u32 {
    Dol = 0,
    MainLoop = 1,
    MainGame = 2,
    SelNgc = 3,
};

struct Region {
    ModuleId id;
    void* vanilla_ptr;
    u32 size;
    bool is_bss;
};

struct RelEntry {
    u16 offset;
    u8 type;
    u8 section;
    u32 addend;
};
static_assert(sizeof(RelEntry) == 0x8);

struct Imp {
    u32 module_id;
    RelEntry* rel_offset;
};
static_assert(sizeof(Imp) == 0x8);

struct RelHeader {
    u32 id;
    RelHeader* next;
    RelHeader* prev;
    u32 num_sections;
    void* section_info_offset;
    char* name_offset;
    u32 name_size;
    u32 version;
    u32 bss_size;
    RelEntry* rel_offset;
    Imp* imp_offset;
    u32 imp_size;
    u8 prolog_section;
    u8 epilog_section;
    u8 unresolved_section;
    u8 bss_section;
    void* prolog;
    void* epilog;
    void* unresolved;
    u32 align;
    u32 bssAlign;
    u32 fixSize;
};
static_assert(sizeof(RelHeader) == 0x4C);

static Region s_vanilla_regions[] = {
    {ModuleId::Dol, reinterpret_cast<void*>(0x80000000), 0x199F84, false},
    {ModuleId::MainLoop, reinterpret_cast<void*>(0x80270100), 0x2DC7CC, false},
    {ModuleId::MainLoop, reinterpret_cast<void*>(0x8054C8E0), 0xDDA4C, true},
    {ModuleId::MainGame, reinterpret_cast<void*>(0x808F3FE0), 0x8B484, false},
    {ModuleId::MainGame, reinterpret_cast<void*>(0x8097F4A0), 0x65F0, true},
    {ModuleId::SelNgc, reinterpret_cast<void*>(0x808F3FE0), 0x55C87, false},
    {ModuleId::SelNgc, reinterpret_cast<void*>(0x80949CA0), 0x8BD4, true},
};

void* compute_mainloop_reldata_boundary(void* start) {
    RelHeader* module = *reinterpret_cast<RelHeader**>(0x800030C8);
    for (u32 imp_idx = 0; imp_idx * sizeof(Imp) < module->imp_size; imp_idx++) {
        Imp& imp = module->imp_offset[imp_idx];
        // Look for end of relocation data against main_loop.rel itself
        if (imp.module_id != 1) continue;

        // Ignore space already allocated for this mod
        // `rel_offset` may not be `sizeof(RelEntry)` aligned, so give `start` the same alignment
        u32 start_aligned = reinterpret_cast<u32>(start) + (reinterpret_cast<u32>(imp.rel_offset) % sizeof(RelEntry));
        u32 first_valid_ptr = MAX(reinterpret_cast<u32>(imp.rel_offset), start_aligned);
        RelEntry* first_valid = reinterpret_cast<RelEntry*>(first_valid_ptr);

        u32 rel_idx = 0;
        for (; first_valid[rel_idx].type != 203; rel_idx++)
            ;
        return &first_valid[rel_idx + 1];
    }
    return nullptr;
}

static RelHeader* find_loaded_rel(ModuleId id) {
    RelHeader* module = *reinterpret_cast<RelHeader**>(0x800030C8);
    while (module != nullptr) {
        if (module->id == static_cast<u32>(id)) {
            return module;
        }
        module = module->next;
    }
    return nullptr;
}

void* relocate_addr(u32 vanilla_addr) {
    for (const auto& region: s_vanilla_regions) {
        u32 region_addr = reinterpret_cast<u32>(region.vanilla_ptr);
        if (vanilla_addr < region_addr || vanilla_addr >= region_addr + region.size) continue;

        if (region.id == ModuleId::Dol) return reinterpret_cast<void*>(vanilla_addr);

        RelHeader* module = find_loaded_rel(region.id);
        if (module == nullptr) return nullptr;

        u32 live_addr;
        if (region.is_bss) {
            if (region.id == ModuleId::MainLoop) {
                live_addr = reinterpret_cast<u32>(mkb::mainloop_rel_buffer_info.bss_buffer);
            } else {
                live_addr = reinterpret_cast<u32>(mkb::additional_rel_buffer_info.bss_buffer);
            }
        } else {
            live_addr = reinterpret_cast<u32>(module);
        }
        return reinterpret_cast<void*>(live_addr + vanilla_addr - region_addr);
    }
    return nullptr;
}

}// namespace relutil
