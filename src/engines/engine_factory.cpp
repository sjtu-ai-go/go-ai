//
// Created by lz on 12/4/16.
//


#include "engine_factory.hpp"
#include <string>

namespace engines
{
    const char *EngineFactory::ENGINE_NAMES[] = {"RandEngine19x19", "RandEngine9x9", nullptr};

    std::unique_ptr<gtp::IEngine> EngineFactory::create(const std::string &engineName)
    {
        if (engineName == "RandEngine19x19")
            return std::unique_ptr<gtp::IEngine>(new engines::RandEngine<19>);
        if (engineName == "RandEngine9x9")
            return std::unique_ptr<gtp::IEngine>(new engines::RandEngine<9>);
        return nullptr;
    }

    const char *EngineFactory::getDefaultEngineName() {
        return ENGINE_NAMES[0];
    }
}
