#pragma once

#include <units/generic/dimensionless.h>

#include <vector>
#include <atomic>

#include "degree_celsius.h"
#include "service_export.h"


namespace PiFan {

using FanThrottlePercent = units::dimensionless<units::percent>;

class SERVICE_EXPORT PiFanController
{
public:
    explicit PiFanController();
    ~PiFanController() noexcept;

    PiFanController(const PiFanController &other) noexcept = delete;
    PiFanController &operator=(const PiFanController &other) noexcept = delete;
    PiFanController(PiFanController &&other) noexcept;
    PiFanController &operator=(PiFanController &&other) noexcept;


    void setSpeed(FanThrottlePercent throttle);

private:
    unsigned int m_pwm_level = 0;

    // The initialization and de-initialization has to happen once.
    // So as long as we have still an instance alive, we should not
    // close the bcm2835 connection
    inline static std::atomic<int> m_counter = 0;
};

}// namespace PiFan