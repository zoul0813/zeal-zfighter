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
#include "belt.h"
#include "main.h"
#include "hiscore.h"
#include "attract.h"

static uint8_t controller_mode = 1;
static uint16_t input1_prev    = 0;
static int8_t star_field_dir    = 1;
static Vector2_u16 star_field_pos = { .x = 0, .y = 128 };
static uint8_t wave_counter = 0;
static uint8_t asteroid_wave = 5;
static WaveType wave_type = ENEMY_WAVE;
static uint8_t paused = 0;
static uint8_t game_mode = GAME_ATTRACT;

static pattern_t pattern0;
static pattern_t pattern1;
static pattern_t pattern2;
static pattern_t pattern3;

// extern
gfx_context vctx;
uint8_t frames          = 0;
char buffer[32];
track_t track = {
    .title = "Shooter",
    .patterns = {
        &pattern0,
        &pattern1,
        &pattern2,
        &pattern3,
    }
};
const size_t TRACK_SIZE = sizeof(track);

int main(void)
{
    init();
reset:
    load_level(0);

    attract_init();
    reset();

    // while (true) {
game_loop:
    sound_loop();
    if(game_mode != GAME_PAUSED)
        zmt_tick(&track, 1);

    frames++;
    uint8_t action = input();

    switch(game_mode) {
        case GAME_ATTRACT:
            if(action == ACTION_CONTINUE) {
                attract_deinit();
                game_mode = GAME_PLAY;
                goto game_loop;
            }
            attract_mode();
            action = ACTION_NONE;
            break;
        case GAME_OVER:
            if(action == ACTION_CONTINUE) {
                draw_gameover(false);
                reset();
                game_mode = GAME_PLAY;
                goto game_loop;
            }
            action = ACTION_NONE;
            break;
        // case GAME_PAUSED:
        //     goto game_loop;
        // case GAME_PLAY:
        //     break;
    }

    switch(action) {
        case ACTION_NONE:
            if(paused == 1) {
                paused = 2; // released pause
                game_mode == GAME_PAUSED;
                zmt_sound_off();
                zmt_reset(VOL_0);
                sound_stop_all();
                draw_paused(true);
            }
            if(paused == 3) {
                paused = 0; // released unpause
                game_mode == GAME_PLAY;
                zmt_reset(VOL_50);
                draw_paused(false);
            }
            break;
        case ACTION_PAUSE:
            if(paused == 0) {
                paused = 1; // requested pause
                goto game_loop;
            }
            if(paused == 2) {
                paused = 3; // requested unpause
            }
            break;
        case ACTION_QUIT:
            goto quit_game;
    }

    if(paused > 0)
        goto game_loop;

    update();
    draw();
    goto game_loop;
    // }
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

    err = input_init(true);

    hiscore_init();

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

    ascii_map(' ', 1, EMPTY_TILE);
    ascii_map('0', 10, 48); // 0-9
    ascii_map('A', 13, 64); // A-M
    ascii_map('a', 13, 64); // A-M
    ascii_map('N', 13, 80); // N-Z
    ascii_map('n', 13, 80); // N-Z

    tilemap_fill(&vctx, EMPTY_TILE, 0, 0, 0, WIDTH, HEIGHT);
    tilemap_fill(&vctx, EMPTY_TILE, 1, 0, 0, WIDTH, HEIGHT);

    sound_init();
    Sound *bullet = sound_get(BULLET_SOUND);
    bullet->waveform = WAV_SAWTOOTH;

    zmt_reset(VOL_50);

    gfx_enable_screen(1);
}

void deinit(void)
{
    tilemap_scroll(0, 0, 0);
    tilemap_scroll(1, 0, 0);
    sound_deinit();

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
    wave_counter = 0;
    asteroid_wave = 5;
    wave_type = ENEMY_WAVE;

    // reset your game state
    gfx_wait_vblank(&vctx);

    uint16_t color = BG_NORMAL;
    gfx_palette_load(&vctx, &color, sizeof(uint16_t), 0);

    belt_destroy();
    for(uint8_t i = 0; i < MAX_ENEMIES; i++) {
        enemy_destroy(&ENEMIES[i]);
    }
    for(uint8_t i = 0; i < MAX_BULLETS; i++) {
        BULLETS[i].active   = 0;
        BULLETS[i].sprite.x = SCREEN_WIDTH + SPRITE_WIDTH;
    }
    gfx_wait_end_vblank(&vctx);

    msleep(100);

    player_spawn();
    player.lives = PLAYER_MAX_LIVES;
    player_score(0);

    player_draw_lives(0, HEIGHT-1);

    input_flush();

    msleep(500);
}

uint8_t input(void)
{
    uint16_t input1 = input_get();

    if(START1)
        return ACTION_PAUSE; // ACTION_CONTINUE
    if (SELECT1)
        return ACTION_QUIT;

    if(game_mode == GAME_PLAY) {
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
    }

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

    player_score(0);
    player_draw_lives(0, HEIGHT-1);

    load_zmt(&track, 0);
    return 0;
}

