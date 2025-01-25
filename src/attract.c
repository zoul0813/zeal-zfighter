#include <zgdk.h>
#include "main.h"
#include "attract.h"
#include "player.h"
#include "hiscore.h"

#define START1    (input1 & BUTTON_START)
#define PRESS_START     "Press  Start"
#define PRESS_BLINK     "            "

void attract_mode(uint8_t controller_mode) {
    hiscore_show();

    char text[20];
    uint16_t input1 = 0;
    uint8_t blink = 1, crazy_player = 0;
    player.direction.x = DIRECTION_LEFT;
    player.direction.y = DIRECTION_DOWN;

    while(1) {
        sound_loop();
        zmt_tick(&track, 1);

        // input();
        uint16_t input1 = keyboard_read();
        if (controller_mode == 1) {
            input1 |= controller_read();
        }

        if(START1) break;
        player_attract();

        // blink "press start"
        if((frames % 32) == 0) {
            blink ^= 1;
            if(blink) {
                sprintf(text, "%s", PRESS_START);
                nprint_string(&vctx, text, 12, 4, 14);

            } else {
                sprintf(text, "%s", PRESS_BLINK);
                nprint_string(&vctx, text, 12, 4, 14);

            }
        }

        if((frames % 64) == 0) {
            crazy_player ^= 1;
            if(crazy_player) {
                player.direction.x = (rand8() % 2) == 0 ? DIRECTION_LEFT : DIRECTION_RIGHT;
                player.direction.y = (rand8() % 2) == 0 ? DIRECTION_DOWN : DIRECTION_UP;
            } else {
                player.direction.x = DIRECTION_NONE;
                player.direction.y = DIRECTION_NONE;
            }
        }

        frames++;

        update();
        draw();
    }

    hiscore_hide();
}