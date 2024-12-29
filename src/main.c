#include <stdio.h>
#include <stdint.h>
#include <zos_errors.h>
#include <zos_vfs.h>
#include <zos_sys.h>
#include <zos_video.h>
#include <zgdk.h>

#include "assets.h"
#include "player.h"
#include "bullet.h"
#include "enemy.h"
#include "main.h"

gfx_context vctx;
uint8_t controller_mode = 1;
uint16_t input1_prev    = 0;

int main(void)
{
    init();
reset:
    reset();
    load_level(0);
    while (true) {
        uint8_t action = input();
        if (action == ACTION_QUIT)
            goto quit_game;
        update();
        draw();
    }
quit_game:
    deinit();
    return 0;
}

void handle_error(zos_err_t err, const char* message, uint8_t fatal)
{
    if (err != ERR_SUCCESS) {
        if (fatal)
            deinit();
        printf("\nError[%d] (%02x) %s", err, err, message);
        if (fatal)
            exit(err);
    }
}

void init(void)
{
    zos_err_t err;

    err = keyboard_init();
    handle_error(err, "Failed to init keyboard: %d", 1);
    err = keyboard_flush();
    handle_error(err, "Failed to flush keyboard: %d", 1);

    err = controller_init();
    handle_error(err, "Failed to init controller: %d", 1);
    err = controller_flush();
    handle_error(err, "Failed to flush controller: %d", 1);
    uint16_t test = controller_read();
    // if unconnected, we'll get back 0xFFFF (all buttons pressed)
    if (test & 0xFFFF) {
        controller_mode = 0;
    }

    // disable the screen to prevent artifacts from showing
    gfx_enable_screen(0);

    err = gfx_initialize(SCREEN_MODE, &vctx);
    handle_error(err, "failed to init graphics", 1);

    err = load_palette(&vctx);
    handle_error(err, "failed to load palette", 1);

    gfx_tileset_options options = {
        .compression = TILESET_COMP_RLE,
    };
    err = load_tileset(&vctx, &options);
    handle_error(err, "failed to load tileset", 1);

    err = player_init();
    handle_error(err, "failed to init player", 1);

    err = bullet_init();
    handle_error(err, "failed to init bullets", 1);

    err = enemies_init();
    handle_error(err, "failed to init enemies", 1);

    gfx_enable_screen(1);
}

void deinit(void)
{
    // reset screen
    ioctl(DEV_STDOUT, CMD_RESET_SCREEN, NULL);

    error err;
    err = player_deinit();
    handle_error(err, "failed to deinit player", 1);
    err = bullet_deinit();
    handle_error(err, "failed to deinit bullets", 1);

    err = enemies_deinit();
    handle_error(err, "failed to deinit enemies", 1);
}

void reset(void)
{
    // reset your game state
}

uint8_t input(void)
{
    uint16_t input1 = keyboard_read();
    if (controller_mode == 1) {
        input1 |= controller_read();
    }

    player.direction.x = DIRECTION_NONE;
    player.direction.y = DIRECTION_NONE;
    if (LEFT1)
        player.direction.x = DIRECTION_LEFT;
    if (RIGHT1)
        player.direction.x = DIRECTION_RIGHT;
    if (UP1)
        player.direction.y = DIRECTION_UP;
    if (DOWN1)
        player.direction.y = DIRECTION_DOWN;
    if (BUTTON1_B)
        player_shoot();

    if (SELECT1)
        return ACTION_QUIT;

    input1_prev = input1;
    return 0;
}

error load_level(uint8_t which)
{
    (void*) which; // unreferenced
    uint8_t y, x;
    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            gfx_tilemap_place(&vctx, EMPTY_TILE, 0, x, y);
        }
    }
    return 0;
}

void update(void)
{
    player_move();
    bullet_move();
    enemies_move();

    player_update();

    uint8_t i, j;
    uint8_t remaining = MAX_ENEMIES;
    for (i = 0; i < PLAYER_MAX_BULLETS; i++) {
        if (remaining < 1)
            goto next_spawn;
        bullet_t* bullet = &BULLETS[i];
        if (bullet->active == 0)
            continue;

        for (j = 0; j < MAX_ENEMIES; j++) {
            enemy_t* enemy = &ENEMIES[j];
            if (enemy->active == 0) {
                remaining--;
                continue;
            }
            if (bullet->sprite.x + SPRITE_WIDTH >= enemy->sprite_t.x) {
                if ((bullet->sprite.y >= (enemy->sprite_t.y)) &&
                    (bullet->sprite.y <= (enemy->sprite_b.y + SPRITE_HEIGHT))) {
                    bullet->active   = 0;
                    bullet->sprite.x = SCREEN_WIDTH + SPRITE_WIDTH;

                    enemy->active     = 0;
                    enemy->sprite_t.x = SCREEN_WIDTH + SPRITE_WIDTH;
                    enemy->sprite_b.x = SCREEN_WIDTH + SPRITE_WIDTH;
                    remaining--;
                    goto next_bullet;
                }
            }
        }
next_bullet:
    }

next_spawn:

    if (remaining == 0) {
        // 16..224 ((240 - 32) + 16)
        uint8_t y = (rand8() % 128) + 56;
        for (i = 0; i < MAX_ENEMIES; i++) {
            enemy_t* enemy    = &ENEMIES[i];
            enemy->sprite_t.y = y;
            enemy->sprite_b.y = y + SPRITE_HEIGHT;
            enemy->sprite_t.x = SCREEN_WIDTH + (SPRITE_WIDTH * i);
            enemy->sprite_b.x = SCREEN_WIDTH + (SPRITE_WIDTH * i);
            enemy->active     = 1;
        }
    }
}

void draw_paused(uint8_t paused)
{
    (void*) paused; // unreferenced
}

void draw_gameover(uint8_t gameover)
{
    (void*) gameover; // unreferenced
}

void draw(void)
{
    gfx_wait_vblank(&vctx);
    bullet_draw();
    player_draw();
    enemies_draw();
    gfx_wait_end_vblank(&vctx);
}