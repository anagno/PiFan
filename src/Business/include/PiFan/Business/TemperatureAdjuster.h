#pragma once

#include "PiFan/Service/PiFanController.h"
#include "PiFan/Service/degree_celsius.h"
#include "business_export.h"

namespace PiFan {

class BUSINESS_EXPORT TemperatureAdjuster
{
public:
    TemperatureAdjuster(PiFanController &&controller);
    ~TemperatureAdjuster() noexcept = default;

    TemperatureAdjuster(const TemperatureAdjuster &other) noexcept = delete;
    TemperatureAdjuster &operator=(const TemperatureAdjuster &other) noexcept = delete;
    TemperatureAdjuster(TemperatureAdjuster &&other) noexcept = delete;
    TemperatureAdjuster &operator=(TemperatureAdjuster &&other) noexcept = delete;

    FanThrottlePercent adjust(
      const units::isq::si::thermodynamic_temperature<units::isq::si::degree_celsius> &temperature);

private:
    PiFanController m_controller;
};

}// namespace PiFan