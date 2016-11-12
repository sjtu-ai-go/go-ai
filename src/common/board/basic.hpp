//
// Created by lz on 11/2/16.
//

#ifndef GO_AI_BASIC_HPP
#define GO_AI_BASIC_HPP

#include <cstddef>
namespace board
{
    enum struct PointState { NA, W, B }; // state of a point on board
    static const std::size_t PointStateBits = 2; // Can be represented in 2 bits since 2 < 2 ^ 2
    enum struct Player { W, B }; // White or Black side
    inline PointState getPointStateFromPlayer(Player p)
    {
        switch(p)
        {
            case Player::W:
                return PointState::W;
            case Player::B:
                return PointState::B;
        }
    }
    inline Player getOpponentPlayer(Player p)
    {
        switch (p)
        {
            case Player::W:
                return Player::B;
            case Player::B:
                return Player::W;
        }
    }
}
#endif //GO_AI_BASIC_HPP
