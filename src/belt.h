#include <stdint.h>
#include <zvb_sprite.h>
#include <zgdk.h>

#ifndef BELT_H
#define BELT_H

#define BELT_INDEX  64

// asteroid belt

typedef enum {
    ASTEROID_SMALL = 0,
    ASTEROID_LARGE1 = 1,
    ASTEROID_LARGE2 = 2,
    ASTEROID_LARGE3 = 3,
    ASTEROID_LARGE4 = 4,
    ASTEROID_LARGE5 = 5,
} AsteroidSize;
#define ASTEROID_SIZES  6

typedef struct {
    AsteroidSize size;
    gfx_sprite tl;
    gfx_sprite tr;
    gfx_sprite bl;
    gfx_sprite br;
    Direction direction;
} asteroid_t;

uint8_t belt_spawn(void);
uint8_t belt_update(void);
uint8_t belt_collide(Rect *src);
void belt_destroy(void);
void belt_draw(void);

#endif