#if defined(__linux__)

#include "LinuxI2cBus.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include <cerrno>
#include <limits>

LinuxI2cBus::LinuxI2cBus(const char* device)
    : device(device), fd(-1)
{
}

LinuxI2cBus::~LinuxI2cBus()
{
    if (fd >= 0) {
        close(fd);
    }
}

bool LinuxI2cBus::begin()
{
    if (fd >= 0) {
        return true;
    }

    fd = open(device.c_str(), O_RDWR);

    return fd >= 0;
}

int LinuxI2cBus::write(
    uint8_t address,
    const uint8_t* data,
    size_t length
)
{
    if (fd < 0) {
        return -1;
    }

    if (length > 0 && data == nullptr) {
        return -1;
    }

    if (length > static_cast<size_t>(
            std::numeric_limits<ssize_t>::max())) {
        return -1;
    }

    if (ioctl(fd, I2C_SLAVE, address) < 0) {
        return -1;
    }

    ssize_t written = ::write(fd, data, length);

    if (written < 0) {
        return -1;
    }

    if (static_cast<size_t>(written) != length) {
        return -1;
    }

    return 0;
}

int LinuxI2cBus::writeRead(
    uint8_t address,
    const uint8_t* txData,
    size_t txLength,
    uint8_t* rxData,
    size_t rxLength
)
{
    if (fd < 0) {
        return -1;
    }

    if ((txLength > 0 && txData == nullptr) ||
        (rxLength > 0 && rxData == nullptr)) {
        return -1;
    }

    if (txLength == 0 && rxLength == 0) {
        return 0;
    }

    if (txLength == 0) {
        return -1;
    }

    if (txLength > std::numeric_limits<__u16>::max() ||
        rxLength > std::numeric_limits<__u16>::max()) {
        return -1;
    }

    struct i2c_msg messages[2] = {};

    messages[0].addr  = address;
    messages[0].flags = 0;
    messages[0].len   = static_cast<__u16>(txLength);
    messages[0].buf   = const_cast<__u8*>(txData);

    int messageCount = 1;

    if (rxLength > 0) {
        messages[1].addr  = address;
        messages[1].flags = I2C_M_RD;
        messages[1].len   = static_cast<__u16>(rxLength);
        messages[1].buf   = rxData;

        messageCount = 2;
    }

    struct i2c_rdwr_ioctl_data transaction = {};
    transaction.msgs  = messages;
    transaction.nmsgs = messageCount;

    if (ioctl(fd, I2C_RDWR, &transaction) < 0) {
        return -1;
    }

    return 0;
}

#endif