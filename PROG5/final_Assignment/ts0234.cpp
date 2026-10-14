#include "ST0243.hpp"

ST0243::ST0243(I2cBus& bus, uint8_t address)
    : bus(bus),
      address(address),
      dig_T1(0), dig_T2(0), dig_T3(0),
      dig_P1(0), dig_P2(0), dig_P3(0),
      dig_P4(0), dig_P5(0), dig_P6(0),
      dig_P7(0), dig_P8(0), dig_P9(0),
      tFine(0),
      initialized(false)
{
}

// Leest een reeks registers vanaf een registeradres.
bool ST0243::readRegisters(
    uint8_t reg,
    uint8_t* data,
    uint8_t length)
{
    if (data == nullptr || length == 0)
        return false;

    return bus.writeRead(
        address,
        &reg,
        1,
        data,
        length
    ) == 0;
}

// Initialiseert de sensor en leest de kalibratiegegevens.
bool ST0243::begin()
{
    initialized = false;

    // Controleer het chip-ID van de BMP280.
    uint8_t chipId = 0;

    if (!readRegisters(0xD0, &chipId, 1))
        return false;

    if (chipId != 0x58)
        return false;

    // Lees de 24 bytes kalibratiegegevens.
    uint8_t calib[24];

    if (!readRegisters(0x88, calib, sizeof(calib)))
        return false;

    // Kalibratiewaarden zijn little-endian.
    dig_T1 = static_cast<uint16_t>(
        calib[0] | (calib[1] << 8));

    dig_T2 = static_cast<int16_t>(
        calib[2] | (calib[3] << 8));

    dig_T3 = static_cast<int16_t>(
        calib[4] | (calib[5] << 8));

    dig_P1 = static_cast<uint16_t>(
        calib[6] | (calib[7] << 8));

    dig_P2 = static_cast<int16_t>(
        calib[8] | (calib[9] << 8));

    dig_P3 = static_cast<int16_t>(
        calib[10] | (calib[11] << 8));

    dig_P4 = static_cast<int16_t>(
        calib[12] | (calib[13] << 8));

    dig_P5 = static_cast<int16_t>(
        calib[14] | (calib[15] << 8));

    dig_P6 = static_cast<int16_t>(
        calib[16] | (calib[17] << 8));

    dig_P7 = static_cast<int16_t>(
        calib[18] | (calib[19] << 8));

    dig_P8 = static_cast<int16_t>(
        calib[20] | (calib[21] << 8));

    dig_P9 = static_cast<int16_t>(
        calib[22] | (calib[23] << 8));

    // Controleer of de drukkalibratie geldig is.
    if (dig_P1 == 0)
        return false;

    // Configuratie: temperatuur x1, druk x1, normal mode.
    uint8_t control[] = {0xF4, 0x27};

    if (bus.write(address, control, sizeof(control)) != 0)
        return false;

    initialized = true;
    return true;
}

// Leest en compenseert de temperatuur in graden Celsius.
bool ST0243::readTemperature(float& temperature)
{
    if (!initialized)
        return false;

    // Lees temperatuur en druk in één burst.
    uint8_t reg = 0xF7;
    uint8_t data[6];

    if (bus.writeRead(
            address, &reg, 1, data, sizeof(data)) != 0)
        return false;

    uint32_t adcP =
        (static_cast<uint32_t>(data[0]) << 12) |
        (static_cast<uint32_t>(data[1]) << 4) |
        (data[2] >> 4);

    uint32_t adcT =
        (static_cast<uint32_t>(data[3]) << 12) |
        (static_cast<uint32_t>(data[4]) << 4) |
        (data[5] >> 4);

    if (adcT == 0x80000 || adcP == 0x80000)
        return false;

    double var1 =
        (static_cast<double>(adcT) / 16384.0
         - static_cast<double>(dig_T1) / 1024.0)
        * dig_T2;

    double var2 =
        (static_cast<double>(adcT) / 131072.0
         - static_cast<double>(dig_T1) / 8192.0);

    var2 = var2 * var2 * dig_T3;

    tFine = static_cast<int32_t>(var1 + var2);

    temperature = static_cast<float>(
        (var1 + var2) / 5120.0);

    return true;
}

// Leest en compenseert de luchtdruk in Pascal.
bool ST0243::readPressure(float& pressure)
{
    if (!initialized)
        return false;

    // Lees temperatuur en druk samen voor een consistente meting.
    uint8_t reg = 0xF7;
    uint8_t data[6];

    if (bus.writeRead(
            address, &reg, 1, data, sizeof(data)) != 0)
        return false;

    uint32_t adcP =
        (static_cast<uint32_t>(data[0]) << 12) |
        (static_cast<uint32_t>(data[1]) << 4) |
        (data[2] >> 4);

    uint32_t adcT =
        (static_cast<uint32_t>(data[3]) << 12) |
        (static_cast<uint32_t>(data[4]) << 4) |
        (data[5] >> 4);

    if (adcT == 0x80000 || adcP == 0x80000)
        return false;

    // Bepaal tFine opnieuw op basis van de huidige temperatuur.
    double var1 =
        (static_cast<double>(adcT) / 16384.0
         - static_cast<double>(dig_T1) / 1024.0)
        * dig_T2;

    double var2 =
        (static_cast<double>(adcT) / 131072.0
         - static_cast<double>(dig_T1) / 8192.0);

    var2 = var2 * var2 * dig_T3;

    tFine = static_cast<int32_t>(var1 + var2);

    // Drukcompensatie volgens de BMP280-formule.
    double pVar1 = static_cast<double>(tFine) / 2.0 - 64000.0;

    double pVar2 = pVar1 * pVar1 * dig_P6 / 32768.0;
    pVar2 += pVar1 * dig_P5 * 2.0;
    pVar2 = pVar2 / 4.0 + dig_P4 * 65536.0;

    pVar1 =
        (dig_P3 * pVar1 * pVar1 / 524288.0
         + dig_P2 * pVar1) / 524288.0;

    pVar1 = (1.0 + pVar1 / 32768.0) * dig_P1;

    if (pVar1 == 0.0)
        return false;

    double p = 1048576.0 - adcP;
    p = (p - pVar2 / 4096.0) * 6250.0 / pVar1;

    pVar1 = dig_P9 * p * p / 2147483648.0;
    pVar2 = p * dig_P8 / 32768.0;

    p += (pVar1 + pVar2 + dig_P7) / 16.0;

    pressure = static_cast<float>(p);
    return true;
}