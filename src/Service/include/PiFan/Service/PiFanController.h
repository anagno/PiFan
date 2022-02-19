#pragma once

#include "service_export.h"
#include "degree_celsius.h"

#include <units/generic/dimensionless.h>

#include <vector>


namespace PiFan
{

using FanThrottlePercent = units::dimensionless<units::percent>;

class SERVICE_EXPORT PiFanController
{
public:
    explicit PiFanController();
    ~PiFanController() noexcept;

    PiFanController(const PiFanController& other) noexcept = delete;
    PiFanController& operator=(const PiFanController& other) noexcept = delete;
    PiFanController(PiFanController&& other) noexcept = default;
    PiFanController& operator=(PiFanController&& other) noexcept = default;


    void setSpeed(FanThrottlePercent throttle);

private:
    int m_pwm_level = 0;
};

}  // namespace PiFan