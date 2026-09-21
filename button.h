#ifndef BUTTON_H
#define BUTTON_H

#include <avr/io.h>
#include <stdint.h>

#define BUTTON_DDR   DDRC
#define BUTTON_PORT  PORTC
#define BUTTON_PIN   PINC

#define BTN_MODE     PC0
#define BTN_SET      PC1
#define BTN_UP       PC2
#define BTN_DOWN     PC3

void button_init(void);
void button_update(uint32_t now);

// Event nhan 1 lan
uint8_t button_mode_pressed(void);
uint8_t button_set_pressed(void);
uint8_t button_up_pressed(void);
uint8_t button_down_pressed(void);

// Event nhan giu / auto repeat
uint8_t button_up_repeat(void);
uint8_t button_down_repeat(void);

// Kiem tra nut dang duoc giu
uint8_t button_up_held(void);
uint8_t button_down_held(void);

#endif