//
// Created by lz on 11/27/16.
//

#ifndef GO_AI_BOARD_HEADER_HPP
#define GO_AI_BOARD_HEADER_HPP

#include "board_class.hpp"
namespace board
{
    extern template class Board<3, 3>;
    extern template class Board<4, 4>;
    extern template class Board<5, 5>;
    extern template class Board<19, 19>;
    extern template class Board<9, 9>;
}
#endif //GO_AI_BOARD_HEADER_HPP
