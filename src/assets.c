#include <stdio.h>
#include <zvb_gfx.h>
#include "assets.h"

gfx_error load_palette(gfx_context* ctx)
{
    // Load the palette
    const size_t palette_size = &_palette_end - &_palette_start;
    return gfx_palette_load(ctx, &_palette_start, palette_size, 0);
}

gfx_error load_tileset(gfx_context* ctx, gfx_tileset_options* options)
{
    const size_t size = &_tileset_end - &_tileset_start;
    return gfx_tileset_load(ctx, &_tileset_start, size, options);
}

zos_err_t load_zmt(track_t* track, uint8_t index)
{
    zmt_reset(VOL_50);
    switch (index) {
        case 0: {
            const size_t size = &_zmt_track1_end - &_zmt_track1_start;
            return zmt_rom_load(track, &_zmt_track1_start, size);
        } break;
            // case 1: {
            //     const size_t size = &_zmt_track2_end - &_zmt_track2_start;
            //     return zmt_rom_load(track, &_zmt_track2_start, size);
            // } break;
    }
    return ERR_NOT_A_FILE;
}

void __assets__(void) __naked
{
    __asm__(
        // shared palette
        "__palette_start:\n"
        "    .incbin \"assets/tileset.ztp\"\n"
        "__palette_end:\n"

        // tiles
        "__tileset_start:\n"
        "    .incbin \"assets/tileset.zts\"\n"
        "__tileset_end:\n"

        // background music
        // Track 1 - Space Battle
        "__zmt_track1_start:\n"
        "    .incbin \"assets/shooter.zmt\"\n"
        "__zmt_track1_end:\n");
}
