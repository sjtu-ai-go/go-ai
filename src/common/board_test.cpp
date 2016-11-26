//
// Created by lz on 10/11/16.
//
#include <cstdlib>
#include <cstddef>
#include <vector>
#include <map>
#include <functional>
#include <gtest/gtest.h>
#include <list>
#include "board.hpp"
#include "logger.hpp"

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
    auto logger = getGlobalLogger();

    gnl.insert(gnl.cend(), gn_t(Player::B));
    gnl.insert(gnl.cend(), gn_t(Player::W));
    auto n1 = gnl.begin();
    auto n2 = std::next(n1);

    PosGroup<19, 19> pg(gnl.end());
    logger->info("Size of posgroup<19, 19>: {}", sizeof(pg));

    using PT = typename decltype(pg)::PointType;
    EXPECT_EQ(gnl.end(), pg.get(PT{2, 4}));
    pg.set(PT{18, 6}, n1);
    pg.set(PT{0, 18}, n2);
    pg.merge(PT{18, 6}, PT{0, 17});
    pg.merge(PT{0, 18}, PT{18, 5});

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

    // self-merge n1 <- n1 should be okay
    pg.merge(PT{18, 6}, PT{0, 17});
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

    // self-merge n2 <- n2 should be okay too
    pg.merge(PT{0, 18}, PT{18, 5});
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
    pg.merge(PT{18, 6}, PT{ 18, 5 });
    pg.merge(PT{0, 18}, PT{18, 5});
    for (char i=0; i<19; ++i)
        for (char j=0; j<19; ++j)
        {
            if ((i == 18 && j == 6) || (i == 0 && j == 17))
                EXPECT_EQ(n1, pg.get(PT{i, j}));
            else if ((i==0 && j==18) || (i==18 && j == 5))
                EXPECT_EQ(n1, pg.get(PT{i, j})); // n2 should be n1 now
            else
                EXPECT_EQ(gnl.end(), pg.get(PT{i, j}));
        }
}


using GraphItem = std::pair<int, board::Player>;
const GraphItem O = GraphItem(0, board::Player::B);
// Convert
// 1 2
// 4 3
// to (0, 0), (0, 1), (1, 1), (1, 0)
template<std::size_t W, std::size_t H, std::size_t ArrW, std::size_t ArrH>
std::vector<std::pair<board::GridPoint<W, H>, board::Player>> graphToPoint(GraphItem (&arr)[ArrW][ArrH])
{
    static_assert(ArrW <= W && ArrH <= H, "Size of array must <= size of (W, H)");
    std::map<int, std::pair<board::GridPoint<W, H>, board::Player>> m;
    for (std::size_t i=0; i<ArrW; ++i)
        for (std::size_t j=0; j<ArrH; ++j)
        {
            if (arr[i][j] != O)
                m.insert(
                        std::make_pair(arr[i][j].first,
                                       std::make_pair(board::GridPoint<W, H>(i, j), arr[i][j].second)
                        ));
        }
    std::vector<std::pair<board::GridPoint<W, H>, board::Player>> ans;
    std::for_each(m.cbegin(), m.cend(), [&ans](typename decltype(m)::value_type v) {
        ans.push_back(v.second);
    });
    return ans;
};

std::pair<int, board::Player> operator"" _w(unsigned long long ord)
{
    return std::make_pair(ord, board::Player::W);
};

std::pair<int, board::Player> operator"" _b(unsigned long long ord)
{
    return std::make_pair(ord, board::Player::B);
};

template<std::size_t W, std::size_t H, std::size_t ArrW, std::size_t ArrH>
bool check(int (&arr)[ArrW][ArrH],board::Board<W, H> &b)
{
    using PT = typename board::Board<W, H>::PointType;
    for (std::size_t i=0; i<ArrW; ++i)
        for (std::size_t j=0; j<ArrH; ++j)
        {
            auto group = b.getPointGroup(PT {(char)i, (char)j});
            if (group == b.groupEnd())
            {
                if (arr[i][j] != 0)
                    return false;
            }
            else
            {
                if (arr[i][j] != group->getLiberty())
                {
                    return false;
                }
            }
        }
    return true;
};


