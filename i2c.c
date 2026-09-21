#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/twi.h>
#include "i2c.h"

#define I2C_FREQ 100000UL
// TW: Two wire interface
// TWBR: bit rate register
// TWDR: Data register
// TWCR: Control register
// TWEN: Enable
// TWSTA: Start condition
// TWSTO: Stop condition
// TWINT: Interrupt flag
// TWEA: Enable acknowledge
// TWSR: Status register
void i2c_init(void)
{
    TWSR = 0x00;

    TWBR = ((F_CPU / I2C_FREQ) - 16) / 2;
    // Tao xung clk 100Khz
    TWCR = (1 << TWEN);
}

uint8_t i2c_start(uint8_t address)
{
    uint8_t status;
    TWCR = (1 << TWINT) |
           (1 << TWSTA) |
           (1 << TWEN);

    while (!(TWCR & (1 << TWINT)));

    status = TWSR & 0xf8;
    if((status != TW_START) && (status != TW_REP_START))
        return I2C_ERROR;

    TWDR = address;
    TWCR = (1 << TWINT) |
           (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));

    status = TWSR & 0xF8; // 5 BIT CAO la status
    // Check ACK depending on Write or Read
    if ((address & 0x01) == 0) // 0 la write, 1 la read
    {
        // SLA + W
        if (status != TW_MT_SLA_ACK) // Master transmit
        {
            return I2C_ERROR;
        }
    }
    else
    {
        // SLA + R
        if (status != TW_MR_SLA_ACK) // Master receive
        {
            return I2C_ERROR;
        }
    }
    return I2C_OK;
}

void i2c_stop(void)
{
    TWCR = (1 << TWINT) |
           (1 << TWSTO) |
           (1 << TWEN);
}

uint8_t i2c_write(uint8_t data)
{
    uint8_t status;

    TWDR = data;
    TWCR = (1 << TWINT) |
           (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));

    status = TWSR & 0xf8;
    if(status != TW_MT_DATA_ACK)
        return I2C_ERROR;
    return I2C_OK;
}

uint8_t i2c_read_ack(void)
{
    TWCR = (1 << TWINT) |
           (1 << TWEN) |
           (1 << TWEA);

    while (!(TWCR & (1 << TWINT)));

    return TWDR;
}

uint8_t i2c_read_nack(void)
{
    TWCR = (1 << TWINT) |
           (1 << TWEN);

    while (!(TWCR & (1 << TWINT)));

    return TWDR;
}