//
// Created by hjhhh on 12/19/2016.
//

#ifndef GO_AI_UCT_ENGINE_HPP_HPP
#define GO_AI_UCT_ENGINE_HPP_HPP

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
#include "uct/uct.hpp"
#include <chrono>

namespace engines
{
    using namespace gtp;
    template<std::size_t BOARDSIZE>
    class UctEngine : public SimpleEngineBase {
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

        float komi = 6.5;
    public:
        UctEngine() = default;
        virtual std::string handle(const CmdName& cmd) override
        {
            logger->debug("Received CmdName");
            return "UctEngine";
        }
        virtual std::string handle(const CmdVersion& cmd) override
        {
            logger->debug("Received CmdVersion");
            return "1.0";
        }
        virtual void handle(const CmdBoardSize& cmd) override
        {
            std::size_t size = std::get<0>(cmd.params);
            logger->debug("Received boardsize: {}", size);
            if (size != decltype(board)::w)
            {
                logger->error("Unsupported size: {}", size);
                throw std::runtime_error("Unsupported size");
            }
        }
        virtual void handle(const CmdClearBoard& cmd) override
        {
            logger->debug("Received clear_board");
            received_pass = false;
            board.clear();
        }
        virtual void handle(const CmdKomi& cmd) override
        {
            komi = std::get<0>(cmd.params);
            logger->debug("Received komi {}", komi);
        }
        virtual VertexOrPass handle(const CmdGenmove& cmd) override
        {
            logger->debug("Received genmove");
            Color c = std::get<0>(cmd.params);

            using PT = typename decltype(board)::PointType;

            uct::Tree<uct::detail::UCTTreePolicy<BOARDSIZE, BOARDSIZE>> tree(board, colorToPlayer(c), komi);
            using TreeT = decltype(tree);
            tree.run(4, std::chrono::seconds(1));
            typename TreeT::TreeNodeType *p = tree.getResultNode();

            if(p == NULL)
            {
                return Pass();
            } else
            {
                logger->trace("Valid pos: ({}, {})  ", (int)p->block.action.x, (int)p->block.action.y);
                logger->debug("Choose to place at {}, {}", (int)p->block.action.x + 1, (int)p->block.action.y + 1);
                board.place(p->block.action, colorToPlayer(c));
                logger->debug("After genmove \n {}", board);
                return VertexOrPass(p->block.action.x + 1, p->block.action.y + 1);
            }
        }
        virtual void handle(const CmdPlay& cmd) override
        {
            Move move = std::get<0>(cmd.params);
            Color c = move.color;
            if (move.vertex.type == VertexOrPassType::VERTEX)
            {
                using PT = typename decltype(board)::PointType;
                Vertex v = (Vertex)(move.vertex);
                logger->debug("Received Play at ({}, {})", v.x - 1, v.y - 1);
                board.place(PT {(char)(v.x - 1), (char)(v.y - 1)}, colorToPlayer(c));
            }
            else
            {
                logger->debug("Received PASS");
                received_pass = true;
            }
            logger->debug("After play \n {}", board);
        }
        virtual void handle(const CmdTimeSettings& cmd) override
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

        virtual ~UctEngine()
        {
            logger->debug("UCT Engine destroyed!");
        }
    };
}

#endif //GO_AI_UCT_ENGINE_HPP_HPP
