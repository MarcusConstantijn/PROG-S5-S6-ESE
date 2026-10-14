#ifndef LINUX_I2C_H
#define LINUX_I2C_H
#include "i2cBus.hpp"


#pragma once

#include "I2cBus.hpp"
#include <string>

class LinuxI2cBus : public I2cBus {
public:
    explicit LinuxI2cBus(
        const char* device = "/dev/i2c-1"
    );

    ~LinuxI2cBus() override;

    bool begin();

    int write(
        uint8_t address,
        const uint8_t* data,
        size_t length
    ) override;

    int writeRead(
        uint8_t address,
        const uint8_t* txData,
        size_t txLength,
        uint8_t* rxData,
        size_t rxLength
    ) override;

    private:
        std::string device;
        int fd;
};


#endif