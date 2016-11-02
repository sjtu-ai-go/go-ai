//
// Created by lz on 11/2/16.
//

#ifndef GO_AI_GROUP_NODE_HPP
#define GO_AI_GROUP_NODE_HPP

#include <climits>
#include <cstdint>
#include "basic.hpp"

namespace board
{
    // GroupNode is a linked list storing information about one connected group
    static_assert(CHAR_BIT == 8, "WTF! 1 char != 8 bits on your platform");
    struct GroupNode;
    static_assert(sizeof(GroupNode*) <= 8, "Pointer on your platform > 8. Compression unsupported");
    struct alignas(8) GroupNode
    {
    private:
        std::uintptr_t next: 48; // on AMD64 virtual address only supports low 48bits
        Player player: 1;
        unsigned int liberty: 15;
    public:
        GroupNode() = default;
        GroupNode(GroupNode *pgn, Player p, unsigned int liberty):
                next(reinterpret_cast<std::uintptr_t >(pgn)), player(p), liberty(liberty)
        {}
        GroupNode* getNext() const
        {
            return reinterpret_cast<GroupNode*>(next);
        }
        void setNext(GroupNode *pgn)
        {
            next = reinterpret_cast<std::uintptr_t>(pgn);
        }
        unsigned int getLiberty()
        {
            return liberty;
        }
        void setLiberty(unsigned int lib)
        {
            liberty = lib;
        }
        void addLiberty(int libDelta)
        {
            libDelta += libDelta;
        }
        Player getPlayer() const
        {
            return static_cast<Player>(static_cast<unsigned>(player) & 1);
        }
        void setPlayer(Player p)
        {
            player = p;
        }
    };
    static_assert(sizeof(GroupNode) == 8, "Compression failed");

}
#endif //GO_AI_GROUP_NODE_HPP
