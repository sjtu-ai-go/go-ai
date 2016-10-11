#ifndef COMMON_BOARD_HPP
#define COMMON_BOARD_HPP

#include <cstddef>
#include <bitset>
#include <climits>
#include <cstring>
namespace board
{
    enum struct PointState { NA, W, B }; // state of a point on board
    static const std::size_t PointStateBits = 2; // Can be represented in 2 bits since 2 < 2 ^ 2
    enum struct Player { W, B }; // White or Black side

    // Compress the board into bit set
    template<std::size_t W, std::size_t H>
    struct BoardGrid
    {
    private:
        std::bitset< PointStateBits * W * H > buf;

        static inline std::size_t xyToIndex(std::size_t x, std::size_t y)
        {
            return (x * W + y) * PointStateBits;
        }
    public:
        BoardGrid() = default;

        inline PointState get(std::size_t x, std::size_t y) const
        {
            return static_cast<PointState>(((buf[xyToIndex(x,y) + 1]) << 1) + buf[xyToIndex(x,y)]);
        }

        inline PointState set(std::size_t x, std::size_t y, PointState pointState)
        {
            buf[xyToIndex(x,y) + 1] = static_cast<int>(pointState) & 2;
            buf[xyToIndex(x,y)] = static_cast<int>(pointState) & 1;
        }
    };
}

#endif
