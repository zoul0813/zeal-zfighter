#include <stdint.h>
#include <zvb_sprite.h>
#include <zgdk.h>
#include "main.h"

#ifndef ENEMY_H
#define ENEMY_H

#define ENEMY_INDEX 32
#define MAX_ENEMIES 16

typedef struct {
        uint8_t active;
        uint8_t sprite_index;
        gfx_sprite sprite_t;
        gfx_sprite sprite_b;
} enemy_t;

extern enemy_t ENEMIES[MAX_ENEMIES];

error enemies_init(void);
error enemies_deinit(void);

void enemy_move(enemy_t* self);
void enemy_draw(enemy_t* self);
void enemies_move(void);
void enemies_draw(void);

#endif