#include <stdint.h>

#ifndef HISCORE_H
#define HISCORE_H

#define HISCORES_COUNT  10

typedef struct {
    char initials[3];
    uint16_t score;
} highscore_t;

void hiscore_init(void);

/**
 * @brief Show the hiscore table
 */
void hiscore_show(void);

/**
 * @brief Hide the hiscore table
 */
void hiscore_hide(void);

/**
 * @brief add a score to the table
 * @return -1 if score is too low, otherwise the index on the table (ie; 0 for highest score)
 */
int8_t hiscore_add(uint16_t score);

zos_err_t hiscore_save(const char *path);

zos_err_t hiscore_load(const char *path);

#endif