#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <zos_vfs.h>
#include <zgdk.h>
#include "main.h"
#include "hiscore.h"

#define OFFSET_X    ((WIDTH - 10) / 2)
#define OFFSET_Y    (3)

#define LEFT1     (input1 & BUTTON_LEFT)
#define RIGHT1    (input1 & BUTTON_RIGHT)
#define UP1       (input1 & BUTTON_UP)
#define DOWN1     (input1 & BUTTON_DOWN)
#define BUTTON1_B (input1 & BUTTON_B)
#define BUTTON1_A (input1 & BUTTON_A)
#define START1    (input1 & BUTTON_START)
#define SELECT1   (input1 & BUTTON_SELECT)

static highscore_t hiscores[10] = {
    { .initials = { 'D', 'P', 'H' }, .score = 4000 },
    { .initials = { 'Z', 'E', 'B' }, .score = 2500 },
    { .initials = { 'Z', 'E', 'B' }, .score = 2250 },
    { .initials = { 'Z', 'E', 'B' }, .score = 2000 },
    { .initials = { 'Z', 'E', 'B' }, .score = 1750 },
    { .initials = { 'Z', 'E', 'B' }, .score = 1500 },
    { .initials = { 'Z', 'E', 'B' }, .score = 1250 },
    { .initials = { 'Z', 'E', 'B' }, .score = 1000 },
    { .initials = { 'Z', 'E', 'B' }, .score = 750 },
    { .initials = { 'Z', 'E', 'B' }, .score = 500 },
};
static char last_initials[3] = { 'A', 'A', 'A' };
static char _buffer[16];

void hiscore_init(void) {
#ifdef HISCORE_PATH
    hiscore_load(HISCORE_PATH);
#endif
}

void hiscore_show(void) {
    tilemap_fill(&vctx, BLANK_TILE, 1, 0, 0, WIDTH, HEIGHT);
    uint8_t i;
    for(i = 0; i < HISCORES_COUNT; i++) {
        highscore_t *score = &hiscores[i];
        sprintf(_buffer, "%.3s  %05hu", score->initials, score->score);
        nprint_string(&vctx, _buffer, 10, OFFSET_X, OFFSET_Y + i);
    }
}

void hiscore_hide(void) {
    tilemap_fill(&vctx, LAYER1, EMPTY_TILE,0, 0, WIDTH, HEIGHT);
}

int8_t hiscore_add(uint16_t score) {
    // find index of score
    uint8_t i, j;
    highscore_t *hiscore;
    for(i = 0; i < HISCORES_COUNT; i++) {
        hiscore = &hiscores[i];
        if(score > hiscore->score) break; // we found your rank
    }
    if(i >= HISCORES_COUNT) return -1; // sorry, you didn't make the table!

    // move everything else down, drop the lowest
    for(j = HISCORES_COUNT - 1; j > i; j--) {
        hiscores[j].score = hiscores[j-1].score;
        memcpy(&hiscores[j], &hiscores[j-1], sizeof(highscore_t));
    }

    // update the table with your score
    hiscore->score = score;
    memcpy(&hiscore->initials, &last_initials, 3);

    hiscore_show();

    uint8_t _frames = 0, charindex = 0;
    char c = hiscore->initials[charindex];
    uint16_t input1 = 0, last_input = 0;
    while(1) {
        gfx_wait_vblank(&vctx);
        if(_frames > 60) _frames = 0;
        if(_frames > 30) {
            sprintf(_buffer, " ");
        } else {
            sprintf(_buffer, "%c", c);
        }
        nprint_string(&vctx, _buffer, 1, OFFSET_X + charindex, OFFSET_Y + i);

        _frames++;
        gfx_wait_end_vblank(&vctx);

        uint16_t input1 = input_get();

        if(input1 != last_input) {
            if(LEFT1 || RIGHT1) {
                nprint_string(&vctx, &c, 1, OFFSET_X + charindex, OFFSET_Y + i);
                if (LEFT1)  charindex--;
                if (RIGHT1) charindex++;
                if(charindex == 255) charindex = 2;
                if(charindex > 2) charindex = 0;
                c = hiscore->initials[charindex];
            }

            if (UP1)    c++;
            if (DOWN1)  c--;

            if(c > 90) c = 65;
            if(c < 65) c = 90;
            hiscore->initials[charindex] = c;

            if (BUTTON1_B) break; // we're done!
        }
        last_input = input1;
    }

    // copy the last initials for the next round
    memcpy(&last_initials, &hiscore->initials, 3);

    hiscore_show();
    msleep(3000);
    hiscore_hide();
#ifdef HISCORE_PATH
    hiscore_save(HISCORE_PATH);
#endif

    return i;
}

