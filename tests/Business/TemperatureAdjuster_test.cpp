#include <PiFan/Business/TemperatureAdjuster.h>
#include <gtest/gtest.h>


class AdjusterTest : public ::testing::Test
{
protected:
    AdjusterTest() : adjuster(PiFan::PiFanController::create()) {}

    PiFan::TemperatureAdjuster adjuster;
    PiFan::FanThrottlePercent throttle{ 0 };
};

// Over 80 degress the raspberry stars to throttle
// So the Fan should be running in full
TEST_F(AdjusterTest, ActivateFullFan)
{
    for (int i = 1; i < 100; ++i) {
        throttle = adjuster.adjust(80 * units::isq::si::thermodynamic_temperature_references::deg_C);
    }

    EXPECT_GT(throttle, PiFan::FanThrottlePercent(99));
}

// Under 50 degress probably does not make sense to have the
// fan working
TEST_F(AdjusterTest, DeactivateFan)
{
    for (int i = 1; i < 100; ++i) {
        throttle = adjuster.adjust(50 * units::isq::si::thermodynamic_temperature_references::deg_C);
    }

    EXPECT_EQ(throttle, PiFan::FanThrottlePercent(0));
}


// Testing that after 70 degrees the fan is starting to work
TEST_F(AdjusterTest, FanIsThrottling)
{
    for (int i = 1; i < 100; ++i) {
        throttle = adjuster.adjust(75 * units::isq::si::thermodynamic_temperature_references::deg_C);
    }

    EXPECT_GT(throttle, PiFan::FanThrottlePercent(10));
}
