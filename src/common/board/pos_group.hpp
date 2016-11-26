//
// Created by lz on 11/2/16.
//

#ifndef GO_AI_POS_GROUP_HPP
#define GO_AI_POS_GROUP_HPP

#include <cstddef>
#include <cstdint>
#include <list>
#include <algorithm>
#include <iterator>
#ifndef NDEBUG
#include <set>
#endif
#include <boost/variant/variant.hpp>
#include <boost/variant/get.hpp>
#include "grid_point.hpp"
#include "group_node.hpp"

namespace board
{
    template <std::uint_least8_t W, std::uint_least8_t H>
    class PosGroup
    {
        using GroupIterator = typename std::list< GroupNode<W, H> >::iterator;
    public:
        using PointType = GridPoint<W, H>;
    protected:
        using ItemType = boost::variant<GroupIterator, PointType>;
        ItemType arr[W * H];

        static std::size_t pointToIndex(PointType p)
        {
            return p.x * W + p.y;
        }

        PointType findfa(PointType p)
        {
            std::size_t idx = pointToIndex(p);
            if (PointType *pp = boost::get<PointType>(&arr[idx]))
            {
                arr[idx] = findfa(*pp);
                return boost::get<PointType>(arr[idx]);
            }
            else
                return p;
        }

    public:
        PosGroup() = default;
        PosGroup(GroupIterator default_it)
        {
            fill(default_it);
        }
        void fill(GroupIterator default_it)
        {
            std::fill(std::begin(arr), std::end(arr), default_it);
        }
        GroupIterator get(PointType p)
        {
            PointType fa = findfa(p);
            return boost::get<GroupIterator>(arr[pointToIndex(fa)]);
        }
        // Use this only if this is the first time set(*, this_iterator) is called! otherwise please use merge()!
        void set(PointType p, GroupIterator it)
        {
            PointType fa = findfa(p);
            arr[pointToIndex(fa)] = it;
        }

        // Merge p2 into p1
        // fa[ getfa(p2) ] = getfa(p1)
        void merge(PointType p1, PointType p2)
        {
            PointType fa1 = findfa(p1), fa2 = findfa(p2);
            if (fa1 != fa2)
                arr[pointToIndex(fa2)] = p1;
        }

    };
}
#endif //GO_AI_POS_GROUP_HPP
