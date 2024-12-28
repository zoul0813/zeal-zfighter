#include <zgdk.h>
#include "enemy.h"
#include "main.h"

enemy_t ENEMIES[MAX_ENEMIES];

error enemy_init(void) {
    gfx_error err;
    uint8_t i;
    uint8_t index = 31;
    for(i = 0; i < MAX_ENEMIES; i++) {
        enemy_t *self = &ENEMIES[i];
        self->sprite_t.tile  = 4;
        self->sprite_t.x     = (SCREEN_WIDTH / 2) + (SPRITE_WIDTH * i);
        self->sprite_t.y     = SCREEN_HEIGHT / 2;
        self->sprite_t.flags = SPRITE_NONE;
        err                    = gfx_sprite_render(&vctx, ++index, &self->sprite_t);
        if (err != GFX_SUCCESS)
            return err;

        self->sprite_b.tile  = 4;
        self->sprite_b.x     = self->sprite_t.x;
        self->sprite_b.y     = self->sprite_t.y + SPRITE_HEIGHT;
        self->sprite_b.flags = SPRITE_FLIP_Y;
        err                    = gfx_sprite_render(&vctx, ++index, &self->sprite_b);
        if (err != GFX_SUCCESS)
            return err;
    }

    return err;
}

error enemy_deinit(void) {
    return ERR_SUCCESS;
}

void enemy_move(enemy_t *self) {
    self;//unrefed
}

void enemy_draw(enemy_t *self) {
    gfx_sprite_render(&vctx, self->sprite_index, &self->sprite_t);
    gfx_sprite_render(&vctx, self->sprite_index, &self->sprite_b);
}
