#ifndef _BOARD_H
#define _BOARD_H

#include "pico/stdlib.h"
#include "74HC595.h"

/**
 * To use this library:
 * - in main.c: declare and define cascaded registers for rows and columns
 * - in main.c: init those ^ registers (74HC595.h library)
 * - in main.c: assign those registers to *gp_row_regs and *gp_col_regs
 * - in modules: declare board_t <name>, but you dont really have to use it - you can
 *   do everything and the byte arrays (board_bytes_t) but it is significantly
 *   harder that way
 * - in modules: declare board_bytes_t <name> - this will be what will go to the printing
 *   functions to print your data onto the matrix
 * - in modules: you can declare board_bytes_t * <name> as a helpful auxilliary pointer
 *   to make changing from printing the board to empty board or menu easier
 */

#ifndef BYTE
#define BYTE
typedef unsigned char byte_t;
#endif


/********** DEFINES AND TYPEDEF'S **********/

#define BOARD_ROW_COUNT 16
#define BOARD_COL_COUNT 16
#define MENU_CHOICES_COUNT 3

/**
 * @brief Type representing each field on the board (each diode on the matrix)
 */
typedef uint8_t board_t[BOARD_ROW_COUNT][BOARD_COL_COUNT];

/**
 * @brief Type representing all of the data needed to print one
 * full frame on the matrix
 */
typedef byte_t board_bytes_t[BOARD_ROW_COUNT][2];
typedef board_bytes_t frame_t;

/**
 * @brief Auxiliary struct type used for representing
 * coordinates of something on the board
 */
typedef struct coords {
    uint8_t x;
    uint8_t y;
} coords_t;


/********** EXTERN / GLOBAL VARIABLES AND ARRAYS **********/

extern sr_74hc595 * gp_row_regs;
extern sr_74hc595 * gp_col_regs;

/**
 * @brief Frame that writes "Game Over" on the matrix
 */
extern board_bytes_t const g_game_over_bytes;
/**
 * @brief Empty frame
 */
extern board_bytes_t const g_empty_board_bytes;
/**
 * @brief Frame that prints main menu on the matrix
 */
extern board_bytes_t const g_main_menu_bytes;
/**
 * @brief Array of frames used for flashing the main menu board
 */
extern board_bytes_t const * menu_choices[MENU_CHOICES_COUNT];


/********** FUNCTION DELCARATIONS **********/

/**
 * @brief Print one frame on the matrix
 * 
 * @param bytes Pointer to the frame to be printed on the matrix
 */
void matrix_print_frame(board_bytes_t const * bytes);

/**
 * @brief While (true) matrix_print_frame
 * @note Dont use it :)
 * @note To be deprecated
 * @warning CAN ONLY BE BROKEN USING INTERRUPTS -> bytes = NULL
 * @param bytes Pointer to the frame to be printed on the matrix
 */
void matrix_display_engine(board_bytes_t const * bytes);

/**
 * @brief Converts info from passed board to board_bytes_t (frame_t)
 */
void board_to_bytes(board_t const brd, board_bytes_t * bytes);

/**
 * @brief Copies one frame to another
 */
void cpy_bytes(board_bytes_t * dest, board_bytes_t const * src);

#endif