
#include "ArduinoI2cBus.hpp"
#include <Wire.h>

void ArduinoI2cBus::begin()
{
    Wire.begin();
}

int ArduinoI2cBus::write(
    uint8_t address,
    const uint8_t* data,
    size_t length
)
{
    if (length > 0 && data == nullptr) {
        return -1;
    }

    Wire.beginTransmission(address);

    size_t written = Wire.write(data, length);

    if (written != length) {
        Wire.endTransmission();
        return -2;
    }

    return Wire.endTransmission();
}

int ArduinoI2cBus::writeRead(
    uint8_t address,
    const uint8_t* txData,
    size_t txLength,
    uint8_t* rxData,
    size_t rxLength
)
{
    if ((txLength > 0 && txData == nullptr) ||
        (rxLength > 0 && rxData == nullptr)) {
        return -1;
    }

    if (rxLength == 0) {
        return write(address, txData, txLength);
    }
  
    Wire.beginTransmission(address);

    size_t written = Wire.write(txData, txLength);

    if (written != txLength) {
        Wire.endTransmission();
        return -2;
    }

    int status = Wire.endTransmission(false);

    if (status != 0) {
        return status;
    }

    size_t received = Wire.requestFrom(
        address,
        rxLength
    );

    if (received != rxLength) {
        while (Wire.available()) {
            Wire.read();
        }

        return -3;
    }

    for (size_t i = 0; i < rxLength; i++) {
        rxData[i] = Wire.read();
    }

    return 0;
}
