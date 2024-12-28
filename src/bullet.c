#include <zgdk.h>
#include "bullet.h"

bullet_t BULLETS[MAX_BULLETS];

error bullet_init(void)
{
    uint8_t i;

    for (i = 0; i < MAX_BULLETS; i++) {
        bullet_t* bullet     = &BULLETS[i];
        bullet->active       = 0;
        bullet->sprite.tile  = BULLET_GREEN;
        bullet->sprite.x     = 0;
        bullet->sprite.y     = 0;
        bullet->sprite.flags = SPRITE_NONE;
        bullet->sprite_index = BULLET_INDEX + i;
        gfx_sprite_render(&vctx, bullet->sprite_index, &bullet->sprite);
    }

    return 0;
}

error bullet_deinit(void)
{
    return 0;
}

void bullet_move(void)
{
    uint8_t i;
    for (i = 0; i < MAX_BULLETS; i++) {
        bullet_t* bullet = &BULLETS[i];
        if (bullet->active) {
            bullet->sprite.x += BULLET_SPEED * bullet->direction.x;

            if ((bullet->sprite.x <= 1) || (bullet->sprite.x > SCREEN_WIDTH)) {
                bullet->active   = 0;
                bullet->sprite.x = 0;
                bullet->sprite.y = 0;
            }
        }
    }
}

void bullet_draw(void)
{
    uint8_t i;
    for (i = 0; i < MAX_BULLETS; i++) {
        bullet_t* bullet = &BULLETS[i];
        gfx_sprite_render(&vctx, bullet->sprite_index, &bullet->sprite);
    }
}
