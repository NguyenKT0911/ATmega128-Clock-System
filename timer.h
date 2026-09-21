#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>
#include <stdint.h>

void timer_init(void);

uint32_t timer_millis(void);

#endif