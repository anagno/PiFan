add_executable(
    TemperatureAdjuster_test
    TemperatureAdjuster_test.cpp)

target_link_libraries(
    TemperatureAdjuster_test
    PUBLIC PiFan::Business GTest::gtest_main)

add_test(
    NAME TemperatureAdjuster_test
    COMMAND TemperatureAdjuster_test)
