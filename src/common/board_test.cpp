//
// Created by lz on 10/11/16.
//
#include <cstdlib>
#include <cstddef>
#include <vector>
#include <functional>
#include <gtest/gtest.h>
#include <list>
#include "board.hpp"

TEST(BoardTest, TestBoardGridGetSet)
{
    using namespace std;
    board::BoardGrid<19, 19> bg;
    using PT = typename decltype(bg)::PointType;
    EXPECT_EQ(board::PointState::NA, bg.get(PT {18, 18 }));
    EXPECT_EQ(board::PointState::NA, bg.get(PT {0,0 }));
    EXPECT_EQ(board::PointState::NA, bg.get(PT {2,5}));

    std::array<std::array<board::PointState, 19>, 19> arr {};

    for (int i=0; i<100; ++i)
    {
        char x, y;
        do
        {
            x = rand() % 19;
            y = rand() % 19;
        } while(arr[x][y] != board::PointState::NA); // set an arbitary empty point
        board::PointState st { static_cast<board::PointState>(rand() % 3) };
        bg.set(PT{x, y}, st);
        arr[x][y] = st;
        for (x=0; x<19; ++x)
            for (y=0; y<19; ++y)
                EXPECT_EQ(bg.get(PT{x, y}), arr[x][y]);
    }
}

TEST(BoardTest, TestGridPoint)
{
    using namespace std;
    using bg_t = board::BoardGrid<19, 19>;
    bg_t bg;
    bg_t::PointType point(2, 0);
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

TEST(BoardTest, TestBoardGridHash)
{
    using bg_t = board::BoardGrid<18, 18>;
    bg_t bg;
    using PT = typename bg_t::PointType;

    std::hash<bg_t> h;

    std::vector<size_t> v;

    v.push_back(h(bg));
    for (int i=0; i<100; ++i)
    {
        char x, y;
        do
        {
            x = std::rand() % 18;
            y = std::rand() % 18;
        } while(bg.get(PT{x, y}) != board::PointState::NA);
        bg.set(PT{x, y}, static_cast<board::PointState>(std::rand() % 2 + 1));
        v.push_back(h(bg));
    }

    for (std::size_t i=0; i<99; ++i)
        EXPECT_NE(v[i], v[i+1]) << "V[" << i << "] == V[" << i+1 << "]: " << v[i] << " vs " << v[i+1];

}

TEST(BoardTest, TestGroupNode)
{
    using namespace board;
    using namespace std;

    list<GroupNode<19, 19>> gnl;
    using gn_t = GroupNode<19, 19>;
    gnl.insert(gnl.begin(), gn_t(Player::W));
    auto head = gnl.begin();

    gnl.insert(gnl.cend(), gn_t(Player::B));

    EXPECT_EQ(0, head->getLiberty());
    EXPECT_EQ(Player::W, head->getPlayer());
    auto tail = std::next(head);
    EXPECT_EQ(0, tail->getLiberty());
    EXPECT_EQ(Player::B, tail->getPlayer());
}

TEST(BoardTest, TestPosGroup)
{
    using namespace board;
    using gn_t = GroupNode<19, 19>;
    using gnl_t = std::list<gn_t>;
    gnl_t gnl;

    gnl.insert(gnl.cend(), gn_t(Player::B));
    gnl.insert(gnl.cend(), gn_t(Player::W));
    auto n1 = gnl.begin();
    auto n2 = std::next(n1);

    PosGroup<19, 19> pg(gnl.end());
    using PT = typename decltype(pg)::PointType;
    EXPECT_EQ(gnl.end(), pg.get(PT{2, 4}));
    pg.set(PT{18, 6}, n1);
    pg.set(PT{0, 18}, n2);
    pg.set(PT{0, 17}, n1);
    pg.set(PT{18, 5}, n2);

    for (char i=0; i<19; ++i)
        for (char j=0; j<19; ++j)
        {
            if ((i == 18 && j == 6) || (i == 0 && j == 17))
                EXPECT_EQ(n1, pg.get(PT{i, j}));
            else if ((i==0 && j==18) || (i==18 && j == 5))
                EXPECT_EQ(n2, pg.get(PT{i, j}));
            else
                EXPECT_EQ(gnl.end(), pg.get(PT{i, j}));
        }

    pg.set(PT{18, 6}, n2);
    EXPECT_EQ(n2, pg.get(PT{18, 6}));
}

TEST(BoardTest, TestBoardClass)
{
    using namespace board;
    Board<19, 19> b;
}

