#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "timer.h"


static volatile uint32_t system_ms = 0;


ISR(TIMER0_COMP_vect)
{
    system_ms++;
}


void timer_init(void)
{
    // Timer0 CTC mode
    TCCR0 = (1 << WGM01);

    // F_CPU = 8 MHz
    // Prescaler = 64
    //
    // 8 MHz / 64 = 125 kHz
    //
    // 1 count = 8 us
    //
    // 125 counts = 1 ms
    //
    // Timer dem 0 -> 124 = 125 counts
    OCR0 = 124;


    // Enable Output Compare Match interrupt
    TIMSK |= (1 << OCIE0);


    // Start Timer0
    // Prescaler = 64
    TCCR0 |= (1 << CS01) |
             (1 << CS00);


    // Global interrupt enable
    sei();
}


uint32_t timer_millis(void)
{
    uint32_t time;

    uint8_t sreg;

    // Luu trang thai interrupt
    sreg = SREG;

    cli();

    time = system_ms;

    // Khoi phuc trang thai interrupt
    SREG = sreg;

    return time;
}