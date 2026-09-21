#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "onewire.h"


static void onewire_low(void)
{
    ONEWIRE_PORT &= ~(1 << ONEWIRE_DQ);
    ONEWIRE_DDR  |=  (1 << ONEWIRE_DQ); // out
}


static void onewire_release(void)
{
    ONEWIRE_DDR &= ~(1 << ONEWIRE_DQ); // in
    ONEWIRE_PORT &= ~(1 << ONEWIRE_DQ); // 0
}

uint8_t onewire_reset(void)
{
    uint8_t presence;

    onewire_release();
    _delay_us(5);

    onewire_low();
    _delay_us(480);

    onewire_release();
    _delay_us(70);

    presence = !(ONEWIRE_PIN & (1 << ONEWIRE_DQ));

    _delay_us(410);

    return presence;
}

void onewire_write_bit(uint8_t bit)
{
    if (bit)
    {
        // Write 1
        onewire_low();
        _delay_us(6);

        onewire_release();
        _delay_us(64);
    }
    else
    {
        // Write 0
        onewire_low();
        _delay_us(60);

        onewire_release();
        _delay_us(10);
    }
}


uint8_t onewire_read_bit(void)
{
    uint8_t bit;

    // Start read slot
    onewire_low();
    _delay_us(6);

    // Release bus so DS18B20 can control it
    onewire_release();

    _delay_us(9);

    if (ONEWIRE_PIN & (1 << ONEWIRE_DQ))
        bit = 1;
    else
        bit = 0;

    _delay_us(55);

    return bit;
}


void onewire_write_byte(uint8_t data)
{
    uint8_t i;

    for (i = 0; i < 8; i++)
    {
        onewire_write_bit(data & 0x01);
        data >>= 1;
    }
}


uint8_t onewire_read_byte(void)
{
    uint8_t i;
    uint8_t data = 0;

    for (i = 0; i < 8; i++)
    {
        if (onewire_read_bit())
        {
            data |= (1 << i);
        }
    }

    return data;
}