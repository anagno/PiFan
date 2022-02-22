#include "TemperatureAdjuster.h"

#include <units/quantity_cast.h>
#include <units/math.h>

#include <cstdlib>

namespace PiFan {

// The Raspberry pi will start to throttle around 80 degrees. So let`s make certain that it will not throttle
// by setting the temperature a llitle bit lower
constexpr auto THROTTLING_TEMP_FOR_RASP_PI = 79.0 * units::isq::si::thermodynamic_temperature_references::deg_C;
constexpr auto HIGH_TEMP_FOR_RAMPING_FAN = 73.0 * units::isq::si::thermodynamic_temperature_references::deg_C;
constexpr auto TEMP_FOR_STARTING_THE_FAN = 70.0 * units::isq::si::thermodynamic_temperature_references::deg_C;
constexpr auto TEMP_FOR_STOPPING_THE_FAN = 69.0 * units::isq::si::thermodynamic_temperature_references::deg_C;

TemperatureAdjuster::TemperatureAdjuster(PiFanController &&controller)
  : m_current_state(State::LOW_TEMPERATURE),
    m_current_temperature(25 * units::isq::si::thermodynamic_temperature_references::deg_C),
    m_current_throttle(FanThrottlePercent(0)),
    m_controller(std::move(controller))
{}

FanThrottlePercent TemperatureAdjuster::adjust(
  const units::isq::si::thermodynamic_temperature<units::isq::si::degree_celsius> &temperature)
{
    // It is better to use a moving average of the temperature, so we will not be
    // reacting too much
    if (m_current_temperature < temperature) {
        m_current_temperature = 0.9 * m_current_temperature + 0.1 * temperature;
    } else {
        // But be less reactive when reducing the temperature
        m_current_temperature = 0.99 * m_current_temperature + 0.01 * temperature;
    }

    constexpr auto parse_state = [](const State &current_state,
                         const units::isq::si::thermodynamic_temperature<units::isq::si::degree_celsius> &temp) {
        if (temp >= THROTTLING_TEMP_FOR_RASP_PI) {
            return State::HIGH_TEMPERATURE;
        } else if (temp >= HIGH_TEMP_FOR_RAMPING_FAN) {
            return State::RAMP_FAN;
        } else if (temp >= TEMP_FOR_STARTING_THE_FAN) {
            return State::KICK_FAN_ON;
        } else {
            if ((State::KICK_FAN_ON == current_state || State::RAMP_FAN == current_state)
                && temp >= TEMP_FOR_STOPPING_THE_FAN) {
                return State::KICK_FAN_ON;
            } else {
                return State::LOW_TEMPERATURE;
            }
        }
    };

    auto new_state = parse_state(m_current_state, m_current_temperature);

    constexpr auto set_throttle = [](const State &state,
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
            return FanThrottlePercent(20);
        }
        case State::LOW_TEMPERATURE:
            [[fallthrough]];
        default: {
            return FanThrottlePercent(0);
        }
        }
    };

    auto new_throttle = set_throttle(new_state, m_current_temperature);
    if(units::abs(new_throttle - m_current_throttle) > FanThrottlePercent(1))
    {
        m_current_throttle = new_throttle;
        m_controller.setSpeed(m_current_throttle);
    }

    m_current_state = new_state;
    return m_current_throttle;
}

}// namespace PiFan