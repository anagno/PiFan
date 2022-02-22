#include <PiFan/Service/PiFanController.h>
#include <gtest/gtest.h>


// We check that memory is released correct and
// we are not keeping the PiFanController in memory longer
// that it is needed
TEST(PiFanControllerMemoryTest, MemoryTest)
{
    std::shared_ptr<PiFan::PiFanController> controller;
    EXPECT_EQ(controller.use_count(), 0);

    {
        controller = PiFan::PiFanController::create();
        EXPECT_EQ(controller.use_count(), 1);

        // Force the scope on purpose
        {
            auto controller_2 = controller = PiFan::PiFanController::create();
            EXPECT_EQ(controller_2.use_count(), 2);
            EXPECT_EQ(controller.use_count(), 2);
        }
        EXPECT_EQ(controller.use_count(), 1);
    }

    controller.reset();
    EXPECT_EQ(controller.use_count(), 0);
}
