#include <stdio.h>
#include <zgdk.h>
#include "enemy.h"
#include "patterns.h"
#include "main.h"
#include "bullet.h"
#include "player.h"

#define SPAWN_DERLAY 128

enemy_t ENEMIES[MAX_ENEMIES];
// gfx_sprite ENEMY_SPRITES[MAX_ENEMIES * 2];

static uint8_t total_active    = 0;
static uint8_t total_bullets   = 0;
static uint8_t spawn_timer     = 0;
static uint16_t palettes[4][5] = {
    {
     0x4244, 0x6326,
     0x8CA8, 0x9D2A,
     0xADAC, }, // yellow
    {
     0x2109, 0x318C,
     0x42D2, 0x5394,
     0x6436, }, // dark blue
    {
     0x2249, 0x32AC,
     0x43F2, 0x5454,
     0x65B6, }, // light blue
    {
     0x2246, 0x3327,
     0x44AA, 0x552B,
     0x65AD, }, // green
};

error enemies_init(void)
{
    // gfx_error err;
    uint8_t i;
    gfx_sprite sprite = {
        .tile  = ENEMY_TILE,
        .flags = SPRITE_FLAGS,
    };

    for (i = 0; i < MAX_ENEMIES; i++) {
        enemy_t* self = &ENEMIES[i];

        self->sprite_t         = sprites_register(sprite);
        self->sprite_b         = sprites_register(sprite);
        self->sprite_b->flags |= SPRITE_FLIP_Y;
    }

    // enemies_draw();
    enemies_spawn(SCREEN_HEIGHT / 2);

    return ERR_SUCCESS;
}

error enemies_deinit(void)
{
    // TODO: properly deinit the sprites that were initialized
    uint8_t i;
    gfx_error err;
    for (i = 0; i < MAX_ENEMIES * 2; i++) {
        err = gfx_sprite_set_tile(&vctx, ENEMY_INDEX + i, EMPTY_TILE);
    }
    return err;
}

uint8_t enemies_spawn(uint16_t origin_y)
{
    if (spawn_timer != 0) {
        spawn_timer--;
        return 0;
    }

    uint16_t last_x = 0, next_x = SCREEN_WIDTH + SPRITE_WIDTH, next_y = origin_y;
    uint8_t formation = rand8() % 4;
    // formation = F_SINE; // TODO: debug
    uint8_t count = (rand8() % (MAX_ENEMIES - 3)) + 3;
    uint8_t tile  = (rand8() % 10) + ENEMY_TILE;
    // make sure we have an even number of ships if > 8
    if ((count > 8) && ((count & 0x1) == 1))
        count--;

    uint8_t color = rand8() % 4;
    gfx_palette_load(&vctx, &palettes[color], 10, ENEMY_PALETTE);

    for (uint8_t i = 0; i < count; i++) {
        enemy_t* self        = &ENEMIES[i];
        self->active         = 1;
        self->y              = next_y;
        self->sprite_t->tile = tile;
        self->sprite_b->tile = tile;

        self->sprite_t->x = next_x; // SCREEN_WIDTH + ((SPRITE_WIDTH * ((i * ENEMY_OFFSET) + (SPRITE_WIDTH/2))) );
        self->x           = self->sprite_t->x & 0xFF;
        self->sprite_b->x = self->sprite_t->x;

        self->formation   = formation;
        self->f_dir       = 1;
        self->sprite_t->y = next_y;
        self->sprite_b->y = next_y + SPRITE_HEIGHT;

        last_x = self->sprite_t->x;
        if (count > 7) {
            if ((i & 0x1) == 0) {
                if (origin_y < SCREEN_HEIGHT / 2) {
                    next_y += SCREEN_HEIGHT / 4;
                } else {
                    next_y -= SCREEN_HEIGHT / 4;
                }
            } else { // every other
                next_x += ENEMY_OFFSET;
                next_y  = origin_y;
            }
        } else {
            switch (formation) {
                case F_SINE: {
                    if ((count & 0x1) == 0) {
                        next_x += ENEMY_OFFSET / 2;
                    } else {
                        next_x += ENEMY_OFFSET;
                    }
                } break;
                default: next_x += ENEMY_OFFSET;
            }
        }
    }

    total_active = count;
    spawn_timer  = (rand8() % SPAWN_DERLAY) + 32;
    return 1; // wave spawned
}

inline uint8_t enemies_active(void)
{
    return total_active;
}

void enemies_update(void)
{
    uint8_t i, r;
    if (total_active == 0)
        return;

    frames++;

    // once per second?
    // if((frames & 0x3F) == 0) {
    if ((frames & 0x1F) == 0) {
        r = (rand8() % (total_active >> 1));
        for (i = r; i < MAX_ENEMIES; i++) {
            enemy_t* self = &ENEMIES[i];
            if ((self->active == 0) || (self->sprite_t->x < player.sprite_tr->x)) {
                // we found our enemy, but he's inactive, use the next
                if (r == i)
                    r++;
                continue;
            }

            total_bullets++;
            if (total_bullets >= ENEMY_MAX_BULLETS)
                ;
            total_bullets = 0;

            bullet_t* bullet = &BULLETS[total_bullets + PLAYER_MAX_BULLETS];
            if (bullet->active)
                break; // this bullet is still in use

            bullet->active       = 1;
            bullet->sprite->tile = BULLET_RED;
            bullet->direction.x  = DIRECTION_LEFT;
            bullet->sprite->x    = self->sprite_t->x;
            bullet->sprite->y    = self->sprite_b->y + 2;
            sound_play(BULLET_SOUND, 180, 2);


            break; // we only need the first enemy
        }
    }
}

void enemy_move(enemy_t* self)
{
    if (self->active == 0)
        return;

    uint16_t x = self->sprite_t->x;
    uint16_t y = self->sprite_t->y;
    // uint8_t index;
    int8_t offset;
    self->x--;
    switch (self->formation) {
        case F_SINE: {
            offset = WAVE_SINE[self->x];

            x--;
            y = self->y + offset;
        } break;
        case F_TRI: {
            offset = WAVE_SQUARE[self->x];

            x--;
            y += offset;
        } break;
        case F_SQUARE: {
            offset = WAVE_SQUARE[self->x];

            if (offset == 0)
                x--;
            else
                y += offset;
        } break;
        case F_LINE: // fallthru
        default: {
            x--;
        }
    }

    // screen bounds, reverse
    if (y < 16) {
        y            = 16;
        self->f_dir *= -1;
    }
    if (y > (SCREEN_HEIGHT - SPRITE_HEIGHT)) {
        y            = (SCREEN_HEIGHT - SPRITE_HEIGHT);
        self->f_dir *= -1;
    }

    self->sprite_t->x = x;
    self->sprite_b->x = x;
    self->sprite_t->y = y;
    self->sprite_b->y = y + SPRITE_HEIGHT;


    if (self->sprite_t->x <= 1) {
        // enemy off screen
        enemy_destroy(self);
    }
}

void enemy_destroy(enemy_t* self)
{
    self->active      = 0;
    self->sprite_t->x = SCREEN_WIDTH + (SPRITE_WIDTH * 2);
    self->sprite_b->x = SCREEN_WIDTH + (SPRITE_WIDTH * 2);
    if (total_active > 0)
        total_active--;
}

void enemies_move(void)
{
    uint8_t i;

    for (i = 0; i < MAX_ENEMIES; i++) {
        enemy_move(&ENEMIES[i]);
    }
}
