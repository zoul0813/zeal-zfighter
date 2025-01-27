#include <zgdk.h>
#include "player.h"
#include "main.h"

#define PLAYER_TL 2
#define PLAYER_TR 3

#define PLAYER_TL_DMG 16
#define PLAYER_TR_DMG 17
#define PLAYER_BL_DMG 18
#define PLAYER_BR_DMG 19

player_t player;
static uint8_t damaged = 0;

// gfx_sprite PLAYER_SPRITES[4 + 8]; // 4 ship, 8 shield

void player_shield(uint8_t active)
{
    player.shield_active = active;
    if (active) {
        player.sprite_shield_tl->tile = 20;
        player.sprite_shield_t->tile  = 21;
        player.sprite_shield_tr->tile = 22;
        player.sprite_shield_r1->tile = 23;
        player.sprite_shield_r2->tile = 23;
        player.sprite_shield_br->tile = 22;
        player.sprite_shield_b->tile  = 21;
        player.sprite_shield_bl->tile = 20;
    } else {
        player.sprite_shield_tl->tile = EMPTY_TILE;
        player.sprite_shield_t->tile  = EMPTY_TILE;
        player.sprite_shield_tr->tile = EMPTY_TILE;
        player.sprite_shield_r1->tile = EMPTY_TILE;
        player.sprite_shield_r2->tile = EMPTY_TILE;
        player.sprite_shield_br->tile = EMPTY_TILE;
        player.sprite_shield_b->tile  = EMPTY_TILE;
        player.sprite_shield_bl->tile = EMPTY_TILE;
    }
}

error player_init(void)
{
    uint8_t index = 0;

    player.lives       = 3;
    player.health      = 5;
    player.bullet      = 0;
    player.bullet_lock = 0;

    gfx_sprite sprite = {
        .x = 64,
        .y = 64,
        .flags = SPRITE_FLAGS
    };
    // ship
    {
        player.sprite_tl = sprites_register_sprite(sprite);
        player.sprite_tr = sprites_register_sprite(sprite);
        player.sprite_bl = sprites_register_sprite(sprite);
        player.sprite_bl->flags |= SPRITE_FLIP_Y;
        player.sprite_br = sprites_register_sprite(sprite);
        player.sprite_br->flags |= SPRITE_FLIP_Y;
    }

    // shield
    {
        player.shield_active           = 1;
        player.sprite_shield_tl = sprites_register_sprite(sprite);
        player.sprite_shield_t = sprites_register_sprite(sprite);
        player.sprite_shield_tr = sprites_register_sprite(sprite);
        player.sprite_shield_r1 = sprites_register_sprite(sprite);

        sprite.flags |= SPRITE_FLIP_Y;
        player.sprite_shield_r2 = sprites_register_sprite(sprite);
        player.sprite_shield_br = sprites_register_sprite(sprite);
        player.sprite_shield_b = sprites_register_sprite(sprite);
        player.sprite_shield_bl = sprites_register_sprite(sprite);

    }

    player.rect.x = player.sprite_tl->x;
    player.rect.y = player.sprite_tl->y;
    player.rect.w = SPRITE_WIDTH * 2;
    player.rect.h = SPRITE_HEIGHT * 2;

    player_spawn();

    return ERR_SUCCESS;
}

void player_spawn(void)
{
    player.health = PLAYER_MAX_HEALTH;
    player_shield(1);

    player.sprite_tl->tile = PLAYER_TL;
    player.sprite_tr->tile = PLAYER_TR;
    player.sprite_bl->tile = PLAYER_TL;
    player.sprite_br->tile = PLAYER_TR;

    player.sprite_tl->x = 64;
    player.sprite_tl->y = 64;

    player_draw_lives(0, HEIGHT - 1);
}

error player_deinit(void)
{
    gfx_error err;
    uint8_t i;
    for (i = 0; i < 12; i++) {
        err = gfx_sprite_set_tile(&vctx, i, EMPTY_TILE);
    }

    return err;
}

void player_shoot(void)
{
    if (player.bullet_lock > 0)
        return;
    player.bullet++;
    if (player.bullet >= PLAYER_MAX_BULLETS)
        player.bullet = 0;
    bullet_t* bullet = &BULLETS[player.bullet];
    if (bullet->active)
        return; // this bullet is still in use

    bullet->active       = 1;
    bullet->sprite->tile = BULLET_GREEN;
    bullet->direction.x  = DIRECTION_RIGHT;
    bullet->sprite->x    = player.sprite_br->x;
    bullet->sprite->y    = player.sprite_br->y + 2;
    player.bullet_lock   = PLAYER_BULLET_FREQ;

    sound_play(BULLET_SOUND, 220, 2);
}

