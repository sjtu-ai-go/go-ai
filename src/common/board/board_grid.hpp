//
// Created by lz on 11/2/16.
//

#ifndef GO_AI_BOARD_GRID_HPP
#define GO_AI_BOARD_GRID_HPP
#include <cstddef>
#include <functional>
#include <bitset>
#include <compressed_grid.hpp>
#include "basic.hpp"
#include "grid_point.hpp"

namespace board
{
    // Compress the board into bit set
    template<std::size_t W, std::size_t H>
    struct BoardGrid: public compgrid::CompressedGrid<PointState, W, H, 2>
    {
    public:
        BoardGrid() = default;
    };

}


namespace std
{
    template<std::size_t W, std::size_t H>
    struct hash<board::BoardGrid<W, H>>
    {
    private:
        hash< compgrid::CompressedGrid<board::PointState, W, H, 2>> h;
    public:
        std::size_t operator()(const board::BoardGrid<W, H> &bg) const
        {
            return h(bg);
        }
    };
}


#endif //GO_AI_BOARD_GRID_HPP
