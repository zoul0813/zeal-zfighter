#include <stdint.h>
#include <zvb_sprite.h>
#include <zgdk.h>
#include "bullet.h"
#include "main.h"

#ifndef PLAYER_H
#define PLAYER_H


#define PLAYER_SPEED        (2U)
#define PLAYER_MAX_BULLETS  3
#define PLAYER_BULLET_FREQ  8
#define PLAYER_MAX_LIVES    3
#define PLAYER_MAX_HEALTH   8
#define PLAYER_LIVES_TILE   15

#define PLAYER_HIT_INDEX    77
#define PLAYER_COLOR1   0xa9c7 /* ac3940 */
#define PLAYER_COLOR2   0xab32 /* ac6494 */

// typedef struct {
//         uint8_t active;
//         // uint8_t sprite_index;

// } shield_t;

typedef struct {
        uint8_t health;
        uint8_t lives;
        uint16_t score;
        Direction direction;
        uint8_t shield_active;
        uint8_t bullet;
        uint8_t bullet_lock;
        Rect rect;

        // sprites
        // uint8_t sprite_index;
        gfx_sprite* sprite_tl;
        gfx_sprite* sprite_tr;
        gfx_sprite* sprite_bl;
        gfx_sprite* sprite_br;

        gfx_sprite* sprite_shield_tl;
        gfx_sprite* sprite_shield_t;
        gfx_sprite* sprite_shield_tr;
        gfx_sprite* sprite_shield_r1;
        gfx_sprite* sprite_shield_r2;
        gfx_sprite* sprite_shield_br;
        gfx_sprite* sprite_shield_b;
        gfx_sprite* sprite_shield_bl;
} player_t;

extern player_t player;

error player_init(void);
void player_spawn(void);
error player_deinit(void);
void player_shoot(void);
void player_damaged(uint8_t damage);
void player_score(uint8_t score);
uint8_t player_destroyed(void);
void player_update(void);
void player_move(void);
void player_attract(void);
void player_draw(void);
void player_draw_lives(uint8_t x, uint8_t y);

#endif