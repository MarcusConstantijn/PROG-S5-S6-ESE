#include "I2C.h"
#include "ST0243.hpp"

#include <iostream>

int main()
{
    PlatformI2cBus bus;
    if (!bus.begin())
        return -1;

    ST0243 sensor(bus, 0x76);

    if (!sensor.begin())
    {
        bus.end();
        return -1;
    }

    float temperature;
    float pressure;

    if (sensor.readTemperature(temperature) && sensor.readPressure(pressure))
    {
        std::cout << "Temperatuur: "    << temperature << " °C\n";

        std::cout << "Luchtdruk: "      << pressure / 100.0f << " hPa\n";
    }

    bus.end();
    return 0;
}