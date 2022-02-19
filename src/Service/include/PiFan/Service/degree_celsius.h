#pragma once

#include <units/isq/si/thermodynamic_temperature.h>
#include <units/unit.h>

// TODO Fix when Celsius is properly supported (#232)
// https://mpusz.github.io/units/examples/kalman_filter/example_6.html

namespace units::isq::si {

struct degree_celsius : alias_unit<kelvin, basic_symbol_text{ "°C", "deg_C" }, no_prefix>
{
};

namespace thermodynamic_temperature_references {

    inline constexpr auto deg_C = reference<dim_thermodynamic_temperature, degree_celsius>{};


}// namespace thermodynamic_temperature_references

namespace references {

    using namespace thermodynamic_temperature_references;


}// namespace references
}// namespace units::isq::si