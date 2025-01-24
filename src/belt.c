#include "main.h"
#include "belt.h"

static asteroid_t asteroid;

uint8_t belt_spawn(void) {
    uint8_t size = rand8() % ASTEROID_SIZES;
    asteroid.direction.x = DIRECTION_LEFT    *  ((rand8_quick() % 3) + 1);
    asteroid.direction.y = DIRECTION_UP      *  ((rand8() % 3) + 1);

    asteroid.size = size;
    switch(size) {
        case ASTEROID_SMALL: {
            asteroid.tl.tile = 12;
            // let it use whatever flags were already set, somewhat "random" orientation?
            asteroid.tr.tile = EMPTY_TILE;
            asteroid.bl.tile = EMPTY_TILE;
            asteroid.br.tile = EMPTY_TILE;
        } break;
        case ASTEROID_LARGE1: {
            asteroid.tl.tile = 8;
            asteroid.tr.tile = 8;
            asteroid.bl.tile = 8;
            asteroid.br.tile = 8;
            asteroid.tl.flags = SPRITE_NONE;
            asteroid.tr.flags = SPRITE_FLIP_X;
            asteroid.bl.flags = SPRITE_FLIP_Y;
            asteroid.br.flags = SPRITE_FLIP_X | SPRITE_FLIP_Y;
        } break;
        case ASTEROID_LARGE2: {
            asteroid.tl.tile = 9;
            asteroid.tr.tile = 9;
            asteroid.bl.tile = 10;
            asteroid.br.tile = 11;
            asteroid.tl.flags = SPRITE_FLIP_X;
            asteroid.tr.flags = SPRITE_NONE;
            asteroid.bl.flags = SPRITE_NONE;
            asteroid.br.flags = SPRITE_NONE;
        } break;
        case ASTEROID_LARGE3: {
            asteroid.tl.tile = 10;
            asteroid.tr.tile = 9;
            asteroid.bl.tile = 10;
            asteroid.br.tile = 11;
            asteroid.tl.flags = SPRITE_FLIP_Y;
            asteroid.tr.flags = SPRITE_NONE;
            asteroid.bl.flags = SPRITE_NONE;
            asteroid.br.flags = SPRITE_NONE;
        } break;
        case ASTEROID_LARGE4: {
            asteroid.tl.tile = 11;
            asteroid.tr.tile = 11;
            asteroid.bl.tile = 11;
            asteroid.br.tile = 11;
            asteroid.tl.flags = SPRITE_FLIP_X | SPRITE_FLIP_Y;
            asteroid.tr.flags = SPRITE_FLIP_Y;
            asteroid.bl.flags = SPRITE_FLIP_X;
            asteroid.br.flags = SPRITE_NONE;
        } break;
        case ASTEROID_LARGE5: {
            asteroid.tl.tile = 10;
            asteroid.tr.tile = 8;
            asteroid.bl.tile = 11;
            asteroid.br.tile = 9;
            asteroid.tl.flags = SPRITE_FLIP_Y;
            asteroid.tr.flags = SPRITE_FLIP_X;
            asteroid.bl.flags = SPRITE_FLIP_X;
            asteroid.br.flags = SPRITE_FLIP_Y;
        } break;
        default: {
            asteroid.tl.tile = 8;
            asteroid.tr.tile = 9;
            asteroid.bl.tile = 10;
            asteroid.br.tile = 11;
            asteroid.tl.flags = SPRITE_NONE;
            asteroid.tr.flags = SPRITE_NONE;
            asteroid.bl.flags = SPRITE_NONE;
            asteroid.br.flags = SPRITE_NONE;
        }
    }


    uint16_t rx = (rand8_quick() % (SCREEN_WIDTH / 2)) + (SCREEN_WIDTH / 2) + (SPRITE_WIDTH * 2);
    // asteroid.tr.x = SCREEN_WIDTH + SPRITE_WIDTH + SPRITE_WIDTH;
    asteroid.tr.x = rx;
    asteroid.tr.y = SCREEN_HEIGHT + SPRITE_HEIGHT + SPRITE_HEIGHT;

    asteroid.tl.x = asteroid.tr.x - SPRITE_WIDTH;
    asteroid.br.x = asteroid.tr.x;
    asteroid.bl.x = asteroid.tr.x - SPRITE_WIDTH;

    asteroid.tl.y = asteroid.tr.y;
    asteroid.br.y = asteroid.tr.y + SPRITE_HEIGHT;
    asteroid.bl.y = asteroid.tr.y + SPRITE_HEIGHT;

    return 0;
}

uint8_t belt_update(void) {
    int8_t x = asteroid.direction.x;
    int8_t y = asteroid.direction.y;
    asteroid.tl.x += x;
    asteroid.tr.x += x;
    asteroid.bl.x += x;
    asteroid.br.x += x;

    asteroid.tl.y += y;
    asteroid.tr.y += y;
    asteroid.bl.y += y;
    asteroid.br.y += y;

    uint8_t offscreen = (asteroid.tr.x < SPRITE_WIDTH) || (asteroid.bl.y < SPRITE_HEIGHT);
    if(offscreen) {
        belt_destroy();
        return 0;
    }

    return 1;
}

void belt_destroy(void) {
    // asteroid.tr.x = SCREEN_WIDTH + SPRITE_WIDTH + SPRITE_WIDTH;
    // asteroid.tl.x = asteroid.tr.x - SPRITE_WIDTH;
    // asteroid.br.x = asteroid.tr.x;
    // asteroid.bl.x = asteroid.tr.x - SPRITE_WIDTH;

    asteroid.tl.y = SCREEN_HEIGHT + SPRITE_HEIGHT;
    asteroid.tr.y = asteroid.tl.y;
    asteroid.bl.y = asteroid.tl.y + SPRITE_HEIGHT;
    asteroid.br.y = asteroid.tl.y + SPRITE_HEIGHT;
}

uint8_t belt_collide(Rect *src) {
    Rect dst;
    dst.x = asteroid.tl.x;
    dst.y = asteroid.tl.y;

    switch(asteroid.size) {
        case ASTEROID_SMALL:
            dst.w = SPRITE_WIDTH;
            dst.h = SPRITE_WIDTH;
            break;
        default:
            dst.w = SPRITE_WIDTH * 2;
            dst.h = SPRITE_WIDTH * 2;
    }
    // return rect_collide(src, &dst);

    uint8_t left = (src->x + src->w > dst.x) && (src->x + src->w < dst.x + dst.w);
    uint8_t right = (src->x > dst.x) && (src->x < dst.x + dst.w);
    uint8_t top = (src->y + src->h > dst.y) && (src->y + src->h < dst.y + dst.h);
    uint8_t bottom = (src->y > dst.y) && (src->y < dst.y + dst.h);

    return (left || right) && (top || bottom);
}

void belt_draw(void) {
    gfx_sprite_render_array(&vctx, BELT_INDEX, &asteroid.tl, 4);
}