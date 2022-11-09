#include "../include/board.h"

sr_74hc595 * gp_row_regs = NULL;
sr_74hc595 * gp_col_regs = NULL;

board_bytes_t const g_game_over_bytes = {
    { 0x00, 0x00},
    { 0xE4, 0xAE},
    { 0x8A, 0xE8},
    { 0xAA, 0xAE},
    { 0xAE, 0xA8},
    { 0xEA, 0xAE},
    { 0x00, 0x00},
    { 0x00, 0x00},
    { 0x00, 0x00},
    { 0x00, 0x00},
    { 0xEA, 0xEC},
    { 0xAA, 0x8A},
    { 0xAA, 0xEC},
    { 0xAA, 0x8A},
    { 0xE4, 0xEA},
    { 0x00, 0x00}
};

board_bytes_t const g_empty_board_bytes = {0};

board_bytes_t const g_main_menu_bytes = {
    { 0x00, 0x00},
    { 0x7E, 0x3E},
    { 0x42, 0x20},
    { 0x42, 0x3C},
    { 0x42, 0x04},
    { 0x42, 0x7C},
    { 0x7E, 0x40},
    { 0x00, 0x00},
    { 0x00, 0x00},
    { 0x38, 0x00},
    { 0x24, 0x00},
    { 0x24, 0x00},
    { 0x38, 0x00},
    { 0x20, 0x00},
    { 0x20, 0x00},
    { 0x00, 0x00}
};

board_bytes_t const menu_anim_select = {
    { 0x00, 0x00},
    { 0x00, 0x3E},
    { 0x00, 0x20},
    { 0x00, 0x3C},
    { 0x00, 0x04},
    { 0x00, 0x7C},
    { 0x00, 0x40},
    { 0x00, 0x00},
    { 0x00, 0x00},
    { 0x38, 0x00},
    { 0x24, 0x00},
    { 0x24, 0x00},
    { 0x38, 0x00},
    { 0x20, 0x00},
    { 0x20, 0x00},
    { 0x00, 0x00}
};

board_bytes_t const menu_snake_select = {
    { 0x00, 0x00},
    { 0x7E, 0x00},
    { 0x42, 0x00},
    { 0x42, 0x00},
    { 0x42, 0x00},
    { 0x42, 0x00},
    { 0x7E, 0x00},
    { 0x00, 0x00},
    { 0x00, 0x00},
    { 0x38, 0x00},
    { 0x24, 0x00},
    { 0x24, 0x00},
    { 0x38, 0x00},
    { 0x20, 0x00},
    { 0x20, 0x00},
    { 0x00, 0x00}
};

board_bytes_t const menu_pong_select = {
    { 0x00, 0x00},
    { 0x7E, 0x3E},
    { 0x42, 0x20},
    { 0x42, 0x3C},
    { 0x42, 0x04},
    { 0x42, 0x7C},
    { 0x7E, 0x40},
    { 0x00, 0x00},
    { 0x00, 0x00},
    { 0x00, 0x00},
    { 0x00, 0x00},
    { 0x00, 0x00},
    { 0x00, 0x00},
    { 0x00, 0x00},
    { 0x00, 0x00},
    { 0x00, 0x00}
};

board_bytes_t const * menu_choices[MENU_CHOICES_COUNT] = { &menu_anim_select, &menu_snake_select, &menu_pong_select };

// void matrix_print_frame(board_bytes_t const * bytes, sr_74hc595 * row_regs, sr_74hc595 * col_regs)
void matrix_print_frame(board_bytes_t const * bytes)
{
    static byte_t rows[2];
    static uint8_t i, j;
    if (bytes == NULL) { return; }

    rows[0] = 0x00;
    rows[1] = 0x80;
    for (i = 0; i < BOARD_ROW_COUNT/2; i++) {
        sr_cascaded_regs_write_bytes(gp_row_regs, 2, rows);
        sr_cascaded_regs_write_bytes(gp_col_regs, 2, &(*bytes)[i][0]);
        rows[1] = rows[1] >> 1;
        sleep_ms(1);
    }
    
    rows[0] = 0x80;
    rows[1] = 0x00;
    for (i = 8; i < BOARD_ROW_COUNT; i++) {
        sr_cascaded_regs_write_bytes(gp_row_regs, 2, rows);
        sr_cascaded_regs_write_bytes(gp_col_regs, 2, &(*bytes)[i][0]);
        rows[0] = rows[0] >> 1;
        sleep_ms(1);
    }
}

void matrix_display_engine(board_bytes_t const * bytes)
{
    static byte_t rows[2];
    static uint8_t i, j;
    if (bytes == NULL) { return; }

    while (true) {
        rows[0] = 0x00;
        rows[1] = 0x80;
        for (i = 0; i < BOARD_ROW_COUNT/2; i++) {
            sr_cascaded_regs_write_bytes(gp_row_regs, 2, rows);
            sr_cascaded_regs_write_bytes(gp_col_regs, 2, &(*bytes)[i][0]);
            rows[1] = rows[1] >> 1;
            sleep_ms(1);
        }
        
        rows[0] = 0x80;
        rows[1] = 0x00;
        for (i = 8; i < BOARD_ROW_COUNT; i++) {
            sr_cascaded_regs_write_bytes(gp_row_regs, 2, rows);
            sr_cascaded_regs_write_bytes(gp_col_regs, 2, &(*bytes)[i][0]);
            rows[0] = rows[0] >> 1;
            sleep_ms(1);
        }
    }
}

void board_to_bytes(board_t const brd, board_bytes_t * bytes)
{
    static uint8_t i = 0, j = 0;
    for (i = 0; i < BOARD_ROW_COUNT; i++) {
        for (j = 0; j < BOARD_COL_COUNT/2; j++) {
            if (brd[i][j] == 0) {
                (*bytes)[i][0] &= ~(0x80 >> j);
            } else {
                (*bytes)[i][0] |= 0x80 >> j;
            }
        }
        for (j = 0; j < BOARD_COL_COUNT/2; j++) {
            if (brd[i][j+8] == 0) {
                (*bytes)[i][1] &= ~(0x80 >> j);
            } else {
                (*bytes)[i][1] |= 0x80 >> j;
            }
        }
    }
}

void cpy_bytes(board_bytes_t * dest, board_bytes_t const * src)
{
    for (uint8_t i = 0 ; i < BOARD_ROW_COUNT; i++) {
        (*dest)[i][0] = (*src)[i][0];
        (*dest)[i][1] = (*src)[i][1];
    }
}