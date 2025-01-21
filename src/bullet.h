#include <stdint.h>
#include <zvb_sprite.h>
#include <zgdk.h>

#include "main.h"

#ifndef BULLET_H
#define BULLET_H

#define MAX_BULLETS  16
#define BULLET_INDEX 16
#define BULLET_GREEN 13
#define BULLET_RED   14
#define BULLET_SPEED 8
#define BULLET_SOUND 3

typedef struct {
        uint8_t active;
        uint8_t sprite_index;
        gfx_sprite sprite;
        Direction direction;
} bullet_t;

extern bullet_t BULLETS[MAX_BULLETS];

error bullet_init(void);
error bullet_deinit(void);
void bullet_move(void);
void bullet_draw(void);

#endif