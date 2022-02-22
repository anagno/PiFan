add_executable(
    TemperatureFanController_test
    TemperatureFanController_test.cpp)

target_link_libraries(
    TemperatureFanController_test
    PUBLIC PiFan::Service GTest::gtest_main)

add_test(
    NAME TemperatureFanController_test
    COMMAND TemperatureFanController_test)
