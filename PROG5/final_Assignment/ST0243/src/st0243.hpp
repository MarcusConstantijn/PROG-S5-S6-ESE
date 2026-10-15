#ifndef ST0243_HPP
#define ST0243_HPP

#include "I2cBus.hpp"
#include <stdint.h>

class ST0243
{
public:
    explicit ST0243(I2cBus& bus, uint8_t address = 0x76);

    bool begin();
    bool readTemperature(float& temperature);
    bool readPressure(float& pressure);

private:
    I2cBus& bus;
    uint8_t address;
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;

    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;

    int32_t tFine;
    bool initialized;
    
    bool readRegisters(
        uint8_t reg,
        uint8_t* data,
        uint8_t length
    );
};

#endif