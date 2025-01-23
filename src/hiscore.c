#include <stdint.h>
#include <stdio.h>
#include <string.h>
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
    { .initials = { 'D', 'P', 'H' }, .score = 2000 },
    { .initials = { 'D', 'P', 'H' }, .score = 800 },
    { .initials = { 'D', 'P', 'H' }, .score = 700 },
    { .initials = { 'D', 'P', 'H' }, .score = 600 },
    { .initials = { 'D', 'P', 'H' }, .score = 500 },
    { .initials = { 'D', 'P', 'H' }, .score = 400 },
    { .initials = { 'D', 'P', 'H' }, .score = 300 },
    { .initials = { 'D', 'P', 'H' }, .score = 200 },
    { .initials = { 'D', 'P', 'H' }, .score = 100 },
    { .initials = { 'D', 'P', 'H' }, .score = 50 },
};
static char last_initials[3] = { 'A', 'A', 'A' };
static char buffer[16];
static uint8_t controller_mode = 1;

void hiscore_init(uint8_t controller) {
    controller_mode = controller;
}

void hiscore_show(void) {
    tilemap_fill(&vctx, BLANK_TILE, 1, 0, 0, WIDTH, HEIGHT);


    uint8_t i;
    for(i = 0; i < HISCORES_COUNT; i++) {
        highscore_t *score = &hiscores[i];
        sprintf(buffer, "%.3s  %05hu", score->initials, score->score);
        nprint_string(&vctx, buffer, 10, OFFSET_X, OFFSET_Y + i);
    }

    // msleep(5000);
    // tilemap_fill(&vctx, BLANK_TILE, 1, 0, 0, WIDTH, HEIGHT);
}

int8_t hiscore_add(uint16_t score) {
    // find index of score
    uint8_t i, j;
    highscore_t *hiscore;
    for(i = 0; i < HISCORES_COUNT; i++) {
        hiscore = &hiscores[i];
        if(score > hiscore->score) break;
    }
    if(i >= HISCORES_COUNT) return -1; // sorry, you didn't make the table!

    // move everything else down, drop the lowest
    for(j = HISCORES_COUNT - 1; j > i; j--) {
        hiscores[j].score = hiscores[j-1].score;
        memcpy(&hiscores[j], &hiscores[j-1], sizeof(highscore_t));
    }

    hiscores->score = score;
    memcpy(&hiscore->initials, &last_initials, 3);

    hiscore_show();

    // blink the active index???
    uint8_t frames = 0, charindex = 0;
    char c = hiscore->initials[charindex];
    uint16_t input1 = 0, last_input = 0;
    while(1) {
        gfx_wait_vblank(&vctx);
        if(frames > 60) frames = 0;
        if(frames > 30) {
            sprintf(buffer, " ");
        } else {
            sprintf(buffer, "%c", c);
        }
        nprint_string(&vctx, buffer, 1, OFFSET_X + charindex, OFFSET_Y + i);

        frames++;
        gfx_wait_end_vblank(&vctx);

        uint16_t input1 = keyboard_read();
        if (controller_mode == 1) {
            input1 |= controller_read();
        }

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

    hiscore_show();
    msleep(3000);
    tilemap_fill(&vctx, EMPTY_TILE, 1, 0, 0, WIDTH, HEIGHT);

    return i;
}