#pragma once

#include <units/generic/dimensionless.h>

#include <memory>
#include <vector>

#include "degree_celsius.h"
#include "service_export.h"


namespace PiFan {

using FanThrottlePercent = units::dimensionless<units::percent>;

class SERVICE_EXPORT PiFanController
{
public:
    static std::shared_ptr<PiFanController> create();

    ~PiFanController() noexcept;
    PiFanController(const PiFanController &other) noexcept = delete;
    PiFanController &operator=(const PiFanController &other) noexcept = delete;
    PiFanController(PiFanController &&other) noexcept = delete;
    PiFanController &operator=(PiFanController &&other) noexcept = delete;


    void setSpeed(FanThrottlePercent throttle);

private:
    explicit PiFanController();

    inline static std::weak_ptr<PiFanController> m_instance;

    unsigned int m_pwm_level = 0;
};

}// namespace PiFan