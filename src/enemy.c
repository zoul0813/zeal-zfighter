#include <stdio.h>
#include <zgdk.h>
#include "enemy.h"
#include "main.h"
#include "bullet.h"
#include "player.h"

#define SPAWN_DERLAY    128

enemy_t ENEMIES[MAX_ENEMIES];
static uint8_t total_active = 0;
static uint8_t total_bullets = 0;
static uint8_t spawn_timer = 0;
static uint16_t palettes[4][5] = {
    { 0x4244, 0x6326, 0x8CA8, 0x9D2A, 0xADAC, }, // yellow
    { 0x2109, 0x318C, 0x42D2, 0x5394, 0x6436, }, // dark blue
    { 0x2249, 0x32AC, 0x43F2, 0x5454, 0x65B6, }, // light blue
    { 0x2246, 0x3327, 0x44AA, 0x552B, 0x65AD, }, // green
};

// ./tools/wavegen.py -a 42
// static int8_t WAVE1[] = {
//     64, 63, 63, 63, 63, 63, 63, 63, 62, 62, 62, 61, 61, 60, 60, 59,
//     59, 58, 57, 57, 56, 55, 54, 54, 53, 52, 51, 50, 49, 48, 47, 46,
//     45, 44, 42, 41, 40, 39, 38, 36, 35, 34, 32, 31, 30, 28, 27, 25,
//     24, 23, 21, 20, 18, 17, 15, 14, 12, 10, 9, 7, 6, 4, 3, 1,
//     0, -1, -3, -4, -6, -7, -9, -10, -12, -14, -15, -17, -18, -20, -21, -23,
//     -24, -25, -27, -28, -30, -31, -32, -34, -35, -36, -38, -39, -40, -41, -42, -44,
//     -45, -46, -47, -48, -49, -50, -51, -52, -53, -54, -54, -55, -56, -57, -57, -58,
//     -59, -59, -60, -60, -61, -61, -62, -62, -62, -63, -63, -63, -63, -63, -63, -63,
//     -64, -63, -63, -63, -63, -63, -63, -63, -62, -62, -62, -61, -61, -60, -60, -59,
//     -59, -58, -57, -57, -56, -55, -54, -54, -53, -52, -51, -50, -49, -48, -47, -46,
//     -45, -44, -42, -41, -40, -39, -38, -36, -35, -34, -32, -31, -30, -28, -27, -25,
//     -24, -23, -21, -20, -18, -17, -15, -14, -12, -10, -9, -7, -6, -4, -3, -1,
//     0, 1, 3, 4, 6, 7, 9, 10, 12, 14, 15, 17, 18, 20, 21, 23,
//     24, 25, 27, 28, 30, 31, 32, 34, 35, 36, 38, 39, 40, 41, 42, 44,
//     45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 54, 55, 56, 57, 57, 58,
//     59, 59, 60, 60, 61, 61, 62, 62, 62, 63, 63, 63, 63, 63, 63, 63,
// };

static int8_t WAVE_SINE[] = {
    32, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 30, 30, 30, 30, 29,
    29, 29, 28, 28, 28, 27, 27, 27, 26, 26, 25, 25, 24, 24, 23, 23,
    22, 22, 21, 20, 20, 19, 19, 18, 17, 17, 16, 15, 15, 14, 13, 12,
    12, 11, 10, 10, 9, 8, 7, 7, 6, 5, 4, 3, 3, 2, 1, 0,
    0, 0, -1, -2, -3, -3, -4, -5, -6, -7, -7, -8, -9, -10, -10, -11,
    -12, -12, -13, -14, -15, -15, -16, -17, -17, -18, -19, -19, -20, -20, -21, -22,
    -22, -23, -23, -24, -24, -25, -25, -26, -26, -27, -27, -27, -28, -28, -28, -29,
    -29, -29, -30, -30, -30, -30, -31, -31, -31, -31, -31, -31, -31, -31, -31, -31,
    -32, -31, -31, -31, -31, -31, -31, -31, -31, -31, -31, -30, -30, -30, -30, -29,
    -29, -29, -28, -28, -28, -27, -27, -27, -26, -26, -25, -25, -24, -24, -23, -23,
    -22, -22, -21, -20, -20, -19, -19, -18, -17, -17, -16, -15, -15, -14, -13, -12,
    -12, -11, -10, -10, -9, -8, -7, -7, -6, -5, -4, -3, -3, -2, -1, 0,
    0, 0, 1, 2, 3, 3, 4, 5, 6, 7, 7, 8, 9, 10, 10, 11,
    12, 12, 13, 14, 15, 15, 16, 17, 17, 18, 19, 19, 20, 20, 21, 22,
    22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 27, 28, 28, 28, 29,
    29, 29, 30, 30, 30, 30, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
};

