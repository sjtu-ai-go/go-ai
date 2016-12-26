//
// Created by lz on 11/13/16.
//

#ifndef GO_AI_ENGINE_FACTORY_HPP
#define GO_AI_ENGINE_FACTORY_HPP
#include "logger.hpp"
#include "uct_engine.hpp"
#include "rand_engine.hpp"
#include <gtplib/gtpengine.hpp>
#include <string>
#include <memory>
#include <vector>

namespace engines
{
    class EngineFactory
    {
        decltype(getGlobalLogger()) logger = getGlobalLogger();
    public:
        static const char *ENGINE_NAMES[];
        // Returns engine based on their name. If no matching engines found, returns nullptr
        std::unique_ptr<gtp::IEngine> create(const std::string &engineName, const std::vector<std::string> &subargs);
        static const char *getDefaultEngineName();
    };
}

#endif //GO_AI_ENGINE_FACTORY_HPP
