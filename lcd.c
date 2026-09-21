#define F_CPU 8000000UL  // Define CPU frequency as 8 MHz
#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"

static void lcd_enable(void)
{
    LCD_CTRL_PORT |= (1 << LCD_E);  // Set E high
    _delay_us(1);                    // Wait for a short time
    LCD_CTRL_PORT &= ~(1 << LCD_E); // Set E low
    _delay_us(100);                  // Wait for the command to be processed
}

void lcd_command(uint8_t cmd)
{
    LCD_DATA_PORT = cmd;

    LCD_CTRL_PORT &= ~(1 << LCD_RS); // Set RS low for command
    LCD_CTRL_PORT &= ~(1 << LCD_RW); // Set RW low for write
    lcd_enable();

    _delay_ms(2); // Wait for the command to be processed
}

void lcd_putc(char data)
{
    LCD_DATA_PORT = data;

    LCD_CTRL_PORT |= (1 << LCD_RS);  // Set RS high for data
    LCD_CTRL_PORT &= ~(1 << LCD_RW); // Set RW low for write
    lcd_enable();

    _delay_us(100); // Wait for the data to be processed
}

void lcd_puts(const char *str)
{
    while(*str != '\0')
    {
        lcd_putc(*str); // Send each character to the LCD
        str++;          // Move to the next character       
    }
}

void lcd_gotoxy(uint8_t x, uint8_t y)
{
    uint8_t address;

    switch(y)
    {
        case 0:
            address = 0x00 + x; // First line
            break;
        case 1:
            address = 0x40 + x; // Second line
            break;
        case 2:
            address = 0x14 + x; // Third line
            break;
        case 3:
            address = 0x54 + x; // Fourth line
            break;
        default:
            return; // Invalid line number
    }
    lcd_command(0x80 | address); // Set DDRAM address
}

void lcd_clear(void)
{
    lcd_command(0x01); // Clear display command
    _delay_ms(2);      // Wait for the clear command to be processed    
}

void lcd_init(void)
{
    // PB0-PB7 are outputs
    LCD_DATA_DDR = 0xFF; // Set data port as output

    // PD2-PD4 are outputs
    LCD_CTRL_DDR |= (1 << LCD_RS) | (1 << LCD_RW) | (1 << LCD_E); // Set control pins as output

    // Initial control signals = 0
    LCD_CTRL_PORT &= ~((1 << LCD_RS) | (1 << LCD_RW) | (1 << LCD_E)); // Set control pins low
    _delay_ms(20); // Wait for LCD to power up
    lcd_command(0x38); // Function set: 8-bit, 2 line, 5x8 dots
    lcd_command(0x0C); // Display on, cursor off
    lcd_command(0x06); // Entry mode set: increment cursor
    lcd_command(0x01); // Clear display
    _delay_ms(2); // Wait for the clear command to be processed
}