static zos_err_t ensure_parent_dirs(const char *path) {
    char dir[PATH_MAX];
    uint8_t i = 0;

    while(path[i] != 0) {
        if(i >= PATH_MAX - 1)
            return ERR_PATH_TOO_LONG;
        dir[i] = path[i];
        i++;
    }
    dir[i] = 0;

    for(i = 0; dir[i] != 0; i++) {
        if(dir[i] != '/')
            continue;

        if(i == 0)
            continue;
        if((i == 2) && (dir[1] == ':'))
            continue;

        dir[i] = 0;
        zos_err_t err = mkdir(dir);
        dir[i] = '/';

        if((err != ERR_SUCCESS) && (err != ERR_ALREADY_EXIST))
            return err;
    }

    return ERR_SUCCESS;
}

zos_err_t hiscore_save(const char *path) {
    zos_err_t err = ensure_parent_dirs(path);
    if(err != ERR_SUCCESS)
        return err;

    zos_dev_t dev = open(path, O_WRONLY | O_CREAT | O_TRUNC);
    if(dev < 0)
        return (zos_err_t)(-dev);

    uint16_t size = 3;
    err = write(dev, HISCORE_FORMAT, &size);
    if(err != ERR_SUCCESS)
        goto done;
    if(size != 3) {
        err = ERR_FAILURE;
        goto done;
    }

    uint8_t version = HISCORE_VERSION;
    size = sizeof(version);
    err = write(dev, &version, &size);
    if(err != ERR_SUCCESS)
        goto done;
    if(size != sizeof(version)) {
        err = ERR_FAILURE;
        goto done;
    }

    size = sizeof(hiscores);
    err = write(dev, hiscores, &size);
    if(err != ERR_SUCCESS)
        goto done;
    if(size != sizeof(hiscores)) {
        err = ERR_FAILURE;
        goto done;
    }

    size = sizeof(last_initials);
    err = write(dev, last_initials, &size);
    if(err != ERR_SUCCESS)
        goto done;
    if(size != sizeof(last_initials))
        err = ERR_FAILURE;

done:
    close(dev);
    return err;
}

zos_err_t hiscore_load(const char *path) {
    char format[3];
    uint8_t version;
    highscore_t loaded_hiscores[HISCORES_COUNT];
    char loaded_initials[3];
    zos_err_t err;
    uint16_t size;

    zos_dev_t dev = open(path, O_RDONLY);
    if(dev < 0)
        return (zos_err_t)(-dev);

    size = 3;
    err = read(dev, format, &size);
    if(err != ERR_SUCCESS)
        goto done;
    if((size != 3) || (memcmp(format, HISCORE_FORMAT, 3) != 0)) {
        err = ERR_ENTRY_CORRUPTED;
        goto done;
    }

    size = sizeof(version);
    err = read(dev, &version, &size);
    if(err != ERR_SUCCESS)
        goto done;
    if((size != sizeof(version)) || (version != HISCORE_VERSION)) {
        err = ERR_ENTRY_CORRUPTED;
        goto done;
    }

    size = sizeof(loaded_hiscores);
    err = read(dev, loaded_hiscores, &size);
    if(err != ERR_SUCCESS)
        goto done;
    if(size != sizeof(loaded_hiscores)) {
        err = ERR_ENTRY_CORRUPTED;
        goto done;
    }

    size = sizeof(loaded_initials);
    err = read(dev, loaded_initials, &size);
    if(err != ERR_SUCCESS)
        goto done;
    if(size != sizeof(loaded_initials)) {
        err = ERR_ENTRY_CORRUPTED;
        goto done;
    }

    memcpy(hiscores, loaded_hiscores, sizeof(hiscores));
    memcpy(last_initials, loaded_initials, sizeof(last_initials));

done:
    close(dev);
    return err;
}
