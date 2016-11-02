//
// Created by lz on 11/2/16.
//

#ifndef GO_AI_GRID_POINT_HPP
#define GO_AI_GRID_POINT_HPP

#include <cstddef>
#include <cstdint>
#include <compressed_grid.hpp>

namespace board
{
    template<std::size_t W, std::size_t H>
    using GridPoint = compgrid::GridPoint<W, H>;
}
#endif //GO_AI_GRID_POINT_HPP
