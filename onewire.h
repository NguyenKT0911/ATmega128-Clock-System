#ifndef ONEWIRE_H
#define ONEWIRE_H

#include <avr/io.h>

#define ONEWIRE_DDR DDRC
#define ONEWIRE_PORT PORTC
#define ONEWIRE_PIN PINC
#define ONEWIRE_DQ PC5

uint8_t onewire_reset(void);
void onewire_write_bit(uint8_t bit);
uint8_t onewire_read_bit(void);
void onewire_write_byte(uint8_t data);
uint8_t onewire_read_byte(void);
#endif