// static int8_t WAVE3[] = {
//     16, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14,
//     14, 14, 14, 14, 14, 13, 13, 13, 13, 13, 12, 12, 12, 12, 11, 11,
//     11, 11, 10, 10, 10, 9, 9, 9, 8, 8, 8, 7, 7, 7, 6, 6,
//     6, 5, 5, 5, 4, 4, 3, 3, 3, 2, 2, 1, 1, 1, 0, 0,
//     0, 0, 0, -1, -1, -1, -2, -2, -3, -3, -3, -4, -4, -5, -5, -5,
//     -6, -6, -6, -7, -7, -7, -8, -8, -8, -9, -9, -9, -10, -10, -10, -11,
//     -11, -11, -11, -12, -12, -12, -12, -13, -13, -13, -13, -13, -14, -14, -14, -14,
//     -14, -14, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15,
//     -16, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -14,
//     -14, -14, -14, -14, -14, -13, -13, -13, -13, -13, -12, -12, -12, -12, -11, -11,
//     -11, -11, -10, -10, -10, -9, -9, -9, -8, -8, -8, -7, -7, -7, -6, -6,
//     -6, -5, -5, -5, -4, -4, -3, -3, -3, -2, -2, -1, -1, -1, 0, 0,
//     0, 0, 0, 1, 1, 1, 2, 2, 3, 3, 3, 4, 4, 5, 5, 5,
//     6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 10, 11,
//     11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13, 13, 14, 14, 14, 14,
//     14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
// };

static int8_t WAVE_SQUARE[] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
};

error enemies_init(void)
{
    gfx_error err;
    uint8_t i;
    uint8_t index = ENEMY_INDEX - 1;
    for (i = 0; i < MAX_ENEMIES; i++) {
        enemy_t* self        = &ENEMIES[i];
        self->sprite_index = ++index;

        self->sprite_t.tile  = ENEMY_TILE;
        self->sprite_t.flags = SPRITE_FLAGS;
        err                  = gfx_sprite_render(&vctx, index, &self->sprite_t);
        if (err != GFX_SUCCESS)
            return err;


        self->sprite_b.tile  = ENEMY_TILE;
        self->sprite_b.flags = SPRITE_FLAGS | SPRITE_FLIP_Y;
        err                  = gfx_sprite_render(&vctx, ++index, &self->sprite_b);
        if (err != GFX_SUCCESS)
            return err;
    }

    enemies_spawn(SCREEN_HEIGHT / 2);

    return err;
}

error enemies_deinit(void)
{
    uint8_t i;
    gfx_error err;
    for(i = 0; i < MAX_ENEMIES * 2; i++) {
        err = gfx_sprite_set_tile(&vctx, ENEMY_INDEX + i, EMPTY_TILE);
    }
    return err;
}

uint8_t enemies_spawn(uint16_t origin_y) {
    if(spawn_timer != 0) {
        spawn_timer--;
        return 0;
    }

    uint16_t last_x = 0, next_x = SCREEN_WIDTH + SPRITE_WIDTH, next_y = origin_y;
    uint8_t formation = rand8() % 4;
    // formation = F_SINE; // TODO: debug
    uint8_t count = (rand8() % (MAX_ENEMIES - 3)) + 3;
    uint8_t tile = (rand8() % 10) + ENEMY_TILE;
    // make sure we have an even number of ships if > 8
    if((count > 8) && ((count & 0x1) == 1)) count--;

    uint8_t color = rand8() % 4;
    gfx_palette_load(&vctx, &palettes[color], 10, ENEMY_PALETTE);

    for (uint8_t i = 0; i < count; i++) {
        enemy_t* self    = &ENEMIES[i];
        self->active     = 1;
        self->y = next_y;
        self->sprite_t.tile = tile;
        self->sprite_b.tile = tile;

        self->sprite_t.x = next_x; // SCREEN_WIDTH + ((SPRITE_WIDTH * ((i * ENEMY_OFFSET) + (SPRITE_WIDTH/2))) );
        self->x = self->sprite_t.x & 0xFF;
        self->sprite_b.x = self->sprite_t.x;

        self->formation = formation;
        self->f_dir = 1;
        self->sprite_t.y = next_y;
        self->sprite_b.y = next_y + SPRITE_HEIGHT;

        last_x = self->sprite_t.x;
        if(count > 7) {
            if((i & 0x1) == 0) {
                if(origin_y < SCREEN_HEIGHT / 2) {
                    next_y += SCREEN_HEIGHT / 4;
                } else {
                    next_y -= SCREEN_HEIGHT / 4;
                }
            } else { // every other
                next_x += ENEMY_OFFSET;
                next_y = origin_y;
            }
        } else {
            switch(formation) {
                case F_SINE: {
                    if((count & 0x1) == 0) {
                        next_x += ENEMY_OFFSET / 2;
                    } else {
                        next_x += ENEMY_OFFSET;
                    }
                } break;
                default:
                    next_x += ENEMY_OFFSET;
            }
        }
    }

    total_active = count;
    spawn_timer = (rand8() % SPAWN_DERLAY) + 32;
    return 1; // wave spawned
}

