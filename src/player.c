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

#define SPRITE_FLAGS    SPRITE_BEHIND_FG

void player_shield(uint8_t active) {
    player.shield.active = active;
    if (active) {
        player.shield.sprite_tl.tile = 20;
        player.shield.sprite_t.tile  = 21;
        player.shield.sprite_tr.tile = 22;
        player.shield.sprite_r1.tile = 23;
        player.shield.sprite_r2.tile = 23;
        player.shield.sprite_br.tile = 22;
        player.shield.sprite_b.tile  = 21;
        player.shield.sprite_bl.tile = 20;
    } else {
        player.shield.sprite_tl.tile = EMPTY_TILE;
        player.shield.sprite_t.tile  = EMPTY_TILE;
        player.shield.sprite_tr.tile = EMPTY_TILE;
        player.shield.sprite_r1.tile = EMPTY_TILE;
        player.shield.sprite_r2.tile = EMPTY_TILE;
        player.shield.sprite_br.tile = EMPTY_TILE;
        player.shield.sprite_b.tile  = EMPTY_TILE;
        player.shield.sprite_bl.tile = EMPTY_TILE;
    }
}

error player_init(void)
{
    gfx_error err;
    uint8_t index       = 0;
    player.sprite_index = index;
    player.lives        = 3;
    player.health       = 5;
    player.bullet       = 0;
    player.bullet_lock  = 0;

    // ship
    {
        player.sprite_tl.x     = 64;
        player.sprite_tl.y     = 64;
        player.sprite_tl.flags = SPRITE_FLAGS;
        err                    = gfx_sprite_render(&vctx, index, &player.sprite_tl);
        if (err != GFX_SUCCESS)
            return err;

        player.sprite_tr.flags = SPRITE_FLAGS;
        err                    = gfx_sprite_render(&vctx, ++index, &player.sprite_tr);
        if (err != GFX_SUCCESS)
            return err;

        player.sprite_bl.flags = SPRITE_FLAGS | SPRITE_FLIP_Y;
        err                    = gfx_sprite_render(&vctx, ++index, &player.sprite_bl);
        if (err != GFX_SUCCESS)
            return err;

        player.sprite_br.flags = SPRITE_FLAGS | SPRITE_FLIP_Y;
        err                    = gfx_sprite_render(&vctx, ++index, &player.sprite_br);
        if (err != GFX_SUCCESS)
            return err;
    }

    // shield
    {
        shield_t* shield = &player.shield;
        index++;
        shield->sprite_index = index;
        shield->active       = 1;

        // shield->sprite_tl.tile  = 20;
        // shield->sprite_tl.x     = player.sprite_tl.x;
        // shield->sprite_tl.y     = player.sprite_tl.y - SPRITE_HEIGHT;
        shield->sprite_tl.flags = SPRITE_FLAGS;
        err                     = gfx_sprite_render(&vctx, index, &shield->sprite_tl);
        if (err != GFX_SUCCESS)
            return err;

        // shield->sprite_t.tile  = 21;
        // shield->sprite_t.x     = player.sprite_tr.x;
        // shield->sprite_t.y     = player.sprite_tr.y - SPRITE_HEIGHT;
        shield->sprite_t.flags = SPRITE_FLAGS;
        err                    = gfx_sprite_render(&vctx, ++index, &shield->sprite_t);
        if (err != GFX_SUCCESS)
            return err;

        // shield->sprite_tr.tile  = 22;
        // shield->sprite_tr.x     = player.sprite_tr.x + SPRITE_WIDTH;
        // shield->sprite_tr.y     = player.sprite_tr.y - SPRITE_HEIGHT;
        shield->sprite_tr.flags = SPRITE_FLAGS;
        err                     = gfx_sprite_render(&vctx, ++index, &shield->sprite_tr);
        if (err != GFX_SUCCESS)
            return err;

        // shield->sprite_r1.tile  = 23;
        // shield->sprite_r1.x     = player.sprite_tr.x + SPRITE_WIDTH;
        // shield->sprite_r1.y     = player.sprite_tr.y;
        shield->sprite_r1.flags = SPRITE_FLAGS;
        err                     = gfx_sprite_render(&vctx, ++index, &shield->sprite_r1);
        if (err != GFX_SUCCESS)
            return err;

        // shield->sprite_r2.tile  = 23;
        // shield->sprite_r2.x     = player.sprite_br.x + SPRITE_WIDTH;
        // shield->sprite_r2.y     = player.sprite_tr.y;
        shield->sprite_r2.flags = SPRITE_FLAGS | SPRITE_FLIP_Y;
        err                     = gfx_sprite_render(&vctx, ++index, &shield->sprite_r2);
        if (err != GFX_SUCCESS)
            return err;

        // shield->sprite_br.tile  = 22;
        // shield->sprite_br.x     = player.sprite_br.x + SPRITE_WIDTH;
        // shield->sprite_br.y     = player.sprite_br.y + SPRITE_HEIGHT;
        shield->sprite_br.flags = SPRITE_FLAGS | SPRITE_FLIP_Y;
        err                     = gfx_sprite_render(&vctx, ++index, &shield->sprite_br);
        if (err != GFX_SUCCESS)
            return err;

        // shield->sprite_b.tile  = 21;
        // shield->sprite_b.x     = player.sprite_tr.x;
        // shield->sprite_b.y     = player.sprite_tr.y + SPRITE_HEIGHT;
        shield->sprite_b.flags = SPRITE_FLAGS | SPRITE_FLIP_Y;
        err                    = gfx_sprite_render(&vctx, ++index, &shield->sprite_b);
        if (err != GFX_SUCCESS)
            return err;

        // shield->sprite_bl.tile  = 20;
        // shield->sprite_bl.x     = player.sprite_bl.x;
        // shield->sprite_bl.y     = player.sprite_bl.y - SPRITE_HEIGHT;
        shield->sprite_bl.flags = SPRITE_FLAGS | SPRITE_FLIP_Y;
        err                     = gfx_sprite_render(&vctx, ++index, &shield->sprite_bl);
        if (err != GFX_SUCCESS)
            return err;
    }

    player.rect.x = player.sprite_tl.x;
    player.rect.y = player.sprite_tl.y;
    player.rect.w = SPRITE_WIDTH * 2;
    player.rect.h = SPRITE_HEIGHT * 2;

    player_spawn();

    return err;
}

