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
uint16_t input1_prev = 0;

int main(void) {
    init();
reset:
    reset();
    load_level(0);
    while(true) {
        uint8_t action = input();
        if(action == ACTION_QUIT) goto quit_game;
        update();
        draw();
    }
quit_game:
    deinit();
    return 0;
}

void handle_error(zos_err_t err, const char* message, uint8_t fatal) {
    if(err != ERR_SUCCESS) {
        if(fatal) deinit();
        printf("\nError[%d] (%02x) %s", err, err, message);
        if(fatal) exit(err);
    }
}

void init(void) {
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

    err = enemy_init();
    handle_error(err, "failed to init enemies", 1);

    gfx_enable_screen(1);
}

void deinit(void) {
    // reset screen
    ioctl(DEV_STDOUT, CMD_RESET_SCREEN, NULL);

    error err;
    err = player_deinit();
    handle_error(err, "failed to deinit player", 1);
    err = bullet_deinit();
    handle_error(err, "failed to deinit bullets", 1);

    err = enemy_deinit();
    handle_error(err, "failed to deinit enemies", 1);
}

void reset(void) {
    // reset your game state
}

uint8_t input(void) {
    uint16_t input1 = keyboard_read();
    if (controller_mode == 1) {
        input1 |= controller_read();
    }

    player.direction.x = DIRECTION_NONE;
    player.direction.y = DIRECTION_NONE;
    if(LEFT1) player.direction.x = DIRECTION_LEFT;
    if(RIGHT1) player.direction.x = DIRECTION_RIGHT;
    if(UP1) player.direction.y = DIRECTION_UP;
    if(DOWN1) player.direction.y = DIRECTION_DOWN;
    if(BUTTON1_B) {
        BULLETS[0].active = 1;
        BULLETS[0].direction.x = DIRECTION_RIGHT;
        BULLETS[0].sprite.x = player.sprite_tr.x ;
        BULLETS[0].sprite.y = player.sprite_tr.y + (SPRITE_HEIGHT / 2);
    }

    if(SELECT1) return ACTION_QUIT;

    input1_prev = input1;
    return 0;
}

error load_level(uint8_t which) {
    (void *)which; // unreferenced
    uint8_t y,x;
    for(y = 0; y < HEIGHT; y++) {
        for(x = 0; x < WIDTH; x++) {
            gfx_tilemap_place(&vctx, EMPTY_TILE, 0, x, y);
        }
    }
    return 0;
}

void update(void) {
    player_move();
    bullet_move();
}

void draw_paused(uint8_t paused) {
    (void*) paused; // unreferenced
}

void draw_gameover(uint8_t gameover) {
    (void*) gameover; // unreferenced
}

void draw(void) {
    gfx_wait_vblank(&vctx);
    bullet_draw();
    player_draw();
    gfx_wait_end_vblank(&vctx);
}