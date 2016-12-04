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
#include <functional>
#include <unordered_map>
#include <boost/variant/variant.hpp>
#include <boost/variant/get.hpp>
#include "grid_point.hpp"
#include "group_node.hpp"
#include "common/logger.hpp"

namespace board
{
    template <std::uint_least8_t W, std::uint_least8_t H>
    class PosGroup
    {
        using GroupIterator = typename std::list< GroupNode<W, H> >::iterator;
        using GroupConstIterator = typename std::list< GroupNode<W, H> >::const_iterator;
        std::shared_ptr<spdlog::logger> logger = getGlobalLogger();
    public:
        using PointType = GridPoint<W, H>;

        struct GroupConstIteratorHash
        {
            std::hash<const GroupNode<W, H>*> h;
            std::size_t operator() (GroupConstIterator gci) const
            {
                return h(&(*gci));
            }
        };
    protected:
        using ItemType = boost::variant<GroupIterator, PointType>;
        mutable ItemType arr[W * H];

        static std::size_t pointToIndex(PointType p)
        {
            return p.x * W + p.y;
        }

        PointType findfa(PointType p) const
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
        PosGroup(const PosGroup& other,
                 const std::unordered_map<GroupConstIterator , GroupIterator, GroupConstIteratorHash> &oldToNewMap):
                arr(other.arr), logger(other.logger)
        {
            PointType::for_all([&](PointType p) {
                if (GroupIterator *pgi = boost::get<GroupIterator>(&arr[pointToIndex(p)]))
                {
                    auto it = oldToNewMap.find(*pgi);
                    assert(it != oldToNewMap.end());
                    arr[pointToIndex(p)] = it->second;
                }
            });
        }
        PosGroup(GroupIterator default_it)
        {
            fill(default_it);
        }
        void fill(GroupIterator default_it)
        {
            std::fill(std::begin(arr), std::end(arr), default_it);
        }
        GroupIterator get(PointType p) const
        {
            PointType fa = findfa(p);
            return boost::get<GroupIterator>(arr[pointToIndex(fa)]);
        }
        // Use this only if this is the first time set(*, this_iterator) is called! otherwise please use merge()!
        void set(PointType p, GroupIterator it)
        {
            arr[pointToIndex(p)] = it;
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
