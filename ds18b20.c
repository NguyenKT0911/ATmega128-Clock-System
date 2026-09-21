#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>

#include "onewire.h"
#include "ds18b20.h"


uint8_t ds18b20_start_conversion(void)
{
    // Reset bus and check sensor presence
    if (!onewire_reset())
    {
        return 0;
    }

    // Only one DS18B20 on the bus
    onewire_write_byte(DS18B20_SKIP_ROM);

    // Start temperature conversion
    onewire_write_byte(DS18B20_CONVERT_T);

    return 1;
}
uint8_t ds18b20_read_temperature(int16_t *temperature)
{
    uint8_t lsb;
    uint8_t msb;
    int16_t raw;

    // Reset and check sensor
    if (!onewire_reset())
    {
        return 0;
    }

    // Skip ROM
    onewire_write_byte(DS18B20_SKIP_ROM);

    // Read Scratchpad
    onewire_write_byte(DS18B20_READ_SCRATCHPAD);

    // Read temperature bytes
    lsb = onewire_read_byte();
    msb = onewire_read_byte();

    // Combine MSB and LSB
    raw = ((int16_t)msb << 8) | lsb;

    // Convert to 0.1 degree Celsius
    *temperature = (raw * 10) / 16;

    return 1;
}