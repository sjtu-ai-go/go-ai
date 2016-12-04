//
// Created by lz on 12/4/16.
//

#ifndef GO_AI_SIMPLE_ENGINE_BASE_HPP
#define GO_AI_SIMPLE_ENGINE_BASE_HPP

#include <algorithm>
#include <iterator>
#include <string>
#include <tuple>
#include <list>
#include <vector>
#include <numeric>
#include <cstdlib>
#include <gtplib/gtpengine.hpp>
#include "common/board.hpp"
#include "common/logger.hpp"

namespace engines
{
    using namespace gtp;
    // This is a virtual base class of a minimal engine
    class SimpleEngineBase: public gtp::IEngine {
    protected:
        decltype(getGlobalLogger()) logger = getGlobalLogger();
        static const char* KNOWN_COMMANDS[11];
    public:
        SimpleEngineBase() = default;

        // Simple engine implementers only need to implement these commands:
        virtual std::string handle (const CmdName& cmd) = 0;
        virtual std::string handle (const CmdVersion& cmd) = 0;
        virtual void handle (const CmdBoardSize& cmd) = 0;
        virtual void handle (const CmdClearBoard& cmd) = 0;
        virtual void handle (const CmdKomi& cmd) = 0;
        virtual VertexOrPass handle (const CmdGenmove& cmd) = 0;
        virtual void handle (const CmdPlay& cmd) = 0;
        virtual void handle (const CmdTimeSettings& cmd) = 0;
        virtual void handle (const CmdTimeLeft& cmd) = 0;

        virtual int handle(const CmdProtocolVersion &cmd) override
        {
            logger->debug("Received CmdProtocolVersion command");
            return 2;
        }
        virtual bool handle (const CmdKnownCommand& cmd) override
        {
            std::string param = std::get<0>(cmd.params);
            logger->debug("Received KnownCommand: {}", param);
            return std::find(std::begin(KNOWN_COMMANDS), std::end(KNOWN_COMMANDS), param) != std::end(KNOWN_COMMANDS);
        }
        virtual std::list<CommandType> handle (const CmdListCommands& cmd) override
        {
            logger->debug("Received ListCommands");
            static const CommandType cts[] = {CommandType::clear_board, CommandType::boardsize, CommandType::komi,
                                              CommandType::genmove, CommandType::play, CommandType::protocol_version,
                                              CommandType::version, CommandType::name, CommandType::quit,
                                              CommandType::time_settings, CommandType::time_left};

            return std::list<CommandType>(std::begin(cts), std::end(cts));
        }
        virtual void handle (const CmdQuit& cmd) override
        {
            logger->debug("Received quit");
            // No need to std::exit here since GtpLib will stop after this command
        }
        virtual void handle (const CmdUndo &cmd) override
        {
            logger->debug("Received undo");
            throw std::runtime_error("Undo is unsupported");
        }
        virtual std::list<Vertex> handle (const CmdFixedHandicap& cmd) override
        {
            throw std::runtime_error("FixedHandicap not implemented yet");
        }
        virtual std::list<Vertex> handle (const CmdPlaceFreeHandicap& cmd) override
        {
            throw std::runtime_error("PlaceFreeHandicap not implemented yet");
        }
        virtual void handle (const CmdSetFreeHandicap& cmd) override
        {
            logger->debug("Received set free handicap");
            throw std::runtime_error("Unsupported SetFreeHandicap yet");
        }
        virtual void handle (const CmdFinalScore& cmd) override
        {
            Score s = std::get<0>(cmd.params);
            logger->debug("Received final score: Color: {}, Advantage: {}", (int)s.winner, s.advantage);
        }
        virtual std::list<Vertex> handle (const CmdFinalStatusList& cmd) override
        {
            throw std::runtime_error("CmdFinalStatusList not implemented yet");
        }

        virtual void handle(const CmdError &cmd) override
        {
            logger->debug("Received error!");
        }

        virtual ~SimpleEngineBase()
        {
        }
    };


}
#endif //GO_AI_SIMPLE_ENGINE_BASE_HPP
