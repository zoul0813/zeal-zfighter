#include <stdint.h>
#include <zvb_sprite.h>
#include <zgdk.h>
#include "bullet.h"
#include "main.h"

#ifndef PLAYER_H
#define PLAYER_H


#define PLAYER_SPEED       (2U)
#define PLAYER_MAX_BULLETS 3

typedef struct {
        uint8_t active;
        uint8_t sprite_index;
        gfx_sprite sprite_tl;
        gfx_sprite sprite_t;
        gfx_sprite sprite_tr;
        gfx_sprite sprite_r1;
        gfx_sprite sprite_r2;
        gfx_sprite sprite_br;
        gfx_sprite sprite_b;
        gfx_sprite sprite_bl;
} shield_t;

typedef struct {
        uint8_t health;
        uint16_t score;
        uint8_t sprite_index;
        gfx_sprite sprite_tl;
        gfx_sprite sprite_tr;
        gfx_sprite sprite_bl;
        gfx_sprite sprite_br;
        Direction direction;
        shield_t shield;
        uint8_t bullet;
} player_t;

extern player_t player;

error player_init(void);
error player_deinit(void);
void player_shoot(void);
void player_update(void);
void player_move(void);
void player_draw(void);

#endif