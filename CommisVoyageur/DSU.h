#pragma once

#include <memory>
#include <unordered_map>

/*
* Implementation of Disjoint Set Union based on trees.
* Uses two basic euristics.
*/
template<typename Ty>
class DSU 
{
    std::unordered_map<Ty, Ty> parents;
    std::unordered_map<Ty, int> ranks;

public:
    DSU() = default;

    template<typename Container = std::vector<Ty>>
    DSU(Container&& cont)
    {
        for (Ty c : cont)
        {
            parents[c] = c;
        }
    }

    template<typename Y>
    Ty& get(Y&& x) 
    {
        auto deducedX = std::forward<Y>(x);
        return (parents[deducedX] == x) ? static_cast<Ty&>(x) : static_cast<Ty&>(parents[deducedX] = get(parents[deducedX]));
    }

    template<typename Y>
    void join(Y&& x, Y&& y) 
    {
        auto x1 = get(std::forward<Y>(x)); 
        auto y1 = get(std::forward<Y>(y));
        if (ranks[x1] > ranks[y1]) 
            std::swap(x1, y1);
        parents[x1] = y1;
        if (ranks[x1] == ranks[y1]) 
            ++ranks[y1];
    }

    ~DSU() = default;
};


