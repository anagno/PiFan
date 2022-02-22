#include <PiFan/Business/TemperatureAdjuster.h>
#include <PiFan/Service/CPUTemperature.h>
#include <PiFan/Service/PiFanController.h>
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <prometheus/counter.h>
#include <prometheus/exposer.h>
#include <prometheus/registry.h>
#include <units/format.h>

#include <boost/program_options.hpp>
#include <chrono>
#include <thread>

#include "config.h"


int main(int argc, char *argv[])
{

    auto argv_verbose = bool{ false };

    //////// Start of parsing of the command line arguments /////////////////
    {
        namespace boost_options = boost::program_options;
        auto option_description = boost_options::options_description( "\n" +
            PiFan::getProjectName() + ": " + PiFan::getProjectVersion() + "\n" +
            "Usage: \n"
            "pi_fan_hwpwm [options]\n"
            "Options");

        option_description.add_options()("help,h", "Display this help message")(
          "version,v", "Display the version number")("verbose", "Verbose output");

        auto option_variables = boost_options::variables_map{};
        boost_options::store(
          boost_options::command_line_parser(argc, argv).options(option_description).run(), option_variables);

        if (option_variables.count("help")) {
            fmt::print("{} \n", option_description);
            return EXIT_SUCCESS;
        }

        if (option_variables.count("version")) {
            fmt::print("{}: {}\n", PiFan::getProjectName(), PiFan::getProjectVersion());
            return EXIT_SUCCESS;
        }

        if (option_variables.count("verbose")) { argv_verbose = true; }
    }
    //////// End of parsing of the command line arguments /////////////////

    // TODO: Maybe it is not the most idea. First of all it is a hard coded port
    // Second of all, it is accessible from outside the machine.
    // To see the how to change it take a look at
    // https://github.com/civetweb/civetweb/blob/master/docs/UserManual.md#listening_ports-8080
    auto metric_exposer = prometheus::Exposer{ "8456,[::]:8456" };
    auto metric_registry = std::make_shared<prometheus::Registry>();
    auto &temperature_gauge = prometheus::BuildGauge()
                                .Name("fan_measured_temperature")
                                .Help("The temperature as measured from the fan program")
                                .Register(*metric_registry);

    auto &temperature_metric = temperature_gauge.Add({ { "node_temperature", "temp in degrees" } });

    auto &fan_speed_gauge = prometheus::BuildGauge()
                              .Name("fan_measured_speed")
                              .Help(
                                "The speed of the fan in percentage (i.e. 100% means full "
                                "speed and 0% the fan is stopeed")
                              .Register(*metric_registry);

    auto &fan_speed_metric = fan_speed_gauge.Add({ { "node_fan_throttle", "percentage of the usage of the fan" } });

    metric_exposer.RegisterCollectable(metric_registry);

    auto adjuster = PiFan::TemperatureAdjuster{ PiFan::PiFanController{} };

    while (true) {
        auto temperature = PiFan::getCurrentCPUTemperature();
        temperature_metric.Set(temperature.number());

        auto fan_throttle = adjuster.adjust(temperature);
        fan_speed_metric.Set(fan_throttle.number());

        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (argv_verbose) fmt::print("{}, {}\n", temperature, fan_throttle);
    }

    return EXIT_SUCCESS;
}