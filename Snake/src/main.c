#include "../include/snake.h"

/**
 * This file is only meant to be used / included / built when you
 * want to test ONLY the Snake game part of the overall project.
 * The main.c file as well as main() does only the required 
 * declarations and calls the required init() functions so that
 * the Snake module and LED-Matrix work properly. For the "full"
 * experience, check out the main project which is located at the
 * top level of the repo.
 */

void main(void)
{
    sr_74hc595 row_regs = {
        .data_pin  = 14,
        .clk_pin   = 6,
        .latch_pin = 7,
        .clear_pin = -1,
        .oe_pin    = -1,
        .state = 0
    };
    sr_74hc595 col_regs = {
        .data_pin  = 27,
        .clk_pin   = 22,
        .latch_pin = 26,
        .clear_pin = -1,
        .oe_pin    = -1,
        .state = 0
    };
    sr_init(&row_regs);
    sr_init(&col_regs);
    
    gp_row_regs = &row_regs;
    gp_col_regs = &col_regs;

    init_rand();
    
    init_snake_game();
    run_snake_game(Medium);
    
    while (true) {
        matrix_print_frame(&g_main_menu_bytes);
    }
}