/**
 * @file animations.h
 * @author faeisanxious@GitHub
 * @brief TODO
 * @version 0.9
 * @date 2022-10-30
 * @details To use this module:
 *  - declare, define, and sr_init() structs representing shift registers (74HC595.h lib)
 *  - assign ^ their addresses to the gp_row_regs / gp_col_regs pointers from the
 *    board.h lib
 *  - init_rand() from utilities.h lib
 *  - init_animations() and then -> run_animations()
 *  - DONE
 */

#ifndef _ANIMATIONS_H
#define _ANIMATIONS_H

#include "../../include/74HC595.h"
#include "../../include/board.h"
#include "../../include/utilities.h"


/******************** DEFINES AND TYPEDEF'S ********************/

/**
 * @brief Type representing number of frames in each animation
 */
typedef uint8_t anim_size_t;

/**
 * @brief Type representing the time interval between frames in
 * an animation
 */
typedef uint16_t anim_time_t;

/**
 * @brief Struct type that holds all of the data that builds a one animation
 */
typedef struct animation {
    board_bytes_t const * ptr_to_frames;
    anim_size_t frames_count;
    anim_time_t time_between_frames_ms;
} animation_t;


/******************** PUBLIC FUNCTIONS DECLARATIONS ********************/

/**
 * @brief Initialization function, needed to be run before run_animation()
 */
void init_animations(void);

/**
 * @brief All-in-one function to run the Animations module
 */
void run_animations(void);

#endif