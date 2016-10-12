//
// Created by lz on 10/11/16.
//
#include <cstdlib>
#include <cstddef>
#include <vector>
#include <functional>
#include <gtest/gtest.h>
#include "board.hpp"

TEST(BoardTest, TestBoardGridGetSet)
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

TEST(BoardTest, TestGridPoint)
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

TEST(BoardTest, TestBoardGridHash)
{
    using bg_t = board::BoardGrid<18, 18>;
    bg_t bg;

    std::hash<bg_t> h;

    std::vector<size_t> v;

    v.push_back(h(bg));
    for (int i=0; i<100; ++i)
    {
        std::size_t x, y;
        do
        {
            x = std::rand() % 18;
            y = std::rand() % 18;
        } while(bg.get(x, y) != board::PointState::NA);
        bg.set(x, y, static_cast<board::PointState>(std::rand() % 2 + 1));
        v.push_back(h(bg));
    }

    for (std::size_t i=0; i<99; ++i)
        EXPECT_NE(v[i], v[i+1]) << "V[" << i << "] == V[" << i+1 << "]: " << v[i] << " vs " << v[i+1];

}

TEST(BoardTest, TestGroupNode)
{
    using namespace board;
    using namespace std;
    GroupNode *head = new GroupNode(new GroupNode(nullptr, Player::B, 15), Player::W, 300);
    EXPECT_EQ(300, head->getLiberty());
    EXPECT_EQ(Player::W, head->getPlayer());
    EXPECT_EQ(15, head->getNext()->getLiberty());
    EXPECT_EQ(Player::B, head->getNext()->getPlayer());

    head->setLiberty(361);
    EXPECT_EQ(361, head->getLiberty());
    head->setPlayer(Player::B);
    EXPECT_EQ(Player::B, head->getPlayer());
    EXPECT_EQ(361, head->getLiberty());
    delete head->getNext();
    head->setNext(new GroupNode(nullptr, Player::B, 22));
    EXPECT_EQ(22, head->getNext()->getLiberty());

    delete head->getNext();
    delete head;
}

