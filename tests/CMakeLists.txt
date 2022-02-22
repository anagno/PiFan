project(PiFanTests)

find_package(GTest REQUIRED CONFIG)

enable_testing()
add_subdirectory(Business)
add_subdirectory(Service)
