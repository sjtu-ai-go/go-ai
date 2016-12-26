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
#include "board.hpp"
#include <numeric>
#include "logger.hpp"
#include "simple_engine_base.hpp"

namespace engines
{
    using namespace gtp;
    template<std::size_t BOARDSIZE>
    class RandEngine : public SimpleEngineBase {
    private:
        board::Board<BOARDSIZE, BOARDSIZE> board;
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
        RandEngine(const std::vector<std::string> &subargs)
        {}

        virtual std::string handle (const CmdName& cmd) override
        {
            logger->debug("Received CmdName");
            return "RandEngine";
        }
        virtual std::string handle (const CmdVersion& cmd) override
        {
            logger->debug("Received CmdVersion");
            return "1.0";
        }
        virtual void handle (const CmdBoardSize& cmd) override
        {
            std::size_t size = std::get<0>(cmd.params);
            logger->debug("Received boardsize: {}", size);
            if (size != decltype(board)::w)
            {
                logger->error("Unsupported size: {}", size);
                throw std::runtime_error("Unsupported size");
            }
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

            auto posVec = board.getAllGoodPosition(colorToPlayer(c));
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

        virtual ~RandEngine()
        {
            logger->debug("Example engine destroyed!");
        }
    };


}

#endif
