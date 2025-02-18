#include <stdint.h>
#include <zvb_sprite.h>
#include <zgdk.h>
#include "main.h"
#include "patterns.h"

#ifndef BOSS_H
#define BOSS_H

typedef struct {
        uint8_t active;
        uint8_t y;
        uint8_t x;

        Formation formation;
        int8_t f_dir;

        // sprites
        gfx_sprite* sprite_tl;
        gfx_sprite* sprite_tr;
        gfx_sprite* sprite_bl;
        gfx_sprite* sprite_br;
} boss_t;

extern boss_t boss;

error boss_init(void);
error boss_deinit(void);
void boss_move(void);
void boss_update(void);
void boss_spawn(void);
uint8_t boss_active(void);

#endif