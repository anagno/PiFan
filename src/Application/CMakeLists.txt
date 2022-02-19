find_package(Boost CONFIG REQUIRED)
find_package(mp-units CONFIG REQUIRED)
find_package(prometheus-cpp CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

configure_file(config.h.in config.h @ONLY)

add_executable(pi_fan_hwpwm main.cpp 
  ${CMAKE_CURRENT_BINARY_DIR}/config.h)

target_include_directories(pi_fan_hwpwm PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(pi_fan_hwpwm PRIVATE
  PiFan::Service
  PiFan::Business
  Boost::program_options
  mp-units::core
  fmt::fmt
  prometheus-cpp::prometheus-cpp-push
  prometheus-cpp::prometheus-cpp-pull)

install(TARGETS pi_fan_hwpwm
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")


