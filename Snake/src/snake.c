#include "../include/snake.h"

typedef enum snake_fields {
    Empty,
    Wall,
    Snake,
    Food
} snake_fields_t;

/******************** FUNCTION DECLARATIONS ********************/

static void clean_up_snake_game(repeating_timer_t * rt);
static bool update_board(snake_game_board_t * snkbrd);
static void place_food(board_t * brd);


/******************** GLOBAL VARIABLES ********************/

uint8_t const nr_of_flashes = 8;
uint8_t cntr = 0;
snake_game_board_t game_board;
bool game_over = false;


/******************** CALLBACKS ********************/

/**
 * @brief Input handler (IRQ callback) for determining the 
 * direction of the snake 
 */
static void input_callback(uint gpio, uint32_t events)
{
    switch (gpio) {
        case Up:
            if (game_board.snake_dir == Down) { return; }
            else { game_board.snake_dir = Up; }
            return;
        case Right:
            if (game_board.snake_dir == Left) { return; }
            else { game_board.snake_dir = Right; }
            return;
        case Down:
            if (game_board.snake_dir == Up) { return; }
            else { game_board.snake_dir = Down; }
            return;
        case Left:
            if (game_board.snake_dir == Right) { return; }
            else { game_board.snake_dir = Left; }
            return;
        default:
            return;
    }
}

/**
 * @brief Callback used by repeating timers for updating the game
 */
static bool game_update(repeating_timer_t * rt)
{
    if (update_board(&game_board)) {
        // Game Over
        game_over = true;
        clean_up_snake_game(rt);
        return false;
    }
    // board_to_bytes(game_board.brd, bytes);
    board_to_bytes(game_board.brd, (board_bytes_t *)rt->user_data);
    return true;
}

/**
 * @brief Game Over callback for repeating timers - flashes the board
 */
static bool go_callback(repeating_timer_t * rt)
{
    static board_bytes_t const * aux_ptr;
    if (cntr == 0) {
        aux_ptr = *(board_bytes_t const **)rt->user_data;
    }

    if (cntr % 2 == 0) {
        *(board_bytes_t const **)rt->user_data = &g_empty_board_bytes;
    } else {
        *(board_bytes_t const **)rt->user_data = aux_ptr;
    }

    if(cntr++ > nr_of_flashes) {
        game_over = true;
        return false;
    }
    return true;
}


/******************** INIT FUNCTIONS ********************/

static void init_board(board_t * brd)
{
    for (uint8_t i = 0; i < BOARD_ROW_COUNT; i++) {
        for (uint8_t j = 0; j < BOARD_COL_COUNT; j++) {
            (*brd)[i][j] = (i == 0 || i == 15 || j == 0 || j == 15) ? Wall : Empty;
        }
    }
    place_food(brd);
}

static void init_snake(snake_game_board_t * gb)
{
    gb->head.next_part = NULL;
    do {
        gb->head.coords.x = rand_uint16(1, 14);
        gb->head.coords.y = rand_uint16(1, 14);
    } while (gb->brd[gb->head.coords.x][gb->head.coords.y] != Empty);
    gb->brd[gb->head.coords.x][gb->head.coords.y] = Snake;

    gb->snake_dir = rand_uint16(0, 3);
}

void init_snake_game()
{
    game_over = false;
    cntr = 0;
    init_board(&game_board.brd);
    init_snake(&game_board);
    
    gpio_set_irq_enabled_with_callback(UP_INPUT_GPIO,    GPIO_IRQ_EDGE_RISE, true, input_callback);
    gpio_set_irq_enabled_with_callback(RIGHT_INPUT_GPIO, GPIO_IRQ_EDGE_RISE, true, input_callback);
    gpio_set_irq_enabled_with_callback(DOWN_INPUT_GPIO,  GPIO_IRQ_EDGE_RISE, true, input_callback);
    gpio_set_irq_enabled_with_callback(LEFT_INPUT_GPIO,  GPIO_IRQ_EDGE_RISE, true, input_callback);
    
    gpio_pull_down(UP_INPUT_GPIO);
    gpio_pull_down(RIGHT_INPUT_GPIO);
    gpio_pull_down(DOWN_INPUT_GPIO);
    gpio_pull_down(LEFT_INPUT_GPIO);
}


/******************** GAME LOGIC FUNCTIONS ********************/

/**
 * @brief Places food on the board
 * 
 * @param brd 
 */
static void place_food(board_t * brd)
{
    coords_t fc;
    do {
        fc.x = rand_uint16(1, 14);
        fc.y = rand_uint16(1, 14);
    } while ((*brd)[fc.x][fc.y] != Empty);
    
    (*brd)[fc.x][fc.y] = Food;
}

static void lengthen_snake(snake_part_t * head)
{
    while (head->next_part != NULL) {
        head = head->next_part;
    }
    snake_part_t * tail = (snake_part_t *)malloc(sizeof(snake_part_t));
    head->next_part = tail;
    tail->next_part = NULL;
    tail->coords.x = head->coords.x;
    tail->coords.y = head->coords.y;
}

