#include <zgdk.h>
#include "enemy.h"
#include "main.h"

#define ENEMY_T 4;

enemy_t ENEMIES[MAX_ENEMIES];

error enemies_init(void)
{
    gfx_error err;
    uint8_t i;
    uint8_t index = ENEMY_INDEX - 1;
    for (i = 0; i < MAX_ENEMIES; i++) {
        enemy_t* self        = &ENEMIES[i];
        self->active         = 1;
        self->sprite_index = ++index;

        self->sprite_t.tile  = ENEMY_T;
        self->sprite_t.x     = SCREEN_WIDTH + (SPRITE_WIDTH * i);
        self->sprite_t.y     = SCREEN_HEIGHT / 2;
        self->sprite_t.flags = SPRITE_NONE;
        err                  = gfx_sprite_render(&vctx, index, &self->sprite_t);
        if (err != GFX_SUCCESS)
            return err;


        self->sprite_b.tile  = ENEMY_T;
        self->sprite_b.x     = self->sprite_t.x;
        self->sprite_b.y     = self->sprite_t.y + SPRITE_HEIGHT;
        self->sprite_b.flags = SPRITE_FLIP_Y;
        err                  = gfx_sprite_render(&vctx, ++index, &self->sprite_b);
        if (err != GFX_SUCCESS)
            return err;
    }

    return err;
}

error enemies_deinit(void)
{
    return ERR_SUCCESS;
}

void enemy_move(enemy_t* self)
{
    if (self->active == 0)
        return;

    self->sprite_t.x--;
    self->sprite_b.x--;

    if (self->sprite_t.x <= 1) {
        // enemy off screen
        // self->active     = 0;
        self->sprite_t.x = SCREEN_WIDTH + SPRITE_WIDTH;
        self->sprite_b.x = SCREEN_WIDTH + SPRITE_WIDTH;
    }
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
}