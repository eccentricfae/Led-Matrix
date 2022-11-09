#include "pico/stdlib.h"
#include "../include/74HC595.h"
#include "../include/board.h"
#include "../include/utilities.h"
#include "../include/menu.h"
#include "../Animations/include/animations.h"
#include "../Snake/include/snake.h"
#include "../Pong/include/pong.h"

void main(void)
{
    // Setup
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
    gp_row_regs = &row_regs;
    gp_col_regs = &col_regs;
    sr_init(&row_regs);
    sr_init(&col_regs);
    init_rand();

    // Main loop, lets user choose their sub-project they want to launch
    // All of the logic for different sub-projects (menu included)
    // is implemented in different directories or files :)
    while (true) {
        switch (menu()) {
            case 0:
                init_animations();
                run_animations(); 
                break;
            case 1:
                init_snake_game();
                // Easy, Medium, Hard
                run_snake_game(Medium); 
                break;
            case 2:
                // Ez, Normal, Oof
                run_pong(&Normal);
                break;
            default:
                break;
        }
    }
}