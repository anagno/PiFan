#include <PiFan/Business/TemperatureAdjuster.h>
#include <gtest/gtest.h>


class AdjusterTest : public ::testing::Test
{
protected:
    AdjusterTest() : adjuster(PiFan::PiFanController{}) {}

    PiFan::TemperatureAdjuster adjuster;
};

// Over 80 degress the raspberry stars to throttle
// So the Fan should be running in full
TEST_F(AdjusterTest, ActivateFullFan)
{
    auto fan_throttle = adjuster.adjust(80 * units::isq::si::thermodynamic_temperature_references::deg_C);

    EXPECT_EQ(fan_throttle, PiFan::FanThrottlePercent(100));
}

// Under 50 degress probably does not make sense to have the
// fan working
TEST_F(AdjusterTest, DeactivateFan)
{
    auto fan_throttle = adjuster.adjust(50 * units::isq::si::thermodynamic_temperature_references::deg_C);

    EXPECT_EQ(fan_throttle, PiFan::FanThrottlePercent(0));
}


// Testing that after 70 degrees the fan is starting to work
TEST_F(AdjusterTest, FanIsThrottling)
{
    auto fan_throttle = adjuster.adjust(75 * units::isq::si::thermodynamic_temperature_references::deg_C);

    EXPECT_GT(fan_throttle, PiFan::FanThrottlePercent(20));
}