TEST(BoardTest, TestBoardClassPlace1)
{
    using namespace board;
    auto logger = getGlobalLogger();
    logger->set_level(spdlog::level::debug);

    Board<19, 19> b;
    using BT = Board<19, 19>;
    using PT = typename BT::PointType;
    GraphItem graph[3][3] = {
            {1_b, 2_b, 3_b},
            {4_b, 5_w, 6_b},
            {7_b, 8_b, O}
    };
    auto points = graphToPoint<19, 19>(graph);
    std::for_each(points.begin(), points.end(), [&](std::pair<board::GridPoint<19, 19>, board::Player> item) {
        b.place(item.first, item.second);
        //std::cerr << b << std::endl;
    });
    int result[3][3] = {
            {6, 6, 6},
            {6, 0, 6},
            {6, 6, 0}
    };
    EXPECT_TRUE(check(result, b));
}

TEST(BoardTest, TestBoardClassPlace2)
{
    using namespace board;
    auto logger = getGlobalLogger();
    logger->set_level(spdlog::level::debug);

    Board<3, 3> b;
    using BT = Board<3, 3>;
    using PT = typename BT::PointType;
    GraphItem graph[3][3] = {
            {1_b, 2_b, 3_b},
            {4_b, 9_w, 5_b},
            {6_b, 7_b, 8_b}
    };
    auto points = graphToPoint<3, 3>(graph);
    std::for_each(points.begin(), points.end(), [&](std::pair<board::GridPoint<3, 3>, board::Player> item) {
        b.place(item.first, item.second);
        std::cerr << b << std::endl;
    });
    int result[3][3] = {
            {0, 0, 0},
            {0, 4, 0},
            {0, 0, 0}
    };
    EXPECT_TRUE(check(result, b));
}

TEST(BoardTest, TestBoardClassPlace3)
{
    using namespace board;
    auto logger = getGlobalLogger();
    logger->set_level(spdlog::level::debug);

    Board<4, 4> b;
    using BT = Board<4, 4>;
    using PT = typename BT::PointType;
    GraphItem graph[3][3] = {
            {1_b, 2_b, 3_b},
            {4_b, 9_w, 5_b},
            {6_b, 7_b, 8_b}
    };
    auto points = graphToPoint<4, 4>(graph);
    std::for_each(points.begin(), points.end(), [&](std::pair<board::GridPoint<4, 4>, board::Player> item) {
        b.place(item.first, item.second);
        //std::cerr << b << std::endl;
    });
    int result[3][3] = {
            {7, 7, 7},
            {7, 0, 7},
            {7, 7, 7}
    };
    EXPECT_TRUE(check(result, b));
}

TEST(BoardTest, TestBoardClassPlace4)
{
    using namespace board;
    auto logger = getGlobalLogger();
    logger->set_level(spdlog::level::debug);

    Board<3, 3> b;
    using BT = Board<3, 3>;
    using PT = typename BT::PointType;
    GraphItem graph[3][3] = {
            {1_b, 2_w, 6_w},
            {9_b, 8_w, 4_w},
            {3_b, 5_b, 7_b}
    };
    auto points = graphToPoint<3, 3>(graph);
    std::for_each(points.begin(), points.end(), [&](std::pair<board::GridPoint<3, 3>, board::Player> item) {
        b.place(item.first, item.second);
        //std::cerr << b << std::endl;
    });
    int result[3][3] = {
            {3, 0, 0},
            {3, 0, 0},
            {3, 3, 3}
    };
    EXPECT_TRUE(check(result, b));
}


TEST(BoardTest, TestBoardClassLegalMove1)
{
    using namespace board;
    auto logger = getGlobalLogger();
    logger->set_level(spdlog::level::debug);

    Board<5, 5> b;
    using BT = Board<5, 5>;
    using PT = typename BT::PointType;
    GraphItem graph[5][5] = {
            {O,     1_b,    2_w,    3_w,    O},
            {O,     4_b,    5_w,    O,      6_w},
            {7_b,   8_b,    9_b,    10_w,   11_w},
            {O,     12_b,   O,      13_b,   14_b},
            {O,     O,      15_b,   16_w,   O}
    };
    auto points = graphToPoint<5, 5>(graph);
    std::for_each(points.begin(), points.end(), [&](std::pair<board::GridPoint<5, 5>, board::Player> item) {
        b.place(item.first, item.second);
        //std::cerr << b << std::endl;
    });
    auto bValid = b.getAllValidPosition(Player::B);
    EXPECT_EQ(7u, bValid.size());

    auto wValid = b.getAllValidPosition(Player::W);
    EXPECT_EQ(7u, wValid.size()); // (7, 7) is Ko!

}
