//
// Created by lz on 12/4/16.
//


#include "engine_factory.hpp"
#include <string>

namespace engines
{
    const char *EngineFactory::ENGINE_NAMES[] = {"ExampleEngine", nullptr};

    std::unique_ptr<gtp::IEngine> EngineFactory::create(const std::string &engineName)
    {
        if (engineName == "ExampleEngine")
            return std::unique_ptr<gtp::IEngine>(new engines::ExampleEngine);
        return nullptr;
    }

    const char *EngineFactory::getDefaultEngineName() {
        return ENGINE_NAMES[0];
    }
}
