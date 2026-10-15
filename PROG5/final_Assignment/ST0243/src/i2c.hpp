
#ifndef I2C_H
#define I2C_H

#if defined(ARDUINO)
    #include "platforms/arduino/arduinoI2cBus.hpp"
    using GenericeI2cBus = ArduinoI2cBus;

#elif defined(__linux__)
    #include "platforms/linux/linuxI2cBus.hpp"
    using GenericeI2cBus = LinuxI2cBus;

#else
    #error "Unsupported platform"
#endif

#endif
