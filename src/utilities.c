#include "../include/utilities.h"

uint32_t seed = 0;

void init_rand(void)
{
    if (seed != 0) { return; }
    uint8_t const * flash_target_contents = (uint8_t const *)(XIP_BASE + SEED_FLASH_OFFSET);
    static uint8_t seed_flash[FLASH_PAGE_SIZE];
    seed_flash[0] = flash_target_contents[0];
    seed_flash[1] = flash_target_contents[1];
    seed_flash[2] = flash_target_contents[2];
    seed_flash[3] = flash_target_contents[3];  
    seed = (uint32_t)(seed_flash[0] << 24) + (uint32_t)(seed_flash[1] << 16) + (uint32_t)(seed_flash[2] << 8) + (uint32_t)seed_flash[3];
    seed = (seed * 1664525U) + 1013904223U;
    seed_flash[0] += 1;
    seed_flash[1] += 2;
    seed_flash[2] += 3;
    seed_flash[3] += 4;    
    flash_range_erase(SEED_FLASH_OFFSET, FLASH_SECTOR_SIZE);
    flash_range_program(SEED_FLASH_OFFSET, seed_flash, FLASH_PAGE_SIZE);
}

inline uint16_t rand_uint16(uint16_t const range_start, uint16_t const range_end)
{
    seed = (seed * 1664525U) + 1013904223U;
    return (uint16_t)( range_start + (seed % (range_end - range_start + 1)));
}

inline double rand_double(double const range_start, double const range_end)
{
    seed = (seed * 1664525U) + 1013904223U;
    return range_start + (seed / (UINT32_MAX / (range_end-range_start)));
}
