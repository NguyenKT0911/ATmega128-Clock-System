#ifndef LCD_H
#define LCD_H

#include <avr/io.h>

// LCD data: D0 -> D7 connected to PB0 -> PB7
#define LCD_DATA_DDR   DDRB // Input or Output
#define LCD_DATA_PORT  PORTB // Data port

// LCD control pins
#define LCD_CTRL_DDR   DDRD
#define LCD_CTRL_PORT  PORTD

#define LCD_RS  PD2
#define LCD_RW  PD3
#define LCD_E   PD4

// Function prototypes
void lcd_init(void);
void lcd_command(uint8_t cmd);
void lcd_putc(char data);
void lcd_puts(const char *str);
void lcd_gotoxy(uint8_t x, uint8_t y);
void lcd_clear(void);

#endif