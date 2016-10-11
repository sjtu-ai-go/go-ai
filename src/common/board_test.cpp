//
// Created by lz on 10/11/16.
//
#include <cstdlib>
#include <cstddef>
#include <gtest/gtest.h>
#include "board.hpp"

TEST(BoardTest, TestBoardGrid)
{
    using namespace std;
    board::BoardGrid<19, 19> bg;
    EXPECT_EQ(board::PointState::NA, bg.get(18, 18));
    EXPECT_EQ(board::PointState::NA, bg.get(0,0));
    EXPECT_EQ(board::PointState::NA, bg.get(2,5));

    std::array<std::array<board::PointState, 19>, 19> arr {};

    for (int i=0; i<100; ++i)
    {
        size_t x, y;
        do
        {
            x = rand() % 19;
            y = rand() % 19;
        } while(arr[x][y] != board::PointState::NA); // set an arbitary empty point
        board::PointState st { static_cast<board::PointState>(rand() % 3) };
        bg.set(x, y, st);
        arr[x][y] = st;
        for (x=0; x<19; ++x)
            for (y=0; y<19; ++y)
                EXPECT_EQ(bg.get(x,y), arr[x][y]);
    }
}

TEST(BoardTest, TestPoint)
{
    using namespace std;
    using bg_t = board::BoardGrid<19, 19>;
    bg_t bg;
    bg_t::Point point(2, 0);
    EXPECT_EQ(2, point.x);
    EXPECT_EQ(0, point.y);

    EXPECT_EQ(board::PointState::NA, bg.get(point));
    bg.set(point, board::PointState::B);
    EXPECT_EQ(board::PointState::B, bg.get(point));

    point.down();
    EXPECT_EQ(3, point.x);

    EXPECT_TRUE(point.is_left());
    EXPECT_FALSE(point.is_top());
    EXPECT_FALSE(point.is_bottom());
    EXPECT_FALSE(point.is_right());

    --point;
    EXPECT_EQ(2, point.x);
    EXPECT_EQ(18, point.y);
    EXPECT_TRUE(point.is_right());
}

