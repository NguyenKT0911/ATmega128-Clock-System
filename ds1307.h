#ifndef DS1307_H
#define DS1307_H

#include <avr/io.h>
#define DS1307_ADDRESS 0x68

typedef struct
{
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t date;
    uint8_t month;
    uint8_t year;
    
} DS1307_Time;

uint8_t ds1307_bcd_to_dec(uint8_t bcd);
uint8_t ds1307_dec_to_bcd(uint8_t dec);
uint8_t ds1307_set_time(const DS1307_Time *time);
uint8_t ds1307_get_time(DS1307_Time *time);

#endif
