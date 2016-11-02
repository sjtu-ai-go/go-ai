//
// Created by lz on 11/2/16.
//

#ifndef GO_AI_POS_GROUP_HPP
#define GO_AI_POS_GROUP_HPP

#include <cstddef>
#include <cstdint>

namespace board
{
    // PosGroup: GroupNode * [19][19]. If that position does not has a piece, the result would be nullptr
    // actually each GroupNode* will be stored in a 6-bit unsigned char
    template <std::uint_least8_t W, std::uint_least8_t H>
    class PosGroup
    {
    private:

    };
}
#endif //GO_AI_POS_GROUP_HPP
