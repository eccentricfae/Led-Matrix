/**
 * @file 74HC595.h
 * @author Adrian Zaba, faeisanxious@GitHub (adrianzabax@gmail.com)
 * @brief see README.md @ https://github.com/faeisanxious/pico-sources/tree/main/74HC595-shift-register
 * @version 0.3
 * @date 2022-07-2
 */
#ifndef _74HC595_H
#define _74HC595_H

#include "pico/stdlib.h"

#ifndef BYTE
#define BYTE
typedef unsigned char byte_t;
#endif

/**
 * @brief Struct representing all of the necessary info about a shift
 * register connected to the microcontroller.
 */
typedef struct snsr_74hc595 {
    uint8_t data_pin;
    uint8_t oe_pin;
    uint8_t latch_pin;
    uint8_t clk_pin;
    uint8_t clear_pin;
    byte_t state;
} sr_74hc595;

/**
 * @brief Function that: initializes every GPIO pin used for controlling
 * the register, and sets directions of those GPIO pins to GPIO_OUT 
 * @param sr Pointer to the struct representing a shift register connected
 * to the microcontroller
 * @return int8_t Returns -1 if one of the pins used for essential signals
 * (SI signal or SCK signal) are set up incorrectly - so the register will
 * not be, or will be incorrectly, controlled. <br> Otherwise returns 0.
 */
int8_t sr_init(sr_74hc595 * sr);

/**
 * @brief Write a given byte of data to the parallel output (QA...QH pins)
 * of the shift register 
 * @param sr Pointer to the struct repesenting a physical register connected
 * to the microcontroller
 * @param data Byte of data to be written on the parallel output in the
 * following format: data bits b7...b0 == register pins QA...QH
 */
void sr_write_byte(sr_74hc595 * sr, byte_t const data);

/**
 * @brief Write a bit on the specified output pin of the register, while 
 * leaving all other outputs unchanged
 * @param sr Pointer to the struct repesenting a physical register connected
 * to the microcontroller
 * @param pin Pin on which the data "value" should be set. 
 * @note the pin number passed as a "pin" argument is not the number of the
 * physical pin of the register (as they are shown on the datasheet), but
 * the number of the 8-bit sized parallel output of the register, i.e.: tthe
 * QA...QH pins == pin nr 7...0
 * @param value Logical value of the bit to be set
 */
void sr_write_bit(sr_74hc595 * sr, uint8_t const pin, bool const value);

/**
 * @brief Set the parallel output of the register to be either active or 
 * in the state of high impedance
 * @param sr Pointer to the struct repesenting a physical register connected
 * to the microcontroller
 * @param enable_output True == outputs are enabled, flase == high impedance
 * on the output pins
 */
inline static void sr_set_output(sr_74hc595 const * sr, bool const enable_output)
{
    if (sr->oe_pin != -1) {
        gpio_put(sr->oe_pin, !enable_output);
    }
}

/**
 * @brief Clear the parallel output pins of the register by setting them 
 * to the logical value of 0.
 * @note If the parallel output of the register are desired to be all in low
 * logical value (logical 0) use this function instead of writing 0x00 as a
 * byte of data using the sr_write_byte() function, since this one is much
 * faster
 * @param sr Pointer to the struct repesenting a physical register connected
 * to the microcontroller
 */
void sr_clear_output(sr_74hc595 * sr);


/**
 * @brief Write passed data (8-bit words) to a system of cascaded registers 
 * @note This function assumes all of the cascaded registers' latch inputs
 * and clock inputs are connected together
 * @param sr Pointer to the struct representing a physical register connected
 * to the microcontroller, and the first register in the cascade
 * @param nr_cascaded_regs Number of cascaded registers
 * @param data Array of 8-bit words to be written to the cascaded registers.
 * data[0] is the word to be written on the "most cascaded" register, 
 * while data[nr_cascaded_regs-1] is the word to be written on the register
 * connected to the microcontroller.
 */
void sr_cascaded_regs_write_bytes(sr_74hc595 * sr, uint8_t const nr_cascaded_regs, byte_t const data[]);

#endif