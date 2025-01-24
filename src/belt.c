#include "main.h"
#include "belt.h"

static asteroid_t asteroid;

void belt_spawn(void) {
    uint8_t size = rand8() % 2;
    asteroid.direction.x = DIRECTION_LEFT;
    asteroid.direction.y = DIRECTION_UP;
    asteroid.size = size;
    switch(size) {
        case ASTEROID_SMALL: {
            asteroid.tl.tile = 12;
            asteroid.tr.tile = EMPTY_TILE;
            asteroid.bl.tile = EMPTY_TILE;
            asteroid.br.tile = EMPTY_TILE;
        } break;
        default: {
            asteroid.tl.tile = 8;
            asteroid.tr.tile = 9;
            asteroid.bl.tile = 10;
            asteroid.br.tile = 10;
        }
    }

    asteroid.tr.x = SCREEN_WIDTH + SPRITE_WIDTH + SPRITE_WIDTH;
    asteroid.tr.y = SCREEN_HEIGHT + SPRITE_HEIGHT + SPRITE_HEIGHT;

    asteroid.tl.x = asteroid.tr.x - SPRITE_WIDTH;
    asteroid.br.x = asteroid.tr.x;
    asteroid.bl.x = asteroid.tr.x - SPRITE_WIDTH;

    asteroid.tl.y = asteroid.tr.y;
    asteroid.br.y = asteroid.tr.y + SPRITE_HEIGHT;
    asteroid.bl.y = asteroid.tr.y + SPRITE_HEIGHT;
}

uint8_t belt_update(void) {

    asteroid.tl.x += asteroid.direction.x;
    asteroid.tr.x += asteroid.direction.x;
    asteroid.bl.x += asteroid.direction.x;
    asteroid.br.x += asteroid.direction.x;

    asteroid.tl.y += asteroid.direction.y;
    asteroid.tr.y += asteroid.direction.y;
    asteroid.bl.y += asteroid.direction.y;
    asteroid.br.y += asteroid.direction.y;

    gfx_sprite_render(&vctx, BELT_INDEX+0, &asteroid.tl);
    gfx_sprite_render(&vctx, BELT_INDEX+1, &asteroid.tr);
    gfx_sprite_render(&vctx, BELT_INDEX+2, &asteroid.bl);
    gfx_sprite_render(&vctx, BELT_INDEX+3, &asteroid.br);

    uint8_t offscreen = (asteroid.tr.x < SPRITE_WIDTH) && (asteroid.bl.y < SPRITE_HEIGHT);
    if(offscreen) {
        return rand8() % 5 + 5;
    }

    return 0;
}