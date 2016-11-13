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
#include <common/board/basic.hpp>
#include <numeric>
#include "common/logger.hpp"

namespace engines
{
    using namespace gtp;
    class ExampleEngine: public gtp::IEngine {
    private:
        decltype(getGlobalLogger()) logger = getGlobalLogger();

        static const char* KNOWN_COMMANDS[9];
        std::vector< std::vector< board::PointState > > board;
        bool received_pass = false;
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
            std::exit(EXIT_SUCCESS);
        }
        virtual void handle (const CmdBoardSize& cmd) override
        {
            std::size_t size = std::get<0>(cmd.params);
            logger->debug("Received boardsize: {}", size);
            board.resize(size);
            for (auto &row: board)
                row.resize(size);
        }
        virtual void handle (const CmdClearBoard& cmd) override
        {
            logger->debug("Received clear_board");
            received_pass = false;
            std::for_each(std::begin(board), std::end(board), [](std::vector<board::PointState > &row ) {
                std::fill(row.begin(), row.end(), board::PointState::NA);
            });
        }
        virtual void handle (const CmdKomi& cmd) override
        {
            float komi = std::get<0>(cmd.params);
            logger->debug("Received komi {}", komi);
        }
        virtual VertexOrPass handle (const CmdGenmove& cmd) override
        {
            logger->debug("Received genmove");
            int cnt = std::accumulate(board.cbegin(), board.cend(), 0,
                                      [](int num, const std::vector<board::PointState> &row)
                                      {
                                          return num + std::count(row.cbegin(), row.cend(), board::PointState::NA);
                                      }
            );
            if (received_pass || cnt < 0.5 * board.size() * board.size()) return VertexOrPass();
            else
            {
                unsigned int x, y;
                do
                {
                    x = std::rand() % board.size();
                    y = std::rand() % board.size();
                } while (board[x][y] != board::PointState::NA);
                board[x][y] = board::PointState::B;
                return VertexOrPass(x, y + 1);
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
            if (move.vertex.type == VertexOrPassType::VERTEX)
            {
                Vertex v = (Vertex)(move.vertex);
                logger->debug("Received Play at ({}, {})", v.x, v.y - 1);
                board[v.x][v.y - 1] = board::PointState::W;
            }
            else
            {
                logger->debug("Received PASS");
                received_pass = true;
            }
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
            auto l = std::get<0>(cmd.params);
            std::for_each(l.begin(), l.end(), [&](const Vertex &v)
            {
                board[v.x][v.y] = board::PointState::W;
            });
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
