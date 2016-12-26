#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <utility>

#include "logger.hpp"
#include <boost/program_options.hpp>
#include <gtplib/gtpfrontend.hpp>
#include "engines/engine_factory.hpp"


struct ParsedResult
{
    std::string engineName;
    std::vector<std::string> subargs;
};

ParsedResult parseCmd(int argc, char *argv[])
{
    ParsedResult result;
    namespace po = boost::program_options;
    po::options_description global("Global options");
    global.add_options()
            ("help,h", "print help message")
            ("enginelist,s", "Print all engines")
            ("loglevel,l", po::value<int>()->default_value(2), "Log level(smaller <-> more detailed)")
            ("engine,e", po::value<std::string>(&result.engineName)->
                    default_value(engines::EngineFactory::getDefaultEngineName()), "Engine used")
            ("subargs", po::value<std::vector< std::string >>(&result.subargs), "Arguments for engines")
            ;
    po::positional_options_description pos;
    po::variables_map vm;

    po::parsed_options parsed = po::command_line_parser(argc, argv).
            options(global).
            allow_unregistered().
            run();

    po::store(parsed, vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << global << "\n";
        std::exit(EXIT_SUCCESS);
    }

    if (vm.count("enginelist"))
    {
        std::cout << "All engines:" << std::endl;
        for (std::size_t i = 0; engines::EngineFactory::ENGINE_NAMES[i]; ++i)
            std::cout << engines::EngineFactory::ENGINE_NAMES[i] << std::endl;
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
    auto engine = engineFactory.create(cmd.engineName, cmd.subargs);
    if (engine == nullptr)
    {
        logger->critical("Cannot found matching engine {}", cmd.engineName);
        std::exit(EXIT_FAILURE);
    }
    logger->info("Default engine is {}", cmd.engineName);

    //------
    logger->info("Start running engine...");
    gtp::EngineFrontend<gtp::IEngine> frontend (std::cin, std::cout, *engine);
    frontend.start();



}
