#pragma once

#include "service_export.h"
#include "degree_celsius.h"

namespace PiFan
{
SERVICE_EXPORT units::isq::si::thermodynamic_temperature<units::isq::si::degree_celsius> getCurrentCPUTemperature();
}