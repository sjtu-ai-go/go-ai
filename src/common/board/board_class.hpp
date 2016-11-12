//
// Created by lz on 11/2/16.
//

#include <cstddef>
#include <memory>
#include <functional>
#include <list>

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
        std::list<GroupNode<W, H>> groupNodeList_;
        PosGroup<W, H> posGroup_;
        std::size_t step_ = 0;
        std::size_t lastStateHash_ = 0x24512211u; // The hash of board 1 steps before. Used to validate ko.
        std::size_t curStateHash_ = 0xc7151360u; // Hash of current board
    public:
        Board() = default;
        using PointType = GridPoint<W, H>;
        using GroupNodeType = GroupNode<W, H>;
        using GroupListType = std::list< GroupNodeType >;
        friend class std::hash<Board>;
        static const std::size_t w = W;
        static const std::size_t h = H;
        // Returns color of a point
        PointState getPointState(PointType p) const
        {
            return boardGrid_.get(p);
        }
        // Returns pointer to group of a point. NULL if there is no piece
        typename GroupListType::const_iterator getPointGroup(PointType p) const
        {
            return posGroup_.get(p);
        }
        std::size_t getStep() const
        {
            return step_;
        }
        // place a piece on the board. State will be changed
        void place(PointType p, Player player);

        enum struct PositionStatus
        {
            OK, // Legal to place a piece
            NOCHANGE, // The piece to place is immediately taken away, and no change does it make to board
            KO, // Violates the rule of Ko
            NOTEMPTY // The place is not empty
        };
        // Whether it is legal/why it is illegal to place a piece of player at p. State will not be changed.
        PositionStatus getPosStatus(PointType p, Player player);
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
        typename GroupListType::const_iterator groupBegin() const
        {
            return groupNodeList_.cbegin();
        }

        typename GroupListType::const_iterator groupEnd() const
        {
            return groupNodeList_.cend();
        }

    private:
        // Internal use only
        typename GroupListType::iterator getPointGroup(PointType p)
        {
            return posGroup_.get(p);
        }
        void removeGroup(GroupNodeType *);
    };

    template<std::size_t W, std::size_t H>
    void Board<W,H>::place(PointType p, Player player)
    {
        if (getPointState(p) != PointState::NA)
            throw std::runtime_error("Try to place on an non-empty point");

        boardGrid_.set(p, getPointStateFromPlayer(player));

        Player opponent = getOpponentPlayer(player);

        // --- Decrease liberty of adjacent groups
        std::vector<GroupNodeType *> adjGroups;
        p.for_each_adjacent([&](PointType adjP) { adjGroups.push_back(getPointGroup(adjP)); });

        // Remove duplicate groups
        auto newEnd = std::unique(adjGroups.begin(), adjGroups.end());
        adjGroups.erase(newEnd, adjGroups.end());

        // Update liberty and remove opponent's dead groups (liberty of our group may change)
        std::for_each(adjGroups.begin(), adjGroups.end(), [&](GroupNodeType * pgn)
        {
            pgn->setLiberty(p, false);
            if (pgn->getPlayer() == opponent && pgn->getLiberty() == 0)
                removeGroup(pgn); // removing group A won't affect liberty of group B (where A, B belongs to same player)
        });

        // --- Add this group

        GroupNodeType gn(player);
        p.for_each_adjacent([&](PointType adjP) {
            if (getPointState(adjP) == PointState::NA)
                gn.setLiberty(adjP, true);
        });
        groupNodeList_.insert(groupNodeList_.cbegin(), gn);

        // --- TODO: merge our groups

        // --- TODO: remove our dead groups

    }

    template<std::size_t W, std::size_t H>
    auto Board<W,H>::getPosStatus(PointType p, Player player) -> Board::PositionStatus
    {

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