void destroy_player(void) {
    uint16_t color;
    player_destroyed();
    color = BG_WARN;
    gfx_palette_load(&vctx, &color, sizeof(uint16_t), 0);

    msleep(500);

    if(player.lives == 0) {
        color = BG_ERROR;
        gfx_palette_load(&vctx, &color, sizeof(uint16_t), 0);
        sound_stop_all();
        if(game_mode == GAME_PLAY) {
            gfx_wait_vblank(&vctx);
            gfx_wait_end_vblank(&vctx);
            hiscore_add(player.score);
        }
        msleep(500);
        player.lives = PLAYER_MAX_LIVES;
        player.direction.x = DIRECTION_NONE;
        player.direction.y = DIRECTION_NONE;
        player_score(0);
        game_mode = GAME_OVER;
        draw_gameover(true);
        return;
    }

    color = BG_NORMAL;
    gfx_palette_load(&vctx, &color, sizeof(uint16_t), 0);

    wave_counter = 0;
    wave_type = ENEMY_WAVE;
    player_spawn();
}

void update(void)
{
    uint8_t i, j;

    player_move();
    bullet_move();
    enemies_move();

    player_update();
    switch(wave_type) {
        case BELT_SHOWER:
            if(belt_collide(&player.rect)) {
                belt_destroy();
                destroy_player();
            }
            if(!belt_update()) {
                wave_counter = 0;
                wave_type = ENEMY_WAVE;
            }
            break;
        case ENEMY_WAVE:
            enemies_update();
            break;
    }

    if (!enemies_active()) goto next_spawn;
    for (i = 0; i < MAX_BULLETS; i++) {
        bullet_t* bullet = &BULLETS[i];
        if (bullet->active == 0)
            continue;

        if(bullet->direction.x == DIRECTION_RIGHT) {
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
                        player_score(5);
                        goto next_bullet;
                    }
                }
            }
        } else {
                if (
                    (bullet->sprite.x + SPRITE_WIDTH >= player.sprite_tl.x)
                    &&
                    (bullet->sprite.x + SPRITE_WIDTH <= player.sprite_tr.x + SPRITE_WIDTH)
                ) {
                    if (
                        (bullet->sprite.y >= (player.sprite_tl.y))
                        &&
                        (bullet->sprite.y <= (player.sprite_bl.y + SPRITE_HEIGHT))
                    ) {
                        bullet->active   = 0;
                        bullet->sprite.x = SCREEN_WIDTH + SPRITE_WIDTH;

                        player_damaged(1);
                        if(player.health == 0) {
                            sound_stop_all();
                            msleep(500);
                            for(uint8_t e = 0; e < MAX_ENEMIES; e++) {
                                enemy_destroy(&ENEMIES[e]);
                            }

                            destroy_player();
                        }

                        goto next_bullet;
                    }
                }
        }
next_bullet:
    }

    if (!enemies_active()) {
next_spawn:
        if(wave_counter >= asteroid_wave) {
            wave_type = BELT_SHOWER;
            wave_counter = belt_spawn();
            asteroid_wave = rand8_quick() % 10 + 2;
        }
        if(wave_type == ENEMY_WAVE) {
            wave_counter += enemies_spawn((rand8() % 64) + 88);
        }
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
    if (paused)
        sprintf(buffer, "PAUSED");
    else
        sprintf(buffer, "      ");
    nprint_string(&vctx, buffer, 6, WIDTH / 2 - 3, HEIGHT / 2);
}

void draw_gameover(uint8_t gameover)
{
    if (gameover)
        sprintf(buffer, "GAME  OVER");
    else
        sprintf(buffer, "          ");
    nprint_string(&vctx, buffer, 10, WIDTH / 2 - 5, HEIGHT / 2);
}

void draw(void)
{
    gfx_wait_vblank(&vctx);
    bullet_draw();
    player_draw();
    belt_draw();
    enemies_draw();

    // scroll tilemap
    tilemap_scroll(0, star_field_pos.x, star_field_pos.y + (player.sprite_tl.y >> 2));

    // char buffer[16];
    // sprintf(buffer, "%03d", wave_counter);
    // nprint_string(&vctx, buffer, 3, 16, 0);
    // sprintf(buffer, "%03d", wave_type);
    // nprint_string(&vctx, buffer, 3, 16, 1);

    // uint8_t value;
    // DEBUG
    // sprintf(buffer, "%05d", star_field_pos.y);
    // nprint_string(&vctx, buffer, 5, 14, 0);
    // sprintf(buffer, "%05d", player.sprite_tl.y);
    // nprint_string(&vctx, buffer, 5, 14, 1);

    // DEBUG
    sprintf(buffer, "%02d", game_mode);
    nprint_string(&vctx, buffer, 2, 18, 14);

    gfx_wait_end_vblank(&vctx);
}