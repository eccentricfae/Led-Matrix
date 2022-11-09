#include "../include/pong.h"

/******************** GLOBAL VARIABLES ********************/

pong_difficulty_t Ez = {
    .move_scaler = 1,
    .update_interval = 500
};

pong_difficulty_t Normal = {
    .move_scaler = 2,
    .update_interval = 250
};

pong_difficulty_t Oof = {
    .move_scaler = 3,
    .update_interval = 100
};

paddle_dir_t * p_pdir = NULL;


/******************** CALLBACKS ********************/

static void input_callback(uint gpio, uint32_t eventmask)
{
    if (gpio == LEFT_INPUT_GPIO) {
        *p_pdir = pLeft;
    } else {
        *p_pdir = pRight;
    }
}

static bool update_rt_callback(repeating_timer_t * rt)
{
    *(bool *)rt->user_data = true;
}

static bool go_callback(repeating_timer_t * rt)
{
    *(bool *)rt->user_data = false;
    return false;
}

/******************** INIT FUNCTIONS ********************/

void init_board(pong_game_board_t * pb)
{
    for (uint8_t i = 0; i < BOARD_ROW_COUNT; i++) {
        for (uint8_t j = 0; j < BOARD_COL_COUNT; j++) {
        pb->brd[i][j] = (i == 0 || j == 0 || j == 15) ? Wall : Empty;
        }
    }
    pb->ppos = 6;
    for (uint8_t i = 0; i < 4; i++) {
        pb->brd[15][pb->ppos + i] = Paddle;
    }
    pb->pdir = pNoMove;
}

void init_ball(pong_game_board_t * pb)
{
    pb->ball_coords.x = (double)BALL_STARTPOS_COL;
    pb->ball_coords.y = (double)BALL_STARTPOS_ROW;
    pb->brd[(int)round(pb->ball_coords.y)][(int)round(pb->ball_coords.x)] = Ball;
    pb->ball_vect.x = rand_double(-0.4, 0.4);
    pb->ball_vect.y = -(sqrt(1.0f - (pb->ball_vect.x * pb->ball_vect.x)));
}

/******************** MAIN LOGIC ********************/

void move_paddle(pong_game_board_t * pb)
{
    paddle_dir_t pd = pb->pdir;

    if (pd == pNoMove) { return; }
    if (pb->ppos == 1 && pd == pLeft) { return; }
    if (pb->ppos == 11 && pd == pRight) { return; }

    pb->ppos += pd;

    int8_t x = (pd == pLeft) ? 0 : -1;
    int8_t n = (pd == pLeft) ? 5 : 4;

    for (int8_t i = x; i < n; i++) {
        if (i == x) {
            pb->brd[15][pb->ppos + i] = (pd == pLeft) ? Paddle : Empty;
        } else if (i == n-1) {
            pb->brd[15][pb->ppos + i] = (pd == pLeft) ? Empty : Paddle;
        } else {
            pb->brd[15][pb->ppos + i] = Paddle;
        }
    }
    pb->pdir = pNoMove;
}

int8_t is_in_board(dcrds_t const * p, vect_t const * v)
{
    // Yeah this kinds also doesnt work the best
    // TODO: rewrite it? depending on the update_game() fun
    double nx = p->x + v->x;
    double ny = p->y + v->y; 

    bool x_in = (nx > 0.0f) && (nx < 15.0f);
    bool y_in = (ny > 0.0f) && (ny < 15.0f);
    
    if (x_in && y_in) {
        return 0;
    } else {
        if (!x_in && !y_in) {
            if (fabs(v->x) <= fabs(v->y)) {
                if (p->y > 8.0f) {
                    return -3;
                } else {
                    return -1;
                }
            } else {
                if (p->x > 8.0f) {
                    return -2;
                } else {
                    return -4;
                }
            }
        }
        if (ny <= 0 && x_in) {
            return 1;
        }
        if (nx >= 15.0f && y_in) {
            return 2;
        }
        if (ny >= 15.0f && x_in) {
            return 3;
        }
        if (nx <= 0 && y_in) {
            return 4;
        }
        return 0;
    }
}

