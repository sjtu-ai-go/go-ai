//
// Created by lz on 11/2/16.
//

#ifndef GO_AI_POS_GROUP_HPP
#define GO_AI_POS_GROUP_HPP

#include <cstddef>
#include <cstdint>
#include <list>
#include "group_node.hpp"

namespace board
{
    // PosGroup: GroupNode * [19][19]. If that position does not has a piece, the result would be nullptr
    // actually each GroupNode* will be stored in a 6-byte unsigned char
    template <std::uint_least8_t W, std::uint_least8_t H>
    class PosGroup: public compgrid::CompressedGrid< typename std::list<GroupNode<W, H> >::iterator, W, H, 48>
    {
        static_assert(sizeof(typename std::list<GroupNode<W, H>>::iterator) <= 64, "Iterator too large to be compressed");
        using BaseType = compgrid::CompressedGrid< typename std::list<GroupNode<W, H> >::iterator, W, H, 48>;
    public:
        PosGroup() = default;
        PosGroup(const typename std::list<GroupNode<W, H>>::iterator &it_end): BaseType (it_end) {}
    };
}
#endif //GO_AI_POS_GROUP_HPP
