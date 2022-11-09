/**
 * @file pong.h
 * @author faeisanxious@GitHub
 * @brief TODO
 * @version 0.9
 * @date 2022-10-31
 * @brief To use this module:
 *  - declare, define, and sr_init() structs representing shift registers (74HC595.h lib)
 *  - assign ^ their addresses to the gp_row_regs / gp_col_regs pointers from the
 *    board.h lib
 *  - init_rand() from utilities.h lib
 *  - run_pong()
 *  - DONE
 */

#ifndef _PONG_H
#define _PONG_H

#include "../../include/74HC595.h"
#include "../../include/board.h"
#include "../../include/utilities.h"
#include <math.h>

#define BALL_STARTPOS_ROW 8
#define BALL_STARTPOS_COL 8

typedef struct pong_difficulty {
    uint8_t move_scaler;
    uint16_t update_interval;
} pong_difficulty_t;

typedef struct vect {
    double x;
    double y;
} vect_t;

/**
 * @brief dcrds == double coords (coords_t with double x, y instead of uint8_t x, y)
 */
typedef struct dcrds {
    double x;
    double y;
} dcrds_t;

typedef enum paddle_dir {
    pLeft = -1,
    pNoMove = 0,
    pRight = 1
} paddle_dir_t;

typedef enum board_fields {
    Empty,
    Wall,
    Ball,
    Paddle
} board_fields_t;

typedef struct pong_game_board {
    board_t brd;
    board_bytes_t frame;
    dcrds_t ball_coords;
    vect_t ball_vect;
    uint8_t ppos; // Leftmost part of the paddle
    paddle_dir_t pdir;
} pong_game_board_t;

extern pong_difficulty_t Ez;
extern pong_difficulty_t Normal;
extern pong_difficulty_t Oof;

void run_pong(pong_difficulty_t const * diff);

#endif