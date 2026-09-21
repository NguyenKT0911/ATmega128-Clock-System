#ifndef DS18B20_H
#define DS18B20_H

#include <avr/io.h>

#define DS18B20_SKIP_ROM         0xCC
#define DS18B20_CONVERT_T        0x44
#define DS18B20_READ_SCRATCHPAD  0xBE

uint8_t ds18b20_start_conversion(void);
uint8_t ds18b20_read_temperature(int16_t *temperature);

#endif