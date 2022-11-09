#include "../include/pong.h"

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

    run_pong(&Normal);

    while (true) {
        matrix_print_frame(&g_main_menu_bytes);
    }
}