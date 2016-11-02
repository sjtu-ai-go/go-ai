//
// Created by lz on 11/2/16.
//

#include <cstddef>
#include <memory>
#include <functional>

#ifndef GO_AI_BOARD_CLASS_HPP
#define GO_AI_BOARD_CLASS_HPP

#include "basic.hpp"
#include "grid_point.hpp"
#include "group_node.hpp"
#include "pos_group.hpp"
#include "board_grid.hpp"
namespace board
{
    template<std::size_t W, std::size_t H>
    class Board;
}

namespace std
{
    template<std::size_t W, std::size_t H>
    struct hash<board::Board<W, H>>;
}

namespace board
{
    template <std::size_t W, std::size_t H>
    class Board
    {
    private:
        BoardGrid<W, H> boardGrid_;
        std::unique_ptr< GroupNode > pGroupNode_;
        PosGroup<W, H> posGroup_;
        std::size_t step_ = 0;
        std::size_t last2StateHash_ = 0x24512211u; // The hash of board 2 steps before. Used to validate ko.
    public:
        Board() = default;
        using PointType = GridPoint<W, H>;
        friend class std::hash<Board>;
        static const std::size_t w = W;
        static const std::size_t h = H;
        // Returns color of a point
        PointState getPointState(PointType p) const
        {
            return boardGrid_.get(p);
        }
        // Returns pointer to group of a point. NULL if there is no piece
        const GroupNode *getPointGroup(PointType p) const
        {
            return posGroup_.get(p);
        }
        std::size_t getStep() const
        {
            return step_;
        }
        // place a piece on the board. State will be changed
        void place(PointType p, Player player)
        {
            // TODO: Finish this function
            ++step_;
        }

        enum struct PositionStatus
        {
            OK, // Legal to place a piece
            NOCHANGE, // The piece to place is immediately taken away, and no change does it make to board
            KO, // Violates the rule of Ko
            NOTEMPTY // The place is not empty
        };
        // Whether it is legal/why it is illegal to place a piece of player at p. State will not be changed.
        PositionStatus getPosStatus(PointType p, Player player)
        {
            // TODO: Implement this
            return PositionStatus::OK;
        }
        // Find all valid position for player
        std::vector<PointType> getAllValidPosition(Player player)
        {
            std::vector<PointType> ans;
            for (std::size_t i=0; i<W; ++i)
                for (std::size_t j=0; j<H; ++j)
                    if (getPosStatus(PointType {i, j}, player) == PositionStatus::OK)
                    {
                        ans.push_back(PointType {i, j});
                    }
            return ans;
        }
        // Returns first node(may be empty) of GroupNode link list
        const GroupNode* groupBegin() const
        {
            return pGroupNode_.get();
        }
    };
}

namespace std
{

    template<std::size_t W, std::size_t H>
    struct hash<board::Board<W, H>>
    {
    private:
        hash<board::BoardGrid<W, H>> h;
    public:
        std::size_t operator() (const board::Board<W, H> &b) const
        {
            return h(b.boardGrid_);
        }
    };
}
#endif //GO_AI_BOARD_CLASS_HPP
