//
// Created by lz on 11/13/16.
//

#ifndef GO_AI_ENGINE_FACTORY_HPP
#define GO_AI_ENGINE_FACTORY_HPP
#include "common/logger.hpp"
#include "exampleEngine.hpp"
#include <gtplib/gtpengine.hpp>
#include <string>
#include <memory>

namespace engines
{
    class EngineFactory
    {
        decltype(getGlobalLogger()) logger = getGlobalLogger();
    public:
        // Returns engine based on their name. If no matching engines found, returns nullptr
        std::unique_ptr<gtp::IEngine> create(const std::string &engineName)
        {
            if (engineName == "ExampleEngine")
                return std::unique_ptr<gtp::IEngine>(new engines::ExampleEngine);
            return nullptr;
        }
    };
}

#endif //GO_AI_ENGINE_FACTORY_HPP