void player_spawn(void) {
    player.health = PLAYER_MAX_HEALTH;
    player_shield(1);

    player.sprite_tl.tile  = PLAYER_TL;
    player.sprite_tr.tile  = PLAYER_TR;
    player.sprite_bl.tile  = PLAYER_TL;
    player.sprite_br.tile  = PLAYER_TR;

    player.sprite_tl.x     = 64;
    player.sprite_tl.y     = 64;
    // player.sprite_tr.x     = player.sprite_tr.x + SPRITE_WIDTH;
    // player.sprite_tr.y     = player.sprite_tr.y;
    // player.sprite_bl.x     = player.sprite_tl.x;
    // player.sprite_bl.y     = player.sprite_tl.y + SPRITE_HEIGHT;
    // player.sprite_br.x     = player.sprite_tr.x;
    // player.sprite_br.y     = player.sprite_br.y + SPRITE_HEIGHT;
}

error player_deinit(void)
{
    gfx_error err;
    uint8_t index = player.sprite_index;
    err           = gfx_sprite_set_tile(&vctx, index, EMPTY_TILE);
    err           = gfx_sprite_set_tile(&vctx, ++index, EMPTY_TILE);
    err           = gfx_sprite_set_tile(&vctx, ++index, EMPTY_TILE);
    err           = gfx_sprite_set_tile(&vctx, ++index, EMPTY_TILE);
    return err;
}

void player_shoot(void)
{
    if(player.bullet_lock > 0) return;
    player.bullet++;
    if (player.bullet >= PLAYER_MAX_BULLETS)
        player.bullet = 0;
    bullet_t* bullet = &BULLETS[player.bullet];
    if (bullet->active)
        return; // this bullet is still in use

    bullet->active      = 1;
    bullet->sprite.tile = BULLET_GREEN;
    bullet->direction.x = DIRECTION_RIGHT;
    bullet->sprite.x    = player.sprite_br.x;
    bullet->sprite.y    = player.sprite_br.y + 2;
    player.bullet_lock = PLAYER_BULLET_FREQ;

    sound_play(BULLET_SOUND, 220, 2);
}

