#include "engines/exampleEngine.hpp"
#include "common/logger.hpp"
#include <boost/program_options.hpp>
#include <engines/engine_factory.hpp>

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <utility>

struct ParsedResult
{
    std::string engineName;
};

ParsedResult parseCmd(int argc, char *argv[])
{
    ParsedResult result;
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help,h", "print help message")
            ("loglevel,l", po::value<int>()->default_value(2), "Log level(smaller <-> more detailed)")
            ("engine,e", po::value<std::string>(&result.engineName)->default_value(std::string("ExampleEngine")), "Engine used")
            ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << "\n";
        std::exit(EXIT_SUCCESS);
    }

    auto logger = getGlobalLogger();
    logger->set_level(spdlog::level::level_enum(vm["loglevel"].as<int>()));

    logger->info("log level is: {0}", spdlog::level::to_str(logger->level()));
    return result;
}

int main(int argc, char *argv[])
{
    auto logger = getGlobalLogger();

    ParsedResult cmd = parseCmd(argc, argv);
    engines::EngineFactory engineFactory;
    auto engine = engineFactory.create(cmd.engineName);
    if (engine == nullptr)
    {
        logger->critical("Cannot found matching engine {}", cmd.engineName);
        std::exit(EXIT_FAILURE);
    }
    logger->info("Default engine is {}", cmd.engineName);

}
