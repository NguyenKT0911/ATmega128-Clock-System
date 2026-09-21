#include <avr/io.h>
#include "ds1307.h"
#include "i2c.h"

uint8_t ds1307_bcd_to_dec(uint8_t bcd)
{
    return ((bcd >> 4) * 10) + (bcd & 0x0f);
}

uint8_t ds1307_dec_to_bcd(uint8_t dec)
{
    return ((dec / 10) << 4) | (dec % 10);
}

uint8_t ds1307_set_time(const DS1307_Time *time)
{
    if(i2c_start((DS1307_ADDRESS << 1) | 0) == I2C_ERROR) // Write
    {
        i2c_stop();
        return 0;
    }
    if(i2c_write(0x00) == I2C_ERROR)
    {
        i2c_stop();
        return 0;
    }
    // Write time
    if(i2c_write(ds1307_dec_to_bcd(time -> second)) == I2C_ERROR)
    {
        i2c_stop();
        return 0;
    }
    if (i2c_write(ds1307_dec_to_bcd(time->minute)) == I2C_ERROR)
    {
        i2c_stop();
        return 0;
    }
    if (i2c_write(ds1307_dec_to_bcd(time->hour)) == I2C_ERROR)
    {
        i2c_stop();
        return 0;
    }
    if (i2c_write(ds1307_dec_to_bcd(time->day)) == I2C_ERROR)
    {
        i2c_stop();
        return 0;
    }
    if (i2c_write(ds1307_dec_to_bcd(time->date)) == I2C_ERROR)
    {
        i2c_stop();
        return 0;
    }
    if (i2c_write(ds1307_dec_to_bcd(time->month)) == I2C_ERROR)
    {
        i2c_stop();
        return 0;
    }
    if (i2c_write(ds1307_dec_to_bcd(time->year)) == I2C_ERROR)
    {
        i2c_stop();
        return 0;
    }
    i2c_stop();

    return 1;
}

uint8_t ds1307_get_time(DS1307_Time *time)
{
    uint8_t data;
    // start + ds1307 write
    if(i2c_start((DS1307_ADDRESS << 1) | 0) == I2C_ERROR)
    {
        i2c_stop();
        return 0;
    }
    if(i2c_write(0x00) == I2C_ERROR)
    {
        i2c_stop();
        return 0;
    }
    // Repeated start + ds1307 read
    if(i2c_start((DS1307_ADDRESS << 1) | 1) == I2C_ERROR)
    {
        i2c_stop();
        return 0;
    }
    // second
    data = i2c_read_ack();
    time -> second = ds1307_bcd_to_dec(data & 0x7f); // bit 7 la clock halt => bo
    // Minutes
    data = i2c_read_ack();
    time->minute = ds1307_bcd_to_dec(data);
    // Hours
    data = i2c_read_ack();
    time->hour = ds1307_bcd_to_dec(data & 0x3F); // 24 hour mode
    // Day
    data = i2c_read_ack();
    time->day = ds1307_bcd_to_dec(data);
    // Date
    data = i2c_read_ack();
    time->date = ds1307_bcd_to_dec(data);
    // Month
    data = i2c_read_ack();
    time->month = ds1307_bcd_to_dec(data);
    // Year - last byte
    data = i2c_read_nack();
    time->year = ds1307_bcd_to_dec(data);

    i2c_stop();

    return 1;
}