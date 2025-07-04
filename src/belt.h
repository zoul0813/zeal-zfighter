#include <stdint.h>
#include <zvb_sprite.h>
#include <zgdk.h>

#ifndef BELT_H
#define BELT_H

#define BELT_INDEX  64

// asteroid belt

typedef enum {
    ASTEROID_SMALL  = 0,
    ASTEROID_LARGE  = 1,
    ASTEROID_LARGE1 = 2,
    ASTEROID_LARGE2 = 3,
    ASTEROID_LARGE3 = 4,
    ASTEROID_LARGE4 = 5,
    ASTEROID_LARGE5 = 6,
} AsteroidSize;
#define ASTEROID_SIZES  7

typedef struct {
    AsteroidSize size;
    Direction direction;
    gfx_sprite* tl;
    gfx_sprite* tr;
    gfx_sprite* bl;
    gfx_sprite* br;
} asteroid_t;

error belt_init(void);
uint8_t belt_spawn(void);
uint8_t belt_update(void);
uint8_t belt_collide(Rect *src);
void belt_destroy(void);
// void belt_draw(void);

#endif