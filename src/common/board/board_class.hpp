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
#include <ostream>
#include <common/logger.hpp>
#include <spdlog/fmt/ostr.h>

namespace board
{
    template<std::size_t W, std::size_t H>
    class Board;
    template<std::size_t W, std::size_t H>
    std::ostream &  operator<<(std::ostream & o, const Board<W, H> & b);
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
        std::list< GroupNode<W, H> > groupNodeList_;
        PosGroup<W, H> posGroup_;
        std::size_t step_ = 0;
        std::size_t lastStateHash_ = 0x24512211u; // The hash of board 1 steps before. Used to validate ko.
        std::size_t curStateHash_ = 0xc7151360u; // Hash of current board
        std::shared_ptr<spdlog::logger> logger = getGlobalLogger();
    public:
        Board(): posGroup_(groupNodeList_.end())
        {
        }
        using PointType = GridPoint<W, H>;
        using GroupNodeType = GroupNode<W, H>;
        using GroupListType = std::list< GroupNodeType >;
        using GroupIterator = typename GroupListType::iterator;
        using GroupConstIterator = typename GroupListType::const_iterator;
        friend class std::hash<Board>;
        static const std::size_t w = W;
        static const std::size_t h = H;
        // Returns color of a point
        PointState getPointState(PointType p) const
        {
            return boardGrid_.get(p);
        }
        // Returns pointer to group of a point. NULL if there is no piece
        GroupConstIterator getPointGroup(PointType p) const
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
        GroupConstIterator groupBegin() const
        {
            return groupNodeList_.cbegin();
        }

        GroupConstIterator groupEnd() const
        {
            return groupNodeList_.cend();
        }

        friend std::ostream& operator<< <>(std::ostream&, const Board&);

