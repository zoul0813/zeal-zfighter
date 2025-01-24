#include <stdint.h>
#include <zvb_sprite.h>
#include <zgdk.h>

#ifndef BELT_H
#define BELT_H

#define BELT_INDEX  64

// asteroid belt

typedef enum {
    ASTEROID_SMALL = 0,
    ASTEROID_LARGE = 1,
} AsteroidSize;

typedef struct {
    AsteroidSize size;
    gfx_sprite tl;
    gfx_sprite tr;
    gfx_sprite bl;
    gfx_sprite br;
    Direction direction;
} asteroid_t;

void belt_spawn(void);
uint8_t belt_update(void);

#endif