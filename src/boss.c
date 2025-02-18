#include <stdio.h>
#include <zgdk.h>
#include "boss.h"
#include "patterns.h"
#include "main.h"
#include "bullet.h"
#include "player.h"

boss_t boss;

error boss_init(void) {
    gfx_sprite sprite = {
        .tile = 5,
        .flags = SPRITE_FLAGS,
    };

    boss.x = 0;
    boss.y = SCREEN_HEIGHT / 2;
    boss.formation = F_SINE;
    boss.f_dir = -1;
    boss.active = 1;

    boss.sprite_tl = sprites_register(sprite);
    sprite.flags = SPRITE_FLAGS | SPRITE_FLIP_X;
    boss.sprite_tr = sprites_register(sprite);
    sprite.flags = SPRITE_FLAGS | SPRITE_FLIP_Y;
    boss.sprite_bl = sprites_register(sprite);
    sprite.flags = SPRITE_FLAGS | SPRITE_FLIP_Y | SPRITE_FLIP_X;
    boss.sprite_br = sprites_register(sprite);

    return ERR_SUCCESS;
}

error boss_deinit(void) {
    // TODO: deinit
    return ERR_SUCCESS;
}

void boss_move(void) {
    if (boss.active == 0)
        return;

    // boss_t *self = &boss;

    uint16_t x = boss.x, y = boss.y;

    // uint8_t index;
    int8_t offset;
    boss.y += boss.f_dir;
    switch (boss.formation) {
        case F_SINE: {
            offset = WAVE_SINE[boss.y];

            y--;
            x = boss.x + (offset * 2);
        } break;
        case F_TRI: {
            offset = WAVE_SQUARE[boss.y];

            y--;
            x += offset;
        } break;
        case F_SQUARE: {
            offset = WAVE_SQUARE[boss.y];

            if (offset == 0)
                y--;
            else
                x += offset;
        } break;
        case F_LINE: // fallthru
        default: {
            y--;
        }
    }

    // // screen bounds, reverse
    if (y < 16) {
        y            = 16;
        boss.f_dir = 1;
    }
    if (y > (SCREEN_HEIGHT - SPRITE_HEIGHT)) {
        y            = (SCREEN_HEIGHT - SPRITE_HEIGHT);
        boss.f_dir = -1;
    }

    boss.sprite_tl->x = 240 + x;
    boss.sprite_tr->x = 240 + x + SPRITE_WIDTH;
    boss.sprite_bl->x = 240 + x;
    boss.sprite_br->x = 240 + x + SPRITE_WIDTH;

    boss.sprite_tl->y = y;
    boss.sprite_tr->y = y;
    boss.sprite_bl->y = y + SPRITE_HEIGHT;
    boss.sprite_br->y = y + SPRITE_HEIGHT;
}

void boss_update(void) {

}

uint8_t boss_active(void) {
    return boss.active;
}

void boss_spawn(void) {
    boss.active = 1;
}
