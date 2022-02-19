#pragma once

#include "degree_celsius.h"
#include "service_export.h"

namespace PiFan {
SERVICE_EXPORT units::isq::si::thermodynamic_temperature<units::isq::si::degree_celsius> getCurrentCPUTemperature();
}