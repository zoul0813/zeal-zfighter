#include <stdint.h>
#include <zvb_sprite.h>
#include <zgdk.h>
#include "main.h"

#ifndef ENEMY_H
#define ENEMY_H

#define ENEMY_INDEX     32
#define MAX_ENEMIES     16
#define ENEMY_OFFSET    48

typedef enum {
    F_SINE      = 0,
    F_TRI       = 1,
    F_SQUARE    = 2,
    F_LINE      = 3,
} Formation;

typedef struct {
        uint8_t active;
        uint8_t y;
        uint8_t x;

        Formation formation;
        uint8_t f_dir;

        uint8_t sprite_index;
        gfx_sprite sprite_t;
        gfx_sprite sprite_b;
} enemy_t;

extern enemy_t ENEMIES[MAX_ENEMIES];

error enemies_init(void);
error enemies_deinit(void);

void enemy_move(enemy_t* self);
void enemy_draw(enemy_t* self);
void enemy_destroy(enemy_t *self);
void enemies_move(void);
void enemies_draw(void);
void enemies_spawn(uint16_t y);
uint8_t enemies_active(void);
#endif