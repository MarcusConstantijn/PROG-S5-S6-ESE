
#ifndef I2C_H
#define I2C_H

#if defined(ARDUINO)
    #include "ArduinoI2cBus.hpp"
    using GenericeI2cBus = ArduinoI2cBus;

#elif defined(__linux__)
    #include "LinuxI2cBus.hpp"
    using GenericeI2cBus = LinuxI2cBus;

#else
    #error "Unsupported platform"
#endif

#endif