void player_damaged(uint8_t damage)
{
    // uint16_t color = PLAYER_HIT_COLOR2;
    // gfx_palette_load(&vctx, &color, sizeof(uint16_t), PLAYER_HIT_INDEX);

    if (player.health <= damage) {
        player.health          = 0;
        player.sprite_tl->tile = PLAYER_TL;
        player.sprite_tr->tile = PLAYER_TR;
        player.sprite_bl->tile = PLAYER_TL;
        player.sprite_br->tile = PLAYER_TR;
        player.shield_active   = 1;
        return;
    }

    player.health -= damage;
    if (player.health < 6) {
        player_shield(0);
    }
    if (player.health < 5) {
        player.sprite_tl->tile = PLAYER_TL_DMG;
    }
    if (player.health < 4) {
        player.sprite_br->tile = PLAYER_BR_DMG;
    }
    if (player.health < 3) {
        player.sprite_bl->tile = PLAYER_BL_DMG;
    }
    if (player.health < 2) {
        player.sprite_tr->tile = PLAYER_TR_DMG;
    }

    uint16_t color = PLAYER_COLOR2;
    gfx_palette_load(&vctx, &color, sizeof(uint16_t), PLAYER_HIT_INDEX);
    damaged = 12;
}

void player_score(uint8_t score)
{
    if (score > 0) {
        player.score += score;
    } else {
        player.score = 0;
    }
    sprintf(buffer, "%05d", player.score);
    nprint_string(&vctx, buffer, 5, 0, 0);
}

uint8_t player_destroyed(void)
{
    player.health = PLAYER_MAX_HEALTH;
    player.lives--;

    player_draw_lives(0, HEIGHT - 1);

    return player.lives;
}

void player_update(void)
{
    if (player.bullet_lock > 0)
        player.bullet_lock--;
    if (damaged > 0)
        damaged--;
    if (damaged == 0) {
        uint16_t color = PLAYER_COLOR1;
        gfx_palette_load(&vctx, &color, sizeof(uint16_t), PLAYER_HIT_INDEX);
    }
}

void player_move(void)
{
    int16_t x  = player.sprite_tl->x;
    int16_t y  = player.sprite_tl->y;
    x         += (player.direction.x * PLAYER_SPEED);
    y         += (player.direction.y * PLAYER_SPEED);

    if (x < (SPRITE_WIDTH / 2)) {
        x = (SPRITE_WIDTH / 2);
    } else if (x > SCREEN_WIDTH - (SPRITE_WIDTH / 2)) {
        x = SCREEN_WIDTH - (SPRITE_WIDTH / 2);
    }

    if (y < 0) {
        y = 1;
    } else if (y > SCREEN_HEIGHT) {
        y = SCREEN_HEIGHT;
    }
    // ship
    {
        player.sprite_tl->x = x;
        player.sprite_tl->y = y;

        player.sprite_tr->x = x + SPRITE_WIDTH;
        player.sprite_tr->y = y;

        player.sprite_bl->x = x;
        player.sprite_bl->y = y + SPRITE_HEIGHT;

        player.sprite_br->x = x + SPRITE_WIDTH;
        player.sprite_br->y = y + SPRITE_HEIGHT;
    }

    // shield
    {
        player.sprite_shield_tl->x = player.sprite_tl->x;
        player.sprite_shield_tl->y = player.sprite_tl->y - SPRITE_HEIGHT;

        player.sprite_shield_t->x = player.sprite_tr->x;
        player.sprite_shield_t->y = player.sprite_tr->y - SPRITE_HEIGHT;

        player.sprite_shield_tr->x = player.sprite_tr->x + SPRITE_HEIGHT;
        player.sprite_shield_tr->y = player.sprite_tr->y - SPRITE_WIDTH;

        player.sprite_shield_r1->x = player.sprite_tr->x + SPRITE_WIDTH;
        player.sprite_shield_r1->y = player.sprite_tr->y;

        player.sprite_shield_r2->x = player.sprite_br->x + SPRITE_WIDTH;
        player.sprite_shield_r2->y = player.sprite_br->y;

        player.sprite_shield_br->x = player.sprite_br->x + SPRITE_WIDTH;
        player.sprite_shield_br->y = player.sprite_br->y + SPRITE_HEIGHT;

        player.sprite_shield_b->x = player.sprite_br->x;
        player.sprite_shield_b->y = player.sprite_br->y + SPRITE_HEIGHT;

        player.sprite_shield_bl->x = player.sprite_bl->x;
        player.sprite_shield_bl->y = player.sprite_bl->y + SPRITE_HEIGHT;
    }

    player.rect.x = player.sprite_tl->x;
    player.rect.y = player.sprite_tl->y;
}

void player_attract(void)
{
    if (player.sprite_tl->x > 100)
        player.direction.x = DIRECTION_LEFT;
    if (player.sprite_tl->x < 32)
        player.direction.x = DIRECTION_RIGHT;

    if (player.sprite_tl->y > 200)
        player.direction.y = DIRECTION_UP;
    if (player.sprite_tl->y < 32)
        player.direction.y = DIRECTION_DOWN;

    player_shoot();
}

// void player_draw(void)
// {
//     gfx_sprite_render_array(&vctx, 0, PLAYER_SPRITES, 12);
// }

void player_draw_lives(uint8_t x, uint8_t y)
{
    uint8_t lives[3];
    for (uint8_t l = 1; l < 4; l++) {
        if (player.lives >= l) {
            // gfx_tilemap_place(&vctx, 15, 1, l-1, 14);
            lives[l - 1] = PLAYER_LIVES_TILE;
        } else {
            lives[l - 1] = EMPTY_TILE;
        }
    }
    gfx_tilemap_load(&vctx, lives, 3, 1, x, y);
}
