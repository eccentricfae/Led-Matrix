/**
 * @file snake.h
 * @author faeIsAnxious@GitHub
 * @brief Main file for the module that is the Snake game for this project's LED-MATRIX
 * @version 1.1
 * @date 2022-10-28
 * @details To use this module:
 *  - declare, define, and sr_init() structs representing shift registers (74HC595.h lib)
 *  - assign ^ their addresses to the gp_row_regs / gp_col_regs pointers from the
 *    board.h lib
 *  - init_rand() from utilities.h lib
 *  - init_snake_game() and then -> run_snake_game()
 *  - DONE
 */

#ifndef _SNAKE_H
#define _SNAKE_H

#include <stdlib.h>
#include "../../include/board.h"
#include "../../include/utilities.h"

/********** DEFINES AND TYPEDEFS *********/

/**
 * @brief Enumeration type used for determining the
 * difficulty of the game. (Difficulty == time interval
 * between frames)
 */
typedef enum difficulty {
    Hard,
    Medium,
    Easy
} difficulty_t;

/**
 * @brief Enumeration type used as an alias for clearer understanding
 * of direction in which the snake is moving 
 */
typedef enum direction {
    Up, 
    Right, 
    Down, 
    Left
} direction_t;

/**
 * @brief Type representing one "part" of the snake, 
 * implemented as a linked list
 */
typedef struct snake_part {
    coords_t coords;
    struct snake_part * next_part;
} snake_part_t;

/**
 * @brief Struct type containing all of the info needed for the
 * snake game to be run
 */
typedef struct snake_game_board {
    board_t brd;
    snake_part_t head;
    direction_t snake_dir;
} snake_game_board_t;


/********** PUBLIC FUNCTION DECLARATIONS *********/

/**
 * @brief Initialization function. to be run before run_snake_game()
 */
void init_snake_game();

/**
 * @brief All-in-one function that runs the Snake module of the project
 * 
 * @param diff Diffuclty of the game
 * @see difficulty_t
 */
void run_snake_game(difficulty_t const diff);

#endif