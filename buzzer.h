#ifndef BUZZER_H
#define BUZEER_H

#include <avr/io.h>

#define BUZZER_DDR DDRC
#define BUZZER_PORT PORTC
#define BUZZER_PIN PC4

void buzzer_init(void);
void buzzer_on(void);
void buzzer_off(void);
void buzzer_toggle(void);

#endif
