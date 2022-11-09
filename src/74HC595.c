#include "../include/74HC595.h"

static inline void shift_out_byte(sr_74hc595 const * sr, byte_t const data)
{
    for (uint8_t i = 0; i < 8; i++) {
        gpio_put(sr->data_pin, (data >> i) & 0x01);
        gpio_put(sr->clk_pin, 1);
        gpio_put(sr->clk_pin, 0);
    }
}

int8_t sr_init(sr_74hc595 * sr)
{
    if (sr->data_pin < 29) {
        gpio_init(sr->data_pin);
        gpio_set_dir(sr->data_pin, GPIO_OUT);
        gpio_put(sr->data_pin, 0);
    } else {
        return -1;
    }
    if (sr->clk_pin < 29) {
        gpio_init(sr->clk_pin);
        gpio_set_dir(sr->clk_pin, GPIO_OUT);
        gpio_put(sr->clk_pin, 0);
    } else {
        return -1;
    }
    if (sr->latch_pin < 29) {
        gpio_init(sr->latch_pin);
        gpio_set_dir(sr->latch_pin, GPIO_OUT);
        gpio_put(sr->latch_pin, 0);
    } else {
        sr->latch_pin = -1;
    }
    if (sr->oe_pin < 29) {
        gpio_init(sr->oe_pin);
        gpio_set_dir(sr->oe_pin, GPIO_OUT);
        gpio_put(sr->oe_pin, 0);
    } else {
        sr->oe_pin = -1;
    }
    if (sr->clear_pin < 29) {
        gpio_init(sr->clear_pin);
        gpio_set_dir(sr->clear_pin, GPIO_OUT);
        gpio_put(sr->clear_pin, 1);
    } else {
        sr->clear_pin = -1;
    }
    return 0;
}

void sr_write_byte(sr_74hc595 * sr, byte_t const data)
{
    shift_out_byte(sr, data);
    gpio_put(sr->latch_pin, 1);
    gpio_put(sr->latch_pin, 0);
    sr->state = data;
}

void sr_write_bit(sr_74hc595 * sr, uint8_t const pin, bool const value)
{
    byte_t tmp = sr->state;
    if ((int8_t)pin > 7) {
        return;
    }

    if (value) {
        tmp = tmp | (0b00000001 << pin);
    } else {
        tmp = tmp & ~(0b00000001 << pin);
    }
    sr_write_byte(sr, tmp);
}

void sr_clear_output(sr_74hc595 * sr)
{
    if (sr->clear_pin != -1) {
        gpio_put(sr->clear_pin, 0);
        gpio_put(sr->latch_pin, 1);
        gpio_put(sr->latch_pin, 0);
        gpio_put(sr->clear_pin, 1);
    }
}

void sr_cascaded_regs_write_bytes(sr_74hc595 * sr, uint8_t const nr_cascaded_regs, byte_t const data[])
{
    for (uint8_t i = 0; i < nr_cascaded_regs; i++) {
        shift_out_byte(sr, data[i]);
    }
    gpio_put(sr->latch_pin, 1);
    gpio_put(sr->latch_pin, 0);
    sr->state = data[nr_cascaded_regs - 1];
}