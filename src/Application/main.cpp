#include "config.h"

#include <PiFan/Service/CPUTemperature.h>
#include <PiFan/Service/PiFanController.h>
#include <PiFan/Business/TemperatureAdjuster.h>

#include <prometheus/counter.h>
#include <prometheus/exposer.h>
#include <prometheus/registry.h>

#include <units/format.h>
#include <fmt/core.h>
#include <fmt/ostream.h>


#include <boost/program_options.hpp>

#include <thread>
#include <chrono>


int main(int argc, char* argv[])
{

    auto argv_verbose = bool{false};

    //////// Start of parsing of the command line arguments /////////////////
    namespace boost_options = boost::program_options;
    auto option_description = boost_options::options_description( "\n" +
    PiFan::getProjectName() + ": " + PiFan::getProjectVersion() + "\n" +
    "Usage: \n"
    "pi_fan_hwpwm [options]\n"
    "Options");

    option_description.add_options()("help,h", "Display this help message")(
        "version,v", "Display the version number")
        ("verbose", "Verbose output");

    auto option_variables = boost_options::variables_map{};
    boost_options::store(boost_options::command_line_parser(argc, argv).options(option_description).run(), option_variables);

    if(option_variables.count("help"))
    {
        fmt::print("{} \n", option_description);
        return EXIT_SUCCESS;
    }

    if(option_variables.count("version"))
    {
        fmt::print("{}: {}\n", PiFan::getProjectName(), PiFan::getProjectVersion());
        return EXIT_SUCCESS;
    }

    if(option_variables.count("verbose"))
    {
        argv_verbose = true;
    }

    //////// End of parsing of the command line arguments /////////////////

    auto metric_exposer = prometheus::Exposer{"127.0.0.1:8080"};
    auto metric_registry = std::make_shared<prometheus::Registry>();
    auto& temperature_metric = prometheus::BuildGauge()
                                   .Name("fan_measured_temperature")
                                   .Help("The temperature as measured from the fan program")
                                   .Register(*metric_registry);

    auto& fan_speed_metric = prometheus::BuildGauge()
                                 .Name("fan_measured_speed")
                                 .Help(
                                     "The speed of the fan in percentage (i.e. 100% means full "
                                     "speed and 0% the fan is stopeed")
                                 .Register(*metric_registry);

    auto temperature = PiFan::getCurrentCPUTemperature();
    auto adjuster = PiFan::TemperatureAdjuster{PiFan::PiFanController{}};

    while(true)
    {
        // It is better to use a moving average of the temperature, so we will not be 
        // reacting too much
        temperature = 0.1 * PiFan::getCurrentCPUTemperature() + 0.9 * temperature;
        auto fan_throttle = adjuster.adjust(temperature);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if(argv_verbose) fmt::print("{}, {}\n", temperature, fan_throttle);
    }

    return EXIT_SUCCESS;
}