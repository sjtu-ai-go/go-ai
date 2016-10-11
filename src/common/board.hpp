#ifndef COMMON_BOARD_HPP
#define COMMON_BOARD_HPP

#include <cstddef>
#include <bitset>
#include <climits>
#include <cstring>
#include <iterator>
#include <functional>
namespace board
{
    template<std::size_t W, std::size_t H>
    struct GridPoint
    {
        char x, y;
        GridPoint(char x, char y): x(x), y(y) {}
        void right()
        {
            ++y;
        }
        void left()
        {
            --y;
        }
        void down()
        {
            ++x;
        }
        void up()
        {
            --x;
        }
        GridPoint& operator++()
        {
            if (++y == W)
            {
                y = 0;
                ++x;
            }
            return *this;
        }
        GridPoint& operator--()
        {
            if (!y)
            {
                y = W - 1;
                --x;
            } else
                --y;
            return *this;
        }
        bool is_top() const
        {
            return !x;
        }
        bool is_bottom() const
        {
            return x == H - 1;
        }
        bool is_left() const
        {
            return !y;
        }
        bool is_right() const
        {
            return y == W - 1;
        }
    };

    enum struct PointState { NA, W, B }; // state of a point on board
    static const std::size_t PointStateBits = 2; // Can be represented in 2 bits since 2 < 2 ^ 2
    enum struct Player { W, B }; // White or Black side

    // Compress the board into bit set
    template<std::size_t W, std::size_t H>
    struct BoardGrid
    {
    private:
        std::bitset< PointStateBits * W * H > buf;
        friend class std::hash<BoardGrid>;

        static inline std::size_t xyToIndex(std::size_t x, std::size_t y)
        {
            return (x * W + y) * PointStateBits;
        }
    public:
        using Point = GridPoint<W, H>;
        BoardGrid() = default;

        inline PointState get(std::size_t x, std::size_t y) const
        {
            return static_cast<PointState>(((buf[xyToIndex(x,y) + 1]) << 1) + buf[xyToIndex(x,y)]);
        }

        inline void set(std::size_t x, std::size_t y, PointState pointState)
        {
            buf[xyToIndex(x,y) + 1] = static_cast<int>(pointState) & 2;
            buf[xyToIndex(x,y)] = static_cast<int>(pointState) & 1;
        }

        inline PointState get(Point point) const
        {
            return get(point.x, point.y);
        }

        inline void set(Point point, PointState pointState)
        {
            set(point.x, point.y, pointState);
        }
    };

}


namespace std
{
    template<std::size_t W, std::size_t H>
    struct hash<board::BoardGrid<W, H>> {
    private:
        using bg_t = board::BoardGrid<W, H>;
        hash<decltype(declval<bg_t>().buf)> h;
    public:
        std::size_t operator()(const bg_t &bg) const
        {
            return h(bg.buf);
        }
    };
}

#endif
