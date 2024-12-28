#include <zgdk.h>
#include "player.h"
#include "main.h"

player_t player;

error player_init(void)
{
    gfx_error err;
    uint8_t index       = 0;
    player.sprite_index = index;

    // ship
    {
        player.sprite_tl.tile  = 0;
        player.sprite_tl.x     = SCREEN_WIDTH / 2;
        player.sprite_tl.y     = SCREEN_HEIGHT / 2;
        player.sprite_tl.flags = SPRITE_NONE;
        err                    = gfx_sprite_render(&vctx, index, &player.sprite_tl);
        if (err != GFX_SUCCESS)
            return err;

        player.sprite_tr.tile  = 1;
        player.sprite_tr.x     = player.sprite_tr.x + SPRITE_WIDTH;
        player.sprite_tr.y     = player.sprite_tr.y;
        player.sprite_tr.flags = SPRITE_NONE;
        err                    = gfx_sprite_render(&vctx, ++index, &player.sprite_tr);
        if (err != GFX_SUCCESS)
            return err;

        player.sprite_bl.tile  = 16;
        player.sprite_bl.x     = player.sprite_tl.x;
        player.sprite_bl.y     = player.sprite_tl.y + SPRITE_HEIGHT;
        player.sprite_bl.flags = SPRITE_NONE;
        err                    = gfx_sprite_render(&vctx, ++index, &player.sprite_bl);
        if (err != GFX_SUCCESS)
            return err;

        player.sprite_br.tile  = 17;
        player.sprite_br.x     = player.sprite_tr.x;
        player.sprite_br.y     = player.sprite_br.y + SPRITE_HEIGHT;
        player.sprite_br.flags = SPRITE_NONE;
        err                    = gfx_sprite_render(&vctx, ++index, &player.sprite_br);
        if (err != GFX_SUCCESS)
            return err;
    }

    // shield
    {
        shield_t* shield = &player.shield;
        index++;
        shield->sprite_index = index;

        shield->sprite_tl.tile  = 32;
        shield->sprite_tl.x     = player.sprite_tl.x;
        shield->sprite_tl.y     = player.sprite_tl.y - SPRITE_HEIGHT;
        shield->sprite_tl.flags = SPRITE_NONE;
        err                     = gfx_sprite_render(&vctx, index, &shield->sprite_tl);
        if (err != GFX_SUCCESS)
            return err;

        shield->sprite_t.tile  = 33;
        shield->sprite_t.x     = player.sprite_tr.x;
        shield->sprite_t.y     = player.sprite_tr.y - SPRITE_HEIGHT;
        shield->sprite_t.flags = SPRITE_NONE;
        err                    = gfx_sprite_render(&vctx, ++index, &shield->sprite_t);
        if (err != GFX_SUCCESS)
            return err;

        shield->sprite_tr.tile  = 34;
        shield->sprite_tr.x     = player.sprite_tr.x + SPRITE_WIDTH;
        shield->sprite_tr.y     = player.sprite_tr.y - SPRITE_HEIGHT;
        shield->sprite_tr.flags = SPRITE_NONE;
        err                     = gfx_sprite_render(&vctx, ++index, &shield->sprite_tr);
        if (err != GFX_SUCCESS)
            return err;

        shield->sprite_r1.tile  = 35;
        shield->sprite_r1.x     = player.sprite_tr.x + SPRITE_WIDTH;
        shield->sprite_r1.y     = player.sprite_tr.y;
        shield->sprite_r1.flags = SPRITE_NONE;
        err                     = gfx_sprite_render(&vctx, ++index, &shield->sprite_r1);
        if (err != GFX_SUCCESS)
            return err;

        shield->sprite_r2.tile  = 36;
        shield->sprite_r2.x     = player.sprite_br.x + SPRITE_WIDTH;
        shield->sprite_r2.y     = player.sprite_tr.y;
        shield->sprite_r2.flags = SPRITE_NONE;
        err                     = gfx_sprite_render(&vctx, ++index, &shield->sprite_r2);
        if (err != GFX_SUCCESS)
            return err;

        shield->sprite_br.tile  = 37;
        shield->sprite_br.x     = player.sprite_br.x + SPRITE_WIDTH;
        shield->sprite_br.y     = player.sprite_br.y + SPRITE_HEIGHT;
        shield->sprite_br.flags = SPRITE_NONE;
        err                     = gfx_sprite_render(&vctx, ++index, &shield->sprite_br);
        if (err != GFX_SUCCESS)
            return err;

        shield->sprite_b.tile  = 38;
        shield->sprite_b.x     = player.sprite_tr.x;
        shield->sprite_b.y     = player.sprite_tr.y + SPRITE_HEIGHT;
        shield->sprite_b.flags = SPRITE_NONE;
        err                    = gfx_sprite_render(&vctx, ++index, &shield->sprite_b);
        if (err != GFX_SUCCESS)
            return err;

        shield->sprite_bl.tile  = 39;
        shield->sprite_bl.x     = player.sprite_bl.x;
        shield->sprite_bl.y     = player.sprite_bl.y - SPRITE_HEIGHT;
        shield->sprite_bl.flags = SPRITE_NONE;
        err                     = gfx_sprite_render(&vctx, ++index, &shield->sprite_bl);
        if (err != GFX_SUCCESS)
            return err;
    }


    return err;
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

void player_move(void)
{
    uint16_t x  = player.sprite_tl.x;
    uint16_t y  = player.sprite_tl.y;
    x          += (player.direction.x * PLAYER_SPEED);
    y          += (player.direction.y * PLAYER_SPEED);

    // switch (player.direction.x) {
    //     case DIRECTION_LEFT: {
    //         player.sprite_tl.flags = SPRITE_FLIP_X;
    //         player.sprite_tr.flags = SPRITE_FLIP_X;
    //     } break;
    //     case DIRECTION_RIGHT: {
    //         player.sprite_tl.flags = SPRITE_NONE;
    //         player.sprite_tl.flags = SPRITE_NONE;
    //     } break;
    // }

    if (x < SPRITE_WIDTH)
        x = SPRITE_WIDTH;
    else if (x > SCREEN_WIDTH)
        x = SCREEN_WIDTH;
    if (y < SPRITE_HEIGHT)
        y = SPRITE_HEIGHT;
    else if (y > SCREEN_HEIGHT + SPRITE_HEIGHT)
        y = SCREEN_HEIGHT + SPRITE_HEIGHT;

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
}

void player_draw(void)
{
    // ignore errors
    uint8_t index = player.sprite_index;
    gfx_error err;
    // ship
    {
        err = gfx_sprite_render(&vctx, index, &player.sprite_tl);
        err = gfx_sprite_render(&vctx, ++index, &player.sprite_tr);
        err = gfx_sprite_render(&vctx, ++index, &player.sprite_bl);
        err = gfx_sprite_render(&vctx, ++index, &player.sprite_br);
    }

    // shield
    {
        err = gfx_sprite_render(&vctx, ++index, &player.shield.sprite_tl);
        err = gfx_sprite_render(&vctx, ++index, &player.shield.sprite_t);
        err = gfx_sprite_render(&vctx, ++index, &player.shield.sprite_tr);
        err = gfx_sprite_render(&vctx, ++index, &player.shield.sprite_r1);
        err = gfx_sprite_render(&vctx, ++index, &player.shield.sprite_r2);
        err = gfx_sprite_render(&vctx, ++index, &player.shield.sprite_br);
        err = gfx_sprite_render(&vctx, ++index, &player.shield.sprite_b);
        err = gfx_sprite_render(&vctx, ++index, &player.shield.sprite_bl);
    }
}