#include "../include/menu.h"

int8_t curr_choice = 0;
bool changed = false;
bool chosen = false;


/******************** CALLBACKS ********************/

static void input_callback(uint gpio, uint32_t eventmask)
{
    if (changed) { return; }
    if (gpio == UP_INPUT_GPIO) {
        chosen = true;
        return;
    } else {
        if (++curr_choice >= MENU_CHOICES_COUNT) { curr_choice = 0; }
    }
    changed = true;
}

static bool menu_callback(repeating_timer_t * rt)
{
    static uint8_t cntr = 0;

    if (cntr % 2 != 0) {
        *(board_bytes_t const **)rt->user_data = &g_main_menu_bytes;
    } else {
        *(board_bytes_t const **)rt->user_data = menu_choices[curr_choice];
    }

    if (++cntr == 0xFF) { cntr = 0; }
    if (changed) { changed = false; }
    return true;
}


/******************** LOGIC ********************/

uint8_t menu(void)
{
    curr_choice = 0;
    changed = false;
    chosen = false;
    
    board_bytes_t const * bytes = &g_main_menu_bytes;
    repeating_timer_t menu_rt;
 
    gpio_set_irq_enabled_with_callback(UP_INPUT_GPIO,    GPIO_IRQ_EDGE_RISE, true, input_callback);
    gpio_set_irq_enabled_with_callback(RIGHT_INPUT_GPIO, GPIO_IRQ_EDGE_RISE, true, input_callback);
    gpio_pull_down(UP_INPUT_GPIO);
    gpio_pull_down(RIGHT_INPUT_GPIO);
    add_repeating_timer_ms(MENU_FLASH_INTERVAL_MS, menu_callback, &bytes, &menu_rt);

    while (true) {
        if (chosen) { break; }
        matrix_print_frame(bytes);
    }

    cancel_repeating_timer(&menu_rt);
    gpio_set_irq_enabled(UP_INPUT_GPIO, GPIO_IRQ_EDGE_RISE, false);
    gpio_set_irq_enabled(RIGHT_INPUT_GPIO, GPIO_IRQ_EDGE_RISE, false);
    gpio_disable_pulls(UP_INPUT_GPIO);
    gpio_disable_pulls(RIGHT_INPUT_GPIO);
    return curr_choice;
}