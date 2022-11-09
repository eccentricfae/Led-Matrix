/**
 * @file utilities.h
 * @author faeisanxious@GitHub
 * @brief Some utilities for the project
 * @version 0.1
 * @date 2022-10-29
 */
#ifndef _UTILITIES_H
#define _UTILITIES_H

#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/timer.h"

#define UP_INPUT_GPIO       0
#define RIGHT_INPUT_GPIO    1
#define DOWN_INPUT_GPIO     2
#define LEFT_INPUT_GPIO     3

#define SEED_FLASH_OFFSET (256 * 1024)

/**
 * @brief Initializes random number generation. 
 * @note Since I can't really do the time(0) to get a random seed, the 
 * initialization function reads some data from FLASH and sets it as seed, 
 * and then overwrites the data so each time the init_rand() is called the
 * seed will be different 
 */
void init_rand(void);

/**
 * @brief Generate random uint16_t number from the given range (borders included) == (closed set)
 */
uint16_t rand_uint16(uint16_t const range_start, uint16_t const range_end);

/**
 * @brief Generate random double number from the given range
 */
double rand_double(double const range_start, double const range_end);

#endif