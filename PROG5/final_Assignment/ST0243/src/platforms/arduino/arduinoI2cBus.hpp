#ifndef ARDUINO_I2C_BUS_H
#define ARDUINO_I2C_BUS_H

#include "i2cBus.hpp"

class ArduinoI2cBus : public I2cBus {
public:

    bool begin();

    int write
    (
        uint8_t address,
        const uint8_t* data,
        size_t length
    ) override; 

    int writeRead
    (
        uint8_t address,
        const uint8_t* txData,
        size_t txLength,
        uint8_t* rxData,
        size_t rxLength
    ) override;
};

#endif