inline uint8_t enemies_active(void) {
    return total_active;
}

void enemies_update(void) {
    uint8_t i, r;
    if(total_active == 0) return;

    frames++;

    // once per second?
    // if((frames & 0x3F) == 0) {
    if((frames & 0x1F) == 0) {
        r = (rand8() % (total_active / 2));
        for(i = 0; i < total_active; i++) {
            enemy_t *self = &ENEMIES[i];
            if(!self->active || self->sprite_t.x > SCREEN_WIDTH) {
                // we found our enemy, but he's inactive, use the next
                if(r == i) r++;
                continue;
            }
            // this isn't the enemy you're looking for
            if(r != i) continue;

            total_bullets++;
            if (total_bullets >= ENEMY_MAX_BULLETS);
                total_bullets = 0;

            bullet_t* bullet = &BULLETS[total_bullets + PLAYER_MAX_BULLETS];
            if (bullet->active)
                return; // this bullet is still in use

            bullet->active      = 1;
            bullet->sprite.tile = BULLET_RED;
            bullet->direction.x = DIRECTION_LEFT;
            bullet->sprite.x    = self->sprite_t.x;
            bullet->sprite.y    = self->sprite_b.y + 2;
            sound_play(BULLET_SOUND, 180, 2);


            break; // we only need the first enemy
        }
    }
}

void enemy_move(enemy_t* self)
{
    if (self->active == 0)
        return;

    uint16_t x = self->sprite_t.x;
    uint16_t y = self->sprite_t.y;
    // uint8_t index;
    int8_t offset;
    self->x--;
    switch(self->formation) {
        case F_SINE: {
            // index = self->sprite_t.x & 0xFF; // x % 256
            offset = WAVE_SINE[self->x];

            x--;
            y = self->y + offset;
        } break;
        case F_TRI: {
            // index = self->x & 0xFF; // x % 256
            offset = WAVE_SQUARE[self->x];

            x--;
            y += offset;
        } break;
        case F_SQUARE: {
            // self->x--;
            // index = self->x & 0xFF; // x % 256
            offset = WAVE_SQUARE[self->x];

            if(offset == 0) x--;
            else y += offset;
        } break;
        case F_LINE: // fallthru
        default: {
            x--;
        }
    }

    // screen bounds, reverse
    if(y < 16) {
        y = 16;
        self->f_dir *= -1;
    }
    if(y > (SCREEN_HEIGHT - SPRITE_HEIGHT)) {
        y = (SCREEN_HEIGHT - SPRITE_HEIGHT);
        self->f_dir *= -1;
    }

    self->sprite_t.x = x;
    self->sprite_b.x = x;
    self->sprite_t.y = y;
    self->sprite_b.y = y + SPRITE_HEIGHT;


    if (self->sprite_t.x <= 1) {
        // enemy off screen
        enemy_destroy(self);
    }
}

void enemy_destroy(enemy_t *self) {
    self->active     = 0;
    self->sprite_t.x = SCREEN_WIDTH + (SPRITE_WIDTH * 2);
    self->sprite_b.x = self->sprite_t.x;
    if(total_active > 0) total_active--;
}

void enemies_move(void)
{
    uint8_t i;

    for (i = 0; i < MAX_ENEMIES; i++) {
        enemy_move(&ENEMIES[i]);
    }
}

void enemy_draw(enemy_t* self)
{
    gfx_sprite_render(&vctx, self->sprite_index, &self->sprite_t);
    gfx_sprite_render(&vctx, self->sprite_index + 1, &self->sprite_b);
}

void enemies_draw(void)
{
    uint8_t i;

    for (i = 0; i < MAX_ENEMIES; i++) {
        enemy_draw(&ENEMIES[i]);
    }

    // char text[16];
    // sprintf(text, "%03d", total_active);
    // nprint_string(&vctx, text, 3, 16, 14);
}