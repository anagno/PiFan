#include "PiFanController.h"

#include <bcm2835.h>

#include <exception>
#include <limits>
#include <stdexcept>

namespace {

constexpr int MAX_FAN = 480;

}// namespace

namespace PiFan {


PiFanController::PiFanController() : m_pwm_level{ std::numeric_limits<unsigned int>::min() }
{
#ifdef MOCK_BCM2835
    bcm2835_set_debug(1);
#endif
    if (!bcm2835_init()) throw std::runtime_error("Unable to initialize bcm2835");

    // two exposed GPIO pins are capable of hardware PWM (GPIO 13 and GPIO 18)
    // We use them both, so the user can connect his fan to any of these two pins
    bcm2835_gpio_fsel(13, BCM2835_GPIO_FSEL_ALT0);
    bcm2835_gpio_fsel(18, BCM2835_GPIO_FSEL_ALT5);

    bcm2835_pwm_set_clock(BCM2835_PWM_CLOCK_DIVIDER_2);
    bcm2835_pwm_set_mode(1, 1, 1);
    bcm2835_pwm_set_range(1, MAX_FAN);

    bcm2835_pwm_set_mode(0, 1, 1);
    bcm2835_pwm_set_range(0, MAX_FAN);
    setSpeed(FanThrottlePercent(0));
}

PiFanController::~PiFanController() noexcept { bcm2835_close(); }

void PiFanController::setSpeed(FanThrottlePercent throttle)
{
    auto level = static_cast<unsigned int>(throttle.number() / 100 * MAX_FAN);
    if (level > MAX_FAN) level = MAX_FAN;

    if (level > m_pwm_level && (level - m_pwm_level) < 5) return;
    if (level < m_pwm_level && (m_pwm_level - level) < 10) return;

    if (level != m_pwm_level) {
        bcm2835_pwm_set_data(1, level);
        bcm2835_pwm_set_data(0, level);
        m_pwm_level = level;
    }
}


}// namespace PiFan