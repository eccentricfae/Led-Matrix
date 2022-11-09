/**
 * @file menu.h
 * @author faeisanxious@GitHub
 * @brief TODO
 * @version 0.9
 * @date 2022-10-30
 */

#ifndef _MENU_H
#define _MENU_H

#include "pico/stdlib.h"
#include "board.h"
#include "utilities.h"

#define MENU_FLASH_INTERVAL_MS 500

/**
 * @brief Displays the main menu on the matrix and 
 * waits for the input (which items is selected)
 * 0 == Animations, 1 == Snake, 2 == Pong, 3 == Random Lights
 * @return uint8_t Descriptor of the selected module of the project
 */
uint8_t menu(void);

#endif