bool update_game(pong_game_board_t * pb, pong_difficulty_t const * diff)
{
    static uint8_t cntr = 0;
    int x_aux, y_aux;
    double aux;
    int8_t tmp;

    if (cntr++ % diff->move_scaler == 0) {
        move_paddle(pb);
    }

    x_aux = (int)round(pb->ball_coords.x);
    y_aux = (int)round(pb->ball_coords.y);
    if (y_aux !=0 && y_aux != 15 && x_aux != 0 && x_aux != 15) {
        pb->brd[y_aux][x_aux] = Empty;
    }

    // If someone is reading and / or analyzing the code:
    // this below is a "logic" for moving the ball. Now, moving the ball in itself 
    // is easy, the harder parts are the bounces. So I'd like to mention here:
    // this is a very poor "logic" for determining to where the ball will bounce
    // and I know I can do it better (copium), but:
    //      1. I don't need really good accuracy since this will be displayed
    //         on a 16x16 grid, it's not like you could see the difference
    //      2. I've been working on this project for longer than I would have liked
    //         and I'm drowning in college work right now so fuck it, I just wanna
    //         complete it :)
    // TODO: rewrite it using some inline functions and loops?? cuz a lot of the code is repeating
    // TODO: rewrite it !OR! make sure this works cuz it doesnt really work all of the time RN
    switch (is_in_board(&pb->ball_coords, &pb->ball_vect)) {
        case 0:
            // new_pos in board -> no bounce
            pb->ball_coords.x = pb->ball_coords.x + pb->ball_vect.x;
            pb->ball_coords.y = pb->ball_coords.y + pb->ball_vect.y;    
            break;
        case 1:
            // Bounce: Top
            pb->ball_coords.x = pb->ball_coords.x + pb->ball_vect.x;
            pb->ball_coords.y = -(pb->ball_coords.y + pb->ball_vect.y);
            pb->ball_vect.y = -(pb->ball_vect.y);
            break;
        case -1:
            if (pb->ball_coords.x > 8.0f) {
                // Bounce top then right
                pb->ball_coords.x = pb->ball_coords.x + pb->ball_vect.x;
                pb->ball_coords.y = -(pb->ball_coords.y + pb->ball_vect.y);
                pb->ball_vect.y = -(pb->ball_vect.y);

                pb->ball_coords.y = pb->ball_coords.y + pb->ball_vect.y;
                pb->ball_coords.x = 15.0f - ((pb->ball_coords.x + pb->ball_vect.x) - 15.0f);
                pb->ball_vect.x = -(pb->ball_vect.x);
            } else {
                // Bounce top then left
                pb->ball_coords.x = pb->ball_coords.x + pb->ball_vect.x;
                pb->ball_coords.y = -(pb->ball_coords.y + pb->ball_vect.y);
                pb->ball_vect.y = -(pb->ball_vect.y);

                pb->ball_coords.y = pb->ball_coords.y + pb->ball_vect.y;
                pb->ball_coords.x = -(pb->ball_coords.x + pb->ball_vect.x);
                pb->ball_vect.x = -(pb->ball_vect.x);
            }
            break;
        case 2:
            // Bounce: Right
            pb->ball_coords.y = pb->ball_coords.y + pb->ball_vect.y;
            pb->ball_coords.x = 15.0f - ((pb->ball_coords.x + pb->ball_vect.x) - 15.0f);
            pb->ball_vect.x = -(pb->ball_vect.x);
            break;
        case -2:
            // Bounce: Right and some other
            if (pb->ball_coords.y > 8.0f) {
                // Bounce right then bot
                pb->ball_coords.y = pb->ball_coords.y + pb->ball_vect.y;
                pb->ball_coords.x = 15.0f - ((pb->ball_coords.x + pb->ball_vect.x) - 15.0f);
                pb->ball_vect.x = -(pb->ball_vect.x);
            
                aux = pb->ball_coords.y + pb->ball_vect.y; 
                if (aux >= 16.0f) { return true; }
                if ((pb->ball_coords.x + pb->ball_vect.x) >= pb->ppos && (pb->ball_coords.x + pb->ball_vect.x) <= (pb->ppos + 3)) {
                    // Bounce
                    pb->ball_coords.x += pb->ball_vect.x;
                    pb->ball_coords.y = 15.0f - ((pb->ball_coords.y + pb->ball_vect.y) - 15.0f);
                    pb->ball_vect.y = -(pb->ball_vect.y);
                } else {
                    return true;
                }
            } else {
                // Bounce right then top
                pb->ball_coords.y = pb->ball_coords.y + pb->ball_vect.y;
                pb->ball_coords.x = 15.0f - ((pb->ball_coords.x + pb->ball_vect.x) - 15.0f);
                pb->ball_vect.x = -(pb->ball_vect.x);
            
                pb->ball_coords.x = pb->ball_coords.x + pb->ball_vect.x;
                pb->ball_coords.y = -(pb->ball_coords.y + pb->ball_vect.y);
                pb->ball_vect.y = -(pb->ball_vect.y);
            }
            break;
        case 3:
            // Something to do with being out of the bottom or hitting the paddle?
            aux = pb->ball_coords.y + pb->ball_vect.y; 
            if (aux >= 16.0f) { return true; }
            if ((pb->ball_coords.x + pb->ball_vect.x) >= pb->ppos && (pb->ball_coords.x + pb->ball_vect.x) <= (pb->ppos + 3)) {
                // Bounce
                pb->ball_coords.x += pb->ball_vect.x;
                pb->ball_vect.x += rand_double(-0.4, 0.4);
                pb->ball_coords.y = 15.0f - ((pb->ball_coords.y + pb->ball_vect.y) - 15.0f);
                // pb->ball_vect.y = -(pb->ball_vect.y);
                pb->ball_vect.y = -(sqrt(1.0f - (pb->ball_vect.x * pb->ball_vect.x)));
            } else {
                return true;
            }
            break;
        case -3:
            if (pb->ball_coords.x > 8.0f) {
                // Bounce bot then right
                aux = pb->ball_coords.y + pb->ball_vect.y; 
                if (aux >= 16.0f) { return true; }
                if ((pb->ball_coords.x + pb->ball_vect.x) >= pb->ppos && (pb->ball_coords.x + pb->ball_vect.x) <= (pb->ppos + 3)) {
                    // Bounce
                    pb->ball_coords.x += pb->ball_vect.x;
                    pb->ball_vect.x += rand_double(-0.4, 0.4);
                    pb->ball_coords.y = 15.0f - ((pb->ball_coords.y + pb->ball_vect.y) - 15.0f);
                    pb->ball_vect.y = -(sqrt(1.0f - (pb->ball_vect.x * pb->ball_vect.x)));
                } else {
                    return true;
                }

                pb->ball_coords.y = pb->ball_coords.y + pb->ball_vect.y;
                pb->ball_coords.x = 15.0f - ((pb->ball_coords.x + pb->ball_vect.x) - 15.0f);
                pb->ball_vect.x = -(pb->ball_vect.x);
            } else {
                // Bounce bot then left
                aux = pb->ball_coords.y + pb->ball_vect.y; 
                if (aux >= 16.0f) { return true; }
                if ((pb->ball_coords.x + pb->ball_vect.x) >= pb->ppos && (pb->ball_coords.x + pb->ball_vect.x) <= (pb->ppos + 3)) {
                    // Bounce
                    pb->ball_coords.x += pb->ball_vect.x;
                    pb->ball_vect.x += rand_double(-0.4, 0.4);
                    pb->ball_coords.y = 15.0f - ((pb->ball_coords.y + pb->ball_vect.y) - 15.0f);
                    pb->ball_vect.y = -(sqrt(1.0f - (pb->ball_vect.x * pb->ball_vect.x)));

                } else {
                    return true;
                }

                pb->ball_coords.y = pb->ball_coords.y + pb->ball_vect.y;
                pb->ball_coords.x = -(pb->ball_coords.x + pb->ball_vect.x);
                pb->ball_vect.x = -(pb->ball_vect.x);
            }
            break;
        case 4:
            // Bounces off of the Left Wall
            pb->ball_coords.y = pb->ball_coords.y + pb->ball_vect.y;
            pb->ball_coords.x = -(pb->ball_coords.x + pb->ball_vect.x);
            pb->ball_vect.x = -(pb->ball_vect.x);
            break;
        case -4:
            if (pb->ball_coords.y > 8.0f) {
                // Bounce left then bot
                pb->ball_coords.y = pb->ball_coords.y + pb->ball_vect.y;
                pb->ball_coords.x = -(pb->ball_coords.x + pb->ball_vect.x);
                pb->ball_vect.x = -(pb->ball_vect.x);

                aux = pb->ball_coords.y + pb->ball_vect.y; 
                if (aux >= 16.0f) { return true; }
                if ((pb->ball_coords.x + pb->ball_vect.x) >= pb->ppos && (pb->ball_coords.x + pb->ball_vect.x) <= (pb->ppos + 3)) {
                    // Bounce
                    pb->ball_coords.x += pb->ball_vect.x;
                    pb->ball_coords.y = 15.0f - ((pb->ball_coords.y + pb->ball_vect.y) - 15.0f);
                    pb->ball_vect.y = -(pb->ball_vect.y);
                } else {
                    return true;
                }
            } else {
                // Bounce left then top
                pb->ball_coords.y = pb->ball_coords.y + pb->ball_vect.y;
                pb->ball_coords.x = -(pb->ball_coords.x + pb->ball_vect.x);
                pb->ball_vect.x = -(pb->ball_vect.x);

                pb->ball_coords.x = pb->ball_coords.x + pb->ball_vect.x;
                pb->ball_coords.y = -(pb->ball_coords.y + pb->ball_vect.y);
                pb->ball_vect.y = -(pb->ball_vect.y);   
            }
            break;
    }

    if (fabs(pb->ball_vect.y) < 0.4) {
        if (pb->ball_vect.y > 0.0f) {
            pb->ball_vect.y += 0.5;
        } else {
            pb->ball_vect.y -= 0.5;
        }
        if (pb->ball_vect.x > 0) {
            pb->ball_vect.x = sqrt(1.0f - (pb->ball_vect.y * pb->ball_vect.y));
        } else {
            pb->ball_vect.x = -sqrt(1.0f - (pb->ball_vect.y * pb->ball_vect.y));
        }
    }

    pb->brd[(int)round(pb->ball_coords.y)][(int)round(pb->ball_coords.x)] = Ball;
    board_to_bytes(pb->brd, &pb->frame);
    return false;
}

