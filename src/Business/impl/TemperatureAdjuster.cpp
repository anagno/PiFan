#include "TemperatureAdjuster.h"

#include <units/quantity_cast.h>

namespace PiFan {

enum class State { HIGH_TEMPERATURE, RAMP_FAN, KICK_FAN_ON, LOW_TEMPERATURE };

constexpr auto THROTTLING_TEMP_FOR_RASP_PI = 80.0 * units::isq::si::thermodynamic_temperature_references::deg_C;
constexpr auto HIGH_TEMP_FOR_RAMPING_FAN = 73.0 * units::isq::si::thermodynamic_temperature_references::deg_C;
constexpr auto TEMP_FOR_STARTING_THE_FAN = 68.0 * units::isq::si::thermodynamic_temperature_references::deg_C;
constexpr auto TEMP_FOR_STOPPING_THE_FAN = 60.0 * units::isq::si::thermodynamic_temperature_references::deg_C;

TemperatureAdjuster::TemperatureAdjuster(PiFanController &&controller) : m_controller(std::move(controller)) {}

FanThrottlePercent TemperatureAdjuster::adjust(
  const units::isq::si::thermodynamic_temperature<units::isq::si::degree_celsius> &temperature)
{
    auto parse_state = [](const units::isq::si::thermodynamic_temperature<units::isq::si::degree_celsius> &temp) {
        if (temp >= THROTTLING_TEMP_FOR_RASP_PI) {
            return State::HIGH_TEMPERATURE;
        } else if (temp >= HIGH_TEMP_FOR_RAMPING_FAN) {
            return State::RAMP_FAN;
        } else if (temp >= TEMP_FOR_STARTING_THE_FAN) {
            return State::KICK_FAN_ON;
        } else {
            return State::LOW_TEMPERATURE;
        }
    };

    auto current_state = parse_state(temperature);

    auto set_throthle = [](const State &state,
                          const units::isq::si::thermodynamic_temperature<units::isq::si::degree_celsius> &temp) {
        switch (state) {
        case State::HIGH_TEMPERATURE: {
            return FanThrottlePercent(100);
        }
        case State::RAMP_FAN: {
            return units::quantity_cast<FanThrottlePercent>(
              (temp - TEMP_FOR_STOPPING_THE_FAN) / (THROTTLING_TEMP_FOR_RASP_PI - TEMP_FOR_STOPPING_THE_FAN));
        }
        case State::KICK_FAN_ON: {
            return FanThrottlePercent(200);
        }
        case State::LOW_TEMPERATURE:
            [[fallthrough]];
        default: {
            return FanThrottlePercent(0);
        }
        }
    };

    auto throttle = set_throthle(current_state, temperature);
    m_controller.setSpeed(throttle);
    return throttle;
}

}// namespace PiFan