void player_damaged(uint8_t damage) {
    if(player.health <= damage) {
        player.health = 0;
        player.sprite_tl.tile = PLAYER_TL;
        player.sprite_tr.tile = PLAYER_TR;
        player.sprite_bl.tile = PLAYER_TL;
        player.sprite_br.tile = PLAYER_TR;
        player.shield.active = 1;
        return;
    }

    player.health -= damage;
    if(player.health < 6) {
        player_shield(0);
    }
    if(player.health < 5) {
        player.sprite_tl.tile = PLAYER_TL_DMG;
    }
    if(player.health < 4) {
        player.sprite_br.tile = PLAYER_BR_DMG;
    }
    if(player.health < 3) {
        player.sprite_bl.tile = PLAYER_BL_DMG;
    }
    if(player.health < 2) {
        player.sprite_tr.tile = PLAYER_TR_DMG;
    }
}

uint8_t player_destroyed(void) {
    player.health = PLAYER_MAX_HEALTH;
    player.lives--;

    player_draw_lives(0, HEIGHT-1);

    return player.lives;
}

void player_update(void)
{
    if(player.bullet_lock > 0) player.bullet_lock--;
}

void player_move(void)
{
    int16_t x  = player.sprite_tl.x;
    int16_t y  = player.sprite_tl.y;
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
        player.sprite_tl.x = x;
        player.sprite_tl.y = y;

        player.sprite_tr.x = x + SPRITE_WIDTH;
        player.sprite_tr.y = y;

        player.sprite_bl.x = x;
        player.sprite_bl.y = y + SPRITE_HEIGHT;

        player.sprite_br.x = x + SPRITE_WIDTH;
        player.sprite_br.y = y + SPRITE_HEIGHT;
    }

    // shield
    {
        player.shield.sprite_tl.x = player.sprite_tl.x;
        player.shield.sprite_tl.y = player.sprite_tl.y - SPRITE_HEIGHT;

        player.shield.sprite_t.x = player.sprite_tr.x;
        player.shield.sprite_t.y = player.sprite_tr.y - SPRITE_HEIGHT;

        player.shield.sprite_tr.x = player.sprite_tr.x + SPRITE_HEIGHT;
        player.shield.sprite_tr.y = player.sprite_tr.y - SPRITE_WIDTH;

        player.shield.sprite_r1.x = player.sprite_tr.x + SPRITE_WIDTH;
        player.shield.sprite_r1.y = player.sprite_tr.y;

        player.shield.sprite_r2.x = player.sprite_br.x + SPRITE_WIDTH;
        player.shield.sprite_r2.y = player.sprite_br.y;

        player.shield.sprite_br.x = player.sprite_br.x + SPRITE_WIDTH;
        player.shield.sprite_br.y = player.sprite_br.y + SPRITE_HEIGHT;

        player.shield.sprite_b.x = player.sprite_br.x;
        player.shield.sprite_b.y = player.sprite_br.y + SPRITE_HEIGHT;

        player.shield.sprite_bl.x = player.sprite_bl.x;
        player.shield.sprite_bl.y = player.sprite_bl.y + SPRITE_HEIGHT;
    }

    player.rect.x = player.sprite_tl.x;
    player.rect.y = player.sprite_tl.y;
}

void player_draw(void)
{
    // ignore errors
    uint8_t index = player.sprite_index;
    gfx_error err;
    // ship
    err = gfx_sprite_render_array(&vctx, index, &player.sprite_tl, 4);

    // shield
    err = gfx_sprite_render_array(&vctx, index+4, &player.shield.sprite_tl, 8);
}

void player_draw_lives(uint8_t x, uint8_t y) {
    uint8_t lives[3];
    for(uint8_t l = 1; l < 4; l++) {
        if(player.lives >= l) {
            // gfx_tilemap_place(&vctx, 15, 1, l-1, 14);
            lives[l-1] = PLAYER_LIVES_TILE;
        } else {
            lives[l-1] = EMPTY_TILE;
        }
    }
    gfx_tilemap_load(&vctx, lives, 3, 1, x, y);
}