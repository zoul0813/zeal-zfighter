#include <zgdk.h>
#include "main.h"
#include "attract.h"
#include "player.h"
#include <zgdk/hiscore.h>

#define START1    (input1 & BUTTON_START)
#define PRESS_START     "Press  Start"
#define PRESS_BLINK     "            "

static uint8_t blink = 1;
static uint8_t erratic = 0;

void attract_init(void) {
    hiscore_show();
    player.direction.x = DIRECTION_LEFT;
    player.direction.y = DIRECTION_DOWN;
}

void attract_deinit(void) {
    hiscore_hide();
    reset();
}

void attract_mode(void) {
    player_attract();

    // blink "press start"
    if((frames % 32) == 0) {
        blink ^= 1;
        if(blink) {
            nprint_string(&vctx, PRESS_START, 12, 4, 14);

        } else {
            nprint_string(&vctx, PRESS_BLINK, 12, 4, 14);

        }
    }

    if((frames % 64) == 0) {
        erratic ^= 1;
        if(erratic) {
            player.direction.x = (rand8() % 2) == 0 ? DIRECTION_LEFT : DIRECTION_RIGHT;
            player.direction.y = (rand8() % 2) == 0 ? DIRECTION_DOWN : DIRECTION_UP;
        } else {
            player.direction.x = DIRECTION_NONE;
            player.direction.y = DIRECTION_NONE;
        }
    }
}
