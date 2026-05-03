#include <stdint.h>
#include <zvb_sprite.h>
#include <zgdk.h>

#include "main.h"

#ifndef BULLET_H
#define BULLET_H

#define PLAYER_BULLET_SLOTS 3
#define ENEMY_BULLET_SLOTS  12
#define MAX_BULLETS         (PLAYER_BULLET_SLOTS + ENEMY_BULLET_SLOTS)
#define BULLET_INDEX 16
#define BULLET_GREEN 13
#define BULLET_RED   14
#define BULLET_SPEED 8
#define BULLET_SOUND 3

typedef struct {
        uint8_t active;
        Direction direction;
        gfx_sprite* sprite;
} bullet_t;

extern bullet_t BULLETS[MAX_BULLETS];

error bullet_init(void);
error bullet_deinit(void);
void bullet_move(void);
// void bullet_draw(void);

#endif
