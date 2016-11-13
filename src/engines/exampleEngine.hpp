#ifndef EXAMPLE_ENGINE_HPP
#define EXAMPLE_ENGINE_HPP
#include <gtplib/gtpengine.hpp>
#include "common/board.hpp"
#include "common/logger.hpp"

namespace engines
{
    using namespace gtp;
    class ExampleEngine: public gtp::IEngine {
    private:
        decltype(getGlobalLogger()) logger = getGlobalLogger();
    public:
        ExampleEngine() = default;

        virtual int handle(const CmdProtocolVersion &cmd) override
        {
            logger->info("Received CmdProtocolVersion command");

        }
        virtual std::string handle (const CmdName& cmd) override
        {

        }
        virtual std::string handle (const CmdVersion& cmd) override
        {

        }
        virtual bool handle (const CmdKnownCommand& cmd) override
        {

        }
        virtual std::list<CommandType> handle (const CmdListCommands& cmd) override
        {

        }
        virtual void handle (const CmdQuit& cmd) override
        {

        }
        virtual void handle (const CmdBoardSize& cmd) override
        {

        }
        virtual void handle (const CmdClearBoard& cmd) override
        {

        }
        virtual void handle (const CmdKomi& cmd) override
        {

        }
        virtual VertexOrPass handle (const CmdGenmove& cmd) override
        {

        }
        virtual void handle (const CmdPlay& cmd) override
        {

        }
        virtual std::list<Vertex> handle (const CmdFixedHandicap& cmd) override
        {

        }
        virtual std::list<Vertex> handle (const CmdPlaceFreeHandicap& cmd) override
        {

        }
        virtual void handle (const CmdSetFreeHandicap& cmd) override
        {

        }
        virtual void handle (const CmdTimeSettings& cmd) override
        {

        }
        virtual void handle (const CmdTimeLeft& cmd) override
        {

        }
        virtual void handle (const CmdFinalScore& cmd) override
        {

        }
        virtual std::list<Vertex> handle (const CmdFinalStatusList& cmd) override
        {

        }

        virtual ~ExampleEngine()
        {
            logger->debug("Example engine destroyed!");
        }
    };
}

#endif