    private:
        // Internal use only
        GroupIterator getPointGroup_(PointType p)
        {
            return posGroup_.get(p);
        }
        void removeGroup(GroupIterator group);
        void mergeGroupAt(PointType thisPoint, PointType otherPoint);
        std::vector<GroupIterator> getAdjacentGroups(PointType p);
    };

    template<std::size_t W, std::size_t H>
    auto Board<W, H>::getAdjacentGroups(PointType p) -> std::vector<GroupIterator>
    {
        std::vector<GroupIterator> adjGroups;
        adjGroups.reserve(4);
        p.for_each_adjacent([&](PointType adjP) {
            GroupIterator group = getPointGroup_(adjP);
            if (group != groupNodeList_.end())
                adjGroups.push_back(group);
        });
        // remove Duplicated groups
        auto newEnd = std::unique(adjGroups.begin(), adjGroups.end());
        adjGroups.erase(newEnd, adjGroups.end());
        return adjGroups;
    }

    template<std::size_t W, std::size_t H>
    void Board<W, H>::removeGroup(GroupIterator group)
    {
        std::vector<PointType> point_to_remove;
        point_to_remove.reserve(W * H);
        PointType::for_all([&](PointType p) {
           if (getPointGroup(p) == group)
           {
               std::vector<GroupIterator> adjGroups = getAdjacentGroups(p);
               std::for_each(adjGroups.begin(), adjGroups.end(), [&](GroupIterator adjGroup) {
                  if (adjGroup != group) adjGroup->setLiberty(p, true);
               });
               boardGrid_.set(p, PointState::NA);
               // posGroup_.set(p, groupNodeList_.end());
               // cannot delete here, since union-set would stuck into inconsistent state
               point_to_remove.push_back(p);
           }
        });
        std::for_each(point_to_remove.begin(), point_to_remove.end(), [&](PointType p){
            posGroup_.set(p, groupNodeList_.end());
        });
        groupNodeList_.erase(group);
    }

    template<std::size_t W, std::size_t H>
    void Board<W, H>::mergeGroupAt(PointType thisPoint, PointType thatPoint)
    {
        GroupIterator thisGroup = getPointGroup_(thisPoint), thatGroup = getPointGroup_(thatPoint);
        posGroup_.merge(thisPoint, thatPoint);

        thisGroup->mergeLiberty(*thatGroup);
        groupNodeList_.erase(thatGroup);
    }

    template<std::size_t W, std::size_t H>
    void Board<W,H>::place(PointType p, Player player)
    {
        logger->debug("Place at {}, {}: {}", (int)p.x, (int)p.y, (int) player);
        if (getPointState(p) != PointState::NA)
            throw std::runtime_error("Try to place on an non-empty point");

        boardGrid_.set(p, getPointStateFromPlayer(player));

        Player opponent = getOpponentPlayer(player);

        // --- Decrease liberty of adjacent groups
        std::vector<GroupIterator> adjGroups = getAdjacentGroups(p);
        std::for_each(adjGroups.begin(), adjGroups.end(), [&](GroupIterator group) {
            logger->trace("Adjacent groups's liberty: {}", group->getLiberty());
        });

        // Update liberty and remove opponent's dead groups (liberty of our group may change)
        std::for_each(adjGroups.begin(), adjGroups.end(), [&](GroupIterator pgn)
        {
            pgn->setLiberty(p, false);
            if (pgn->getPlayer() == opponent && pgn->getLiberty() == 0)
            {
                logger->trace("Removing group with liberty{}", pgn->getLiberty());
                removeGroup(pgn); // removing group A won't affect liberty of group B (where A, B belongs to same player)
            }
        });

        // --- Add this group
        logger->trace("Adding this group");

        GroupNodeType gn(player);
        p.for_each_adjacent([&](PointType adjP) {
            logger->trace("Adjacent point {},{} is empty, setting liberty", (int)adjP.x, (int)adjP.y);
            if (getPointState(adjP) == PointState::NA)
                gn.setLiberty(adjP, true);
            logger->trace("Current liberty: {}", gn.getLiberty());
        });
        auto thisGroup = groupNodeList_.insert(groupNodeList_.cbegin(), gn);
        posGroup_.set(p, thisGroup);

        // --- Merge our group
        logger->trace("Merging group");
        p.for_each_adjacent([&](PointType adjP) {
            GroupIterator adjPointGroup = getPointGroup_(adjP);
            if (adjPointGroup != groupNodeList_.end() &&
                    adjPointGroup->getPlayer() == player &&
                    adjPointGroup != thisGroup)
            {
                logger->trace("Merging group with liberty {}", adjPointGroup->getLiberty());
                mergeGroupAt(p, adjP);
            }
        });

        // --- remove our dead groups
        if (thisGroup->getLiberty() == 0) {
            removeGroup(thisGroup);
            logger->trace("Removing self...");
        }
        p.for_each_adjacent([&](PointType p) {
            GroupIterator adjPointGroup = getPointGroup_(p);
            if (adjPointGroup != groupNodeList_.end() && adjPointGroup->getLiberty() == 0) {
                logger->trace("Removing group at ({}, {})", (int)p.x, (int) p.y);
            }
        });
        logger->trace("After move:{}", *this);
        std::hash<Board> h;
        std::size_t hash_v = h(*this);
        logger->trace("last 2 hash: {}, last 1 hash: {}, cur Hash: {}", lastStateHash_, curStateHash_, hash_v);
        lastStateHash_ = curStateHash_;
        curStateHash_ = hash_v;
        ++step_;
    }

    template<std::size_t W, std::size_t H>
    auto Board<W,H>::getPosStatus(PointType p, Player player) -> Board::PositionStatus
    {
        if (getPointState(p) != PointState::NA)
            return Board::PositionStatus::NOTEMPTY;
        Board testBoard = *this;

        std::size_t last2hash = testBoard.lastStateHash_;
        testBoard.place(p, player);
        if (testBoard.lastStateHash_ == testBoard.curStateHash_)
            return Board::PositionStatus::NOCHANGE;
        if (testBoard.curStateHash_ == last2hash)
            return Board::PositionStatus::KO;
        return Board::PositionStatus::OK;
    };

    template<std::size_t W, std::size_t H>
    std::ostream &  operator<<(std::ostream & o, const Board<W, H> & b) {
        using PT = typename Board<W, H>::PointType;
        o << "Points" << std::endl;
        PT::for_all([&](PT p) {
            o << (int) b.boardGrid_.get(p) << ' ';
            if (p.is_right()) o << std::endl;
        });
        o << "Group Liberties"<< std::endl;
        PT::for_all([&](PT p) {
            auto group = b.getPointGroup(p);
            if (group == b.groupNodeList_.end())
                o << "O\t";
            else
                o << group->getLiberty() << "\t";
            if (p.is_right()) o << std::endl;
        });
        return o;
    }
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