/**
 * @brief Moves the snake
 * 
 * @param snkbrd 
 * @param new_coords New coords for the head of the snake
 * @param grow Pass true if snake grows - "ate" food
 */
static void move_snake(snake_game_board_t * snkbrd, coords_t * new_coords, bool const grow)
{
    snake_part_t * snk;
    snk = &snkbrd->head;
    coords_t c_aux;
    bool first = true;

    if (grow) {
        lengthen_snake(&snkbrd->head);
    }

    // move snake
    while (snk->next_part != NULL) {
        c_aux = snk->coords;
        snk->coords = *new_coords;
        if (first) {
            snkbrd->brd[snk->coords.x][snk->coords.y] = Snake; // This is redundant assignment after the first one
            first = false;
        }
        *new_coords = c_aux;

        snk = snk->next_part;
    }
    // If we get here -> snk = last piece of the snake
    if (grow) {
        // Do not move this part!
    } else {
        c_aux = snk->coords;
        snk->coords = *new_coords;
        snkbrd->brd[snk->coords.x][snk->coords.y] = Snake;
        snkbrd->brd[c_aux.x][c_aux.y] = Empty;
    }
}

/**
 * @brief Update the board: determines where the snake moves,
 * calls approperiate function to move the snake, etc.
 * 
 * @param snkbrd Main snake_game_board_t object used for storing the info about the game
 * @return true Game Over
 * @return false Game continues
 */
static bool update_board(snake_game_board_t * snkbrd)
{
    coords_t nc;
    switch (snkbrd->snake_dir) {
        case Up:
            nc.x = snkbrd->head.coords.x - 1;
            nc.y = snkbrd->head.coords.y;
            break;
        case Right:
            nc.x = snkbrd->head.coords.x;
            nc.y = snkbrd->head.coords.y + 1;
            break;
        case Down:
            nc.x = snkbrd->head.coords.x + 1;
            nc.y = snkbrd->head.coords.y;
            break;
        case Left:
            nc.x = snkbrd->head.coords.x;
            nc.y = snkbrd->head.coords.y - 1;
            break;
        default:
            nc.x = snkbrd->head.coords.x - 1;
            nc.y = snkbrd->head.coords.y;
            break;
    }

    if (snkbrd->brd[nc.x][nc.y] == Empty) {
        move_snake(snkbrd, &nc, false);
        return false;
    } else if (snkbrd->brd[nc.x][nc.y] == Food) {
        move_snake(snkbrd, &nc, true);
        place_food(&snkbrd->brd);
        return false;
    } else {
        // Snake has run into a wall or into itself -> Game Over
        return true;
    }
}

void run_snake_game(difficulty_t const diff)
{
    int32_t update_interval = -(250 + (diff * 250));
    board_bytes_t bytes;
    board_bytes_t const * printable;
    printable = &bytes;

    repeating_timer_t update_timer;
    add_repeating_timer_ms(update_interval, game_update, &bytes, &update_timer);

    while (true) {
        if (game_over) { break; }
        matrix_print_frame(printable);
    }

    // Few instructions to "flash" the board couple of times and then this breaks
    repeating_timer_t go_rt;
    add_repeating_timer_ms(-500, go_callback, &printable, &go_rt);
    game_over = false;
    while (true) { 
        if (game_over) { break; }
        matrix_print_frame(printable);
    }
}


/******************** CLEAN UP FUNCTIONS ********************/

/**
 * @brief Function that deallocates all the snake_part_t objects
 * 
 * @param head Pointer to the first object, part of the snake_game_board_t
 */
static void deallocate_snake(snake_part_t * head)
{
    snake_part_t * aux;
    
    // Skip first part of the snake (head) since it isn't allocated dynamically
    if (head->next_part != NULL) {
        head = head->next_part;
    } else {
        // The snake was only 1 part (head only) long, so do nothing
        // cuz there is nothing to deallocate
        return;
    }

    while (head->next_part != NULL) {
        aux = head->next_part;
        free(head);
        head = aux;
    }
    free(head);
}

/**
 * @brief Simple clean-up function
 * 
 * @param rt ? Deprecated
 */
static void clean_up_snake_game(repeating_timer_t * rt)
{
    gpio_set_irq_enabled(UP_INPUT_GPIO,    GPIO_IRQ_EDGE_RISE, false);
    gpio_set_irq_enabled(RIGHT_INPUT_GPIO, GPIO_IRQ_EDGE_RISE, false);
    gpio_set_irq_enabled(DOWN_INPUT_GPIO,  GPIO_IRQ_EDGE_RISE, false);
    gpio_set_irq_enabled(LEFT_INPUT_GPIO,  GPIO_IRQ_EDGE_RISE, false);
    // cancel_repeating_timer(rt); // I don't need this cancel() if returning false from callback should stop the timer?
    gpio_disable_pulls(UP_INPUT_GPIO);
    gpio_disable_pulls(RIGHT_INPUT_GPIO);
    gpio_disable_pulls(DOWN_INPUT_GPIO);
    gpio_disable_pulls(LEFT_INPUT_GPIO);

    deallocate_snake(&game_board.head);
}