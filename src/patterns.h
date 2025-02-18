#ifndef PATTERNS_H
#define PATTERNS_H


typedef enum {
    F_SINE      = 0,
    F_TRI       = 1,
    F_SQUARE    = 2,
    F_LINE      = 3,
} Formation;

extern int8_t WAVE_SINE[];
extern int8_t WAVE_SQUARE[];

#endif