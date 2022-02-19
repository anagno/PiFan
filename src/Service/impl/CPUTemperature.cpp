#include "CPUTemperature.h"

#include <fstream>

namespace PiFan {

units::isq::si::thermodynamic_temperature<units::isq::si::degree_celsius> getCurrentCPUTemperature()
{
    auto temperature_file = std::ifstream{ "/sys/class/thermal/thermal_zone0/temp", std::ios::in };

    // if something goes wrong just kill the program
    temperature_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    int cpu_temperature;
    temperature_file >> cpu_temperature;

    // The temperature is stored in 5 digits.
    // The first two are degrees in C.  The rest are decimal precision.
    return cpu_temperature / 1000 * units::isq::si::thermodynamic_temperature_references::deg_C;
}

}// namespace PiFan