//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/14/23
//

#include <app/selection.hpp>
#include <app/algorithms.hpp>

namespace mousetrap
{
    Selection::Selection()
    {}

    bool Selection::at(Vector2i xy)
    {
         auto it = _data.find(xy.y);
         if (it == _data.end())
             return false;

         for (auto& pair : it->second)
             if (pair.first <= xy.x and pair.second >= xy.x)
                 return true;

         return false;
    }

    void Selection::create_from(const Vector2iSet& set)
    {
        int x_min = std::numeric_limits<int>::max();
        int x_max = std::numeric_limits<int>::min();
        int y_min = std::numeric_limits<int>::max();
        int y_max = std::numeric_limits<int>::min();

        for (const auto& vec : set)
        {
            if (vec.x <= x_min)
                x_min = vec.x;

            if (vec.x >= x_max)
                x_max = vec.x;

            if (vec.y <= y_min)
                y_min = vec.y;

            if (vec.y >= y_max)
                y_max = vec.y;
        }

        _data.clear();
        for (int y = y_min; y <= y_max; ++y)
        {
            std::vector<std::pair<int, int>> vec = {};
            for (int x = x_min; x <= x_max; ++x)
            {
                auto it = set.find({x, y});
                if (it == set.end())
                    continue;

                if (vec.empty())
                    vec.push_back({x, x});
                else if (x == vec.back().second + 1)
                    vec.back().second += 1;
                else
                    vec.push_back({x, x});
            }

            if (not vec.empty())
                _data[y] = vec;
        }
    }

    void Selection::create_from_rectangle(Vector2i top_left, Vector2i size)
    {
        _data.clear();
        for (int y = top_left.y; y < top_left.y + size.y; ++y)
            _data.insert({y, {{top_left.x, top_left.x + size.x}}});
    }
}