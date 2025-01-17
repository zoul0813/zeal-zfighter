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
uint8_t frames          = 0;
int8_t star_field_dir    = 1;
Vector2_u16 star_field_pos = { .x = 0, .y = 128 };
static char text[32];

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
        frames++;
        // TSTATE_LOG(1);
        update();
        // TSTATE_LOG(1);
        // TSTATE_LOG(2);
        draw();
        // TSTATE_LOG(2);
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

    ascii_map('0', 10, 48); // 0-9
    ascii_map('A', 13, 64);
    ascii_map('a', 13, 64);
    ascii_map('N', 13, 80);
    ascii_map('n', 13, 80);

    sprintf(text, "%05d", player.score);
    nprint_string(&vctx, text, 5, 0, 0);

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

/**
 * generate new starfields for screens 1 and 4
 */
void generate_starfield1(void) {
    uint8_t y, x, r, t;
    // screen 1 and 4 are always the same?
    for (y = 0; y < 15; y++) {
        for (x = 0; x < 20; x++) {
            r = rand8();
            r = r > 250;
            t = rand8() % STAR_TILE_NUM;
            // screen 1
            gfx_tilemap_place(&vctx, r ? STAR_TILE + t : EMPTY_TILE, 0, x, y);
            gfx_tilemap_place(&vctx, r ? STAR_TILE + t : EMPTY_TILE, 0, x, 15 + y);
            // screen 4
            gfx_tilemap_place(&vctx, r ? STAR_TILE + t : EMPTY_TILE, 0, 60 + x, y);
            gfx_tilemap_place(&vctx, r ? STAR_TILE + t : EMPTY_TILE, 0, 60 + x, 15 + y);
        }
    }
}

/**
 * generate new starfields for screens 2 and 3
 */
void generate_starfield2(void) {
    uint8_t y, x, r, t;
    for (y = 0; y < 15; y++) {
        for (x = 20; x < 60; x++) {
            r = rand8();
            r = r > 250;
            t = rand8() % STAR_TILE_NUM;
            gfx_tilemap_place(&vctx, r ? STAR_TILE + t : EMPTY_TILE, 0, x, y);
            gfx_tilemap_place(&vctx, r ? STAR_TILE + t : EMPTY_TILE, 0, x, 15 + y);
        }
    }
}

error load_level(uint8_t which)
{
    (void*) which; // unreferenced
    generate_starfield1();
    generate_starfield2();
    return 0;
}

void update(void)
{
    uint8_t i, j;

    player_move();
    bullet_move();
    enemies_move();

    player_update();

    if (!enemies_active()) goto next_spawn;
    for (i = 0; i < PLAYER_MAX_BULLETS; i++) {
        bullet_t* bullet = &BULLETS[i];
        if (bullet->active == 0)
            continue;

        for (j = 0; j < MAX_ENEMIES; j++) {
            enemy_t* enemy = &ENEMIES[j];
            if (enemy->active == 0) {
                continue;
            }
            if (
                (bullet->sprite.x + SPRITE_WIDTH >= enemy->sprite_t.x)
                &&
                (bullet->sprite.x + SPRITE_WIDTH <= enemy->sprite_t.x + SPRITE_WIDTH)
            ) {
                if (
                    (bullet->sprite.y >= (enemy->sprite_t.y))
                    &&
                    (bullet->sprite.y <= (enemy->sprite_b.y + SPRITE_HEIGHT))
                ) {
                    bullet->active   = 0;
                    bullet->sprite.x = SCREEN_WIDTH + SPRITE_WIDTH;

                    enemy_destroy(enemy);
                    player.score++;
                    sprintf(text, "%05d", player.score);
                    nprint_string(&vctx, text, 5, 0, 0);
                    goto next_bullet;
                }
            }
        }
next_bullet:
    }

    if (!enemies_active()) {
next_spawn:
        enemies_spawn((rand8() % 64) + 88);
    }

    star_field_pos.x++;
    if(star_field_pos.x == 960) {
        // generate new fields for panels 2 and 3
        star_field_pos.x = 0;
        generate_starfield2();
    }
    if(star_field_pos.x == 480) {
        // generate new field for panels 1 and 4
        generate_starfield1();
    }

    if(frames % 32 == 0) {
        star_field_pos.y += star_field_dir;
        if(star_field_pos.y == 239 || star_field_pos.y == 0) {
            star_field_dir *= -1;
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

    // scroll tilemap
    zvb_ctrl_l0_scr_x_low = star_field_pos.x & 0xFF;
    zvb_ctrl_l0_scr_x_high = star_field_pos.x >> 8;
    // zvb_ctrl_l0_scr_x_high = 0;
    zvb_ctrl_l0_scr_y_low = star_field_pos.y & 0xFF;
    // zvb_ctrl_l0_scr_y_high = star_field_pos.y >> 8;
    zvb_ctrl_l0_scr_y_high = 0;

    // char text[16];
    // sprintf(text, "%05d", player.score);
    // nprint_string(&vctx, text, 5, 0, 0);

    // uint8_t value;
    // DEBUG
    // sprintf(text, "%05d", star_field_pos.x);
    // nprint_string(&vctx, text, 5, 14, 0);
    // sprintf(text, "%05d", star_field_pos.x >>  7);
    // nprint_string(&vctx, text, 5, 14, 1);

    // // DEBUG
    // value = enemies_active();
    // sprintf(text, "%02d", value);
    // nprint_string(&vctx, text, 2, 18, 14);
    gfx_wait_end_vblank(&vctx);
}