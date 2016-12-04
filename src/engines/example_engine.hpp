#ifndef EXAMPLE_ENGINE_HPP
#define EXAMPLE_ENGINE_HPP
#include <algorithm>
#include <iterator>
#include <string>
#include <tuple>
#include <list>
#include <vector>
#include <cstdlib>
#include <gtplib/gtpengine.hpp>
#include "common/board.hpp"
#include <numeric>
#include "common/logger.hpp"

namespace engines
{
    using namespace gtp;
    class ExampleEngine: public gtp::IEngine {
    private:
        decltype(getGlobalLogger()) logger = getGlobalLogger();

        static const char* KNOWN_COMMANDS[9];
        board::Board<19, 19> board;
        bool received_pass = false;

        static board::Player colorToPlayer(Color c)
        {
            switch (c)
            {
                case Color::black:
                    return board::Player::B;
                case Color::white:
                    return board::Player::W;
            }
        }
    public:
        ExampleEngine() = default;
        virtual int handle(const CmdProtocolVersion &cmd) override
        {
            logger->debug("Received CmdProtocolVersion command");
            return 2;
        }
        virtual std::string handle (const CmdName& cmd) override
        {
            logger->debug("Received CmdName");
            return "ExampleEngine";
        }
        virtual std::string handle (const CmdVersion& cmd) override
        {
            logger->debug("Received CmdVersion");
            return "1.0";
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
                                              CommandType::version, CommandType::name, CommandType::quit};

            return std::list<CommandType>(std::begin(cts), std::end(cts));
        }
        virtual void handle (const CmdQuit& cmd) override
        {
            logger->debug("Received quit");
            // No need to std::exit here since GtpLib will stop after this command
        }
        virtual void handle (const CmdBoardSize& cmd) override
        {
            std::size_t size = std::get<0>(cmd.params);
            logger->debug("Received boardsize: {}", size);
            if (size != decltype(board)::w)
                logger->error("Unsupported size: {}", size);
        }
        virtual void handle (const CmdClearBoard& cmd) override
        {
            logger->debug("Received clear_board");
            received_pass = false;
            board.clear();
        }
        virtual void handle (const CmdKomi& cmd) override
        {
            float komi = std::get<0>(cmd.params);
            logger->debug("Received komi {}", komi);
        }
        virtual VertexOrPass handle (const CmdGenmove& cmd) override
        {
            logger->debug("Received genmove");
            Color c = std::get<0>(cmd.params);

            using PT = typename decltype(board)::PointType;

            auto posVec = board.getAllValidPosition(colorToPlayer(c));
            std::for_each(posVec.cbegin(), posVec.cend(), [&](PT p) {
                logger->trace("Valid pos: ({}, {})  ", (int)p.x, (int) p.y);
            });
            if (posVec.empty())
            {
                return Pass();
            } else
            {
                std::size_t idx = std::rand() % posVec.size();
                logger->debug("Choose to place at {},{}", (int)posVec[idx].x + 1, (int)posVec[idx].y + 1);
                board.place(posVec[idx], colorToPlayer(c));
                logger->debug("After genmove \n {}", board);
                return VertexOrPass(posVec[idx].x + 1, posVec[idx].y + 1);
            }
        }
        virtual void handle (const CmdUndo &cmd) override
        {
            logger->debug("Received undo");
            throw std::runtime_error("Undo is unsupported");
        }
        virtual void handle (const CmdPlay& cmd) override
        {
            Move move = std::get<0>(cmd.params);
            Color c = move.color;
            if (move.vertex.type == VertexOrPassType::VERTEX)
            {
                using PT = typename decltype(board)::PointType;
                Vertex v = (Vertex)(move.vertex);
                logger->debug("Received Play at ({}, {})", v.x -1 , v.y - 1);
                // Input is (1 to 19, 1 to 19)
                board.place(PT {(char)(v.x - 1), (char)(v.y -1)}, colorToPlayer(c));
            }
            else
            {
                logger->debug("Received PASS");
                received_pass = true;
            }
            logger->debug("After play \n {}", board);
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
        virtual void handle (const CmdTimeSettings& cmd) override
        {
            int t1 = std::get<0>(cmd.params), t2 = std::get<1>(cmd.params), t3 = std::get<2>(cmd.params);
            logger->debug("Received time setting: {}, {}, {}", t1, t2, t3);
        }
        virtual void handle (const CmdTimeLeft& cmd) override
        {
            Color c1 = std::get<0>(cmd.params), c2 = std::get<1>(cmd.params);
            int t1 = std::get<2>(cmd.params), t2 = std::get<3>(cmd.params);
            logger->debug("Received Time left: {} time for {}; {} time for {}", t1, (int)c1, t2, (int)c2);
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

        virtual ~ExampleEngine()
        {
            logger->debug("Example engine destroyed!");
        }
    };


}

#endif
