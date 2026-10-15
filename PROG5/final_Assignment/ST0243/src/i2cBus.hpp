#ifndef I2C_BUS_H
#define I2C_BUS_H

#include <stddef.h>
#include <stdint.h>

class I2cBus
{
public:
    virtual ~I2cBus() = default;

    virtual bool begin() = 0;

    virtual int write(
        uint8_t address,
        const uint8_t* data,
        size_t length
    ) = 0;

    virtual int writeRead(
        uint8_t address,
        const uint8_t* txData,
        size_t txLength,
        uint8_t* rxData,
        size_t rxLength
    ) = 0;
};

#endif