void run_pong(pong_difficulty_t const * diff)
{
    matrix_print_frame(&g_empty_board_bytes);

    pong_game_board_t pong_board;
    repeating_timer_t update_rt;
    bool update = false;

    p_pdir = &pong_board.pdir;

    init_board(&pong_board);
    init_ball(&pong_board);
    board_to_bytes(pong_board.brd, &pong_board.frame);

    gpio_set_irq_enabled_with_callback(RIGHT_INPUT_GPIO, GPIO_IRQ_EDGE_RISE, true, input_callback);
    gpio_set_irq_enabled_with_callback(LEFT_INPUT_GPIO,  GPIO_IRQ_EDGE_RISE, true, input_callback);
    gpio_pull_down(RIGHT_INPUT_GPIO);
    gpio_pull_down(LEFT_INPUT_GPIO);

    add_repeating_timer_ms(-(diff->update_interval), update_rt_callback, &update, &update_rt);
    
    while (true) {
        if (update) {
            if (update_game(&pong_board, diff)) {
                // Game over
                cancel_repeating_timer(&update_rt);
                break;
            }
            update = false;
        }
        matrix_print_frame(&pong_board.frame);
    }

    gpio_set_irq_enabled(RIGHT_INPUT_GPIO, GPIO_IRQ_EDGE_RISE, false);
    gpio_set_irq_enabled(LEFT_INPUT_GPIO,  GPIO_IRQ_EDGE_RISE, false);
    gpio_disable_pulls(RIGHT_INPUT_GPIO);
    gpio_disable_pulls(LEFT_INPUT_GPIO);

    bool flip = true;
    repeating_timer_t go_rt;
    add_repeating_timer_ms(4000, go_callback, &flip, &go_rt);

    while (flip) {
        matrix_print_frame(&g_game_over_bytes);
    }
    cancel_repeating_timer(&go_rt